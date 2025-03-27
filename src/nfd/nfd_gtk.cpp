/*
  Native File Dialog

  http://www.frogtoss.com/labs
*/

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <X11/Xlib.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "nfd.h"
#include "nfd_common.h"


const char INIT_FAIL_MSG[] = "gtk_init_check failed to initilaize GTK+";


static void AddTypeToFilterName( const char *typebuf, char *filterName, size_t bufsize )
{
    const char SEP[] = ", ";

    size_t len = strlen(filterName);
    if ( len != 0 )
    {
        strncat( filterName, SEP, bufsize - len - 1 );
        len += strlen(SEP);
    }
    
    strncat( filterName, typebuf, bufsize - len - 1 );
}

static void AddFiltersToDialog( GtkWidget *dialog, const char *filterList )
{
    GtkFileFilter *filter;
    char typebuf[NFD_MAX_STRLEN] = {0};
    const char *p_filterList = filterList;
    char *p_typebuf = typebuf;
    char filterName[NFD_MAX_STRLEN] = {0};
    
    if ( !filterList || strlen(filterList) == 0 )
        return;

    filter = gtk_file_filter_new();
    while ( 1 )
    {
        
        if ( NFDi_IsFilterSegmentChar(*p_filterList) )
        {
            char typebufWildcard[NFD_MAX_STRLEN];
            /* add another type to the filter */
            assert( strlen(typebuf) > 0 );
            assert( strlen(typebuf) < NFD_MAX_STRLEN-1 );
            
            snprintf( typebufWildcard, NFD_MAX_STRLEN, "*.%s", typebuf );
            AddTypeToFilterName( typebuf, filterName, NFD_MAX_STRLEN );
            
            gtk_file_filter_add_pattern( filter, typebufWildcard );
            
            p_typebuf = typebuf;
            memset( typebuf, 0, sizeof(char) * NFD_MAX_STRLEN );
        }
        
        if ( *p_filterList == ';' || *p_filterList == '\0' )
        {
            /* end of filter -- add it to the dialog */
            
            gtk_file_filter_set_name( filter, filterName );
            gtk_file_chooser_add_filter( GTK_FILE_CHOOSER(dialog), filter );

            filterName[0] = '\0';

            if ( *p_filterList == '\0' )
                break;

            filter = gtk_file_filter_new();            
        }

        if ( !NFDi_IsFilterSegmentChar( *p_filterList ) )
        {
            *p_typebuf = *p_filterList;
            p_typebuf++;
        }

        p_filterList++;
    }
    
    /* always append a wildcard option to the end*/

    filter = gtk_file_filter_new();
    gtk_file_filter_set_name( filter, "*.*" );
    gtk_file_filter_add_pattern( filter, "*" );
    gtk_file_chooser_add_filter( GTK_FILE_CHOOSER(dialog), filter );
}

static void SetDefaultPath( GtkWidget *dialog, const char *defaultPath )
{
    if ( !defaultPath || strlen(defaultPath) == 0 )
        return;

    /* GTK+ manual recommends not specifically setting the default path.
       We do it anyway in order to be consistent across platforms.

       If consistency with the native OS is preferred, this is the line
       to comment out. -ml */
    gtk_file_chooser_set_current_folder( GTK_FILE_CHOOSER(dialog), defaultPath );
}

static nfdresult_t AllocPathSet( GSList *fileList, nfdpathset_t *pathSet )
{
    size_t bufSize = 0;
    GSList *node;
    nfdchar_t *p_buf;
    size_t count = 0;
    
    assert(fileList);
    assert(pathSet);

    pathSet->count = (size_t)g_slist_length( fileList );
    assert( pathSet->count > 0 );

    pathSet->indices = (size_t*) NFDi_Malloc( sizeof(size_t)*pathSet->count );
    if ( !pathSet->indices )
    {
        return NFD_ERROR;
    }

    /* count the total space needed for buf */
    for ( node = fileList; node; node = node->next )
    {
        assert(node->data);
        bufSize += strlen( (const gchar*)node->data ) + 1;
    }

    pathSet->buf = (nfdchar_t *)NFDi_Malloc( sizeof(nfdchar_t) * bufSize );

    /* fill buf */
    p_buf = pathSet->buf;
    for ( node = fileList; node; node = node->next )
    {
        nfdchar_t *path = (nfdchar_t*)(node->data);
        size_t byteLen = strlen(path)+1;
        ptrdiff_t index;
        
        memcpy( p_buf, path, byteLen );
        g_free(node->data);

        index = p_buf - pathSet->buf;
        assert( index >= 0 );
        pathSet->indices[count] = (size_t)index;

        p_buf += byteLen;
        ++count;
    }

    g_slist_free( fileList );
    
    return NFD_OKAY;
}

static void WaitForCleanup(void)
{
    while (gtk_events_pending())
        gtk_main_iteration();
}

typedef struct DialogResponse {
    std::string response;
    std::mutex mtx;
    std::condition_variable cv;
    bool ready;

    DialogResponse() : response(), mtx(), cv(), ready(false) {
    }

    void received(const char* fileName) {
        if (fileName == nullptr) {
            response = "";
        } else {
            response = fileName;
        }

        {
            std::lock_guard<std::mutex> lock(mtx);
            ready = true;
        }
        cv.notify_one();
    }
} DialogResponse;

void on_dialog_realize(GtkWidget* widget, gpointer data) {
    // Get the native window ID from GTK
    GdkWindow* gdk_win = gtk_widget_get_window(widget);
    if (gdk_win && data != NULL) {
        Window x11_window = (Window)(data);
        Window gtk_xid = GDK_WINDOW_XID(gdk_win);
        gdk_window_set_transient_for(gdk_win, gdk_x11_window_foreign_new_for_display(gdk_display_get_default(), x11_window));
    }
}

void on_dialog_response(GtkDialog* dialog, gint response_id, gpointer data) {
    DialogResponse* response = (DialogResponse*)data;
    if (response_id == GTK_RESPONSE_ACCEPT) {
        char* file_name = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        response->received(file_name);
        g_free(file_name);
    } else {
        response->received(nullptr);
    }

    gtk_widget_destroy(GTK_WIDGET(dialog));
}

void on_dialog_multiple_response(GtkDialog* dialog, gint response_id, gpointer data) {
    DialogResponse* response = (DialogResponse*)data;
    if (response_id == GTK_RESPONSE_ACCEPT) {
        GSList *fileList = gtk_file_chooser_get_filenames( GTK_FILE_CHOOSER(dialog) );

        nfdpathset_t outPaths;
        if ( AllocPathSet( fileList, &outPaths ) == NFD_ERROR )
        {
            response->received(nullptr);
        }
        else
        {
            std::string nfdResult;
            int count = NFD_PathSet_GetCount(&outPaths);
            for (int i = 0; i < count; ++i) {
                if (i > 0) nfdResult += ",";
                nfdResult += NFD_PathSet_GetPath(&outPaths, i);
            }
            NFD_PathSet_Free(&outPaths);
            response->received(nfdResult.c_str());
        }

    } else {
        response->received(nullptr);
    }

    gtk_widget_destroy(GTK_WIDGET(dialog));
}

/* public */

void NFD_Loop() {
    while (gtk_events_pending()) {
        gtk_main_iteration();
    }
}

std::string NFD_OpenDialog( const nfdchar_t *filterList,
                            const nfdchar_t *defaultPath,
                            void* parentWindow )
{    
    GtkWidget *dialog;

    if ( !gtk_init_check( NULL, NULL ) )
    {
        NFDi_SetError(INIT_FAIL_MSG);
        return "";
    }

    dialog = gtk_file_chooser_dialog_new( "Open File",
                                          NULL,
                                          GTK_FILE_CHOOSER_ACTION_OPEN,
                                          "_Cancel", GTK_RESPONSE_CANCEL,
                                          "_Open", GTK_RESPONSE_ACCEPT,
                                          NULL );

    /* Build the filter list */
    AddFiltersToDialog(dialog, filterList);

    /* Set the default path */
    SetDefaultPath(dialog, defaultPath);

    DialogResponse dialogResponse = {};

    g_signal_connect(dialog, "realize", G_CALLBACK(on_dialog_realize), parentWindow);
    g_signal_connect(dialog, "response", G_CALLBACK(on_dialog_response), &dialogResponse);

    gtk_widget_show_all(dialog);

    std::unique_lock<std::mutex> lock(dialogResponse.mtx);
    dialogResponse.cv.wait(lock, [&] { return dialogResponse.ready; });

    return dialogResponse.response;
}


std::string NFD_OpenDialogMultiple( const nfdchar_t *filterList,
                                    const nfdchar_t *defaultPath,
                                    void* parentWindow )
{
    GtkWidget *dialog;

    if ( !gtk_init_check( NULL, NULL ) )
    {
        NFDi_SetError(INIT_FAIL_MSG);
        return "";
    }

    dialog = gtk_file_chooser_dialog_new( "Open Files",
                                          NULL,
                                          GTK_FILE_CHOOSER_ACTION_OPEN,
                                          "_Cancel", GTK_RESPONSE_CANCEL,
                                          "_Open", GTK_RESPONSE_ACCEPT,
                                          NULL );

    gtk_file_chooser_set_select_multiple( GTK_FILE_CHOOSER(dialog), TRUE );

    /* Build the filter list */
    AddFiltersToDialog(dialog, filterList);

    /* Set the default path */
    SetDefaultPath(dialog, defaultPath);

    DialogResponse dialogResponse = {};

    g_signal_connect(dialog, "realize", G_CALLBACK(on_dialog_realize), parentWindow);
    g_signal_connect(dialog, "response", G_CALLBACK(on_dialog_multiple_response), &dialogResponse);

    gtk_widget_show_all(dialog);

    std::unique_lock<std::mutex> lock(dialogResponse.mtx);
    dialogResponse.cv.wait(lock, [&] { return dialogResponse.ready; });

    return dialogResponse.response;
}

std::string NFD_SaveDialog( const nfdchar_t *filterList,
                            const nfdchar_t *defaultPath,
                            void* parentWindow )
{
    GtkWidget *dialog;

    if ( !gtk_init_check( NULL, NULL ) )
    {
        NFDi_SetError(INIT_FAIL_MSG);
        return "";
    }

    dialog = gtk_file_chooser_dialog_new( "Save File",
                                          NULL,
                                          GTK_FILE_CHOOSER_ACTION_SAVE,
                                          "_Cancel", GTK_RESPONSE_CANCEL,
                                          "_Save", GTK_RESPONSE_ACCEPT,
                                          NULL );

    gtk_file_chooser_set_do_overwrite_confirmation( GTK_FILE_CHOOSER(dialog), TRUE );

    /* Build the filter list */    
    AddFiltersToDialog(dialog, filterList);

    /* Set the default path */
    SetDefaultPath(dialog, defaultPath);

    DialogResponse dialogResponse = {};

    g_signal_connect(dialog, "realize", G_CALLBACK(on_dialog_realize), parentWindow);
    g_signal_connect(dialog, "response", G_CALLBACK(on_dialog_response), &dialogResponse);

    gtk_widget_show_all(dialog);

    std::unique_lock<std::mutex> lock(dialogResponse.mtx);
    dialogResponse.cv.wait(lock, [&] { return dialogResponse.ready; });

    return dialogResponse.response;
}

std::string NFD_PickFolder( const nfdchar_t *defaultPath,
                            void* parentWindow )
{
    GtkWidget *dialog;

    if (!gtk_init_check(NULL, NULL))
    {
        NFDi_SetError(INIT_FAIL_MSG);
        return "";
    }

    dialog = gtk_file_chooser_dialog_new( "Select folder",
                                          NULL,
                                          GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
                                          "_Cancel", GTK_RESPONSE_CANCEL,
                                          "_Select", GTK_RESPONSE_ACCEPT,
                                          NULL );

    gtk_file_chooser_set_do_overwrite_confirmation( GTK_FILE_CHOOSER(dialog), TRUE );

    /* Set the default path */
    SetDefaultPath(dialog, defaultPath);

    DialogResponse dialogResponse = {};

    g_signal_connect(dialog, "realize", G_CALLBACK(on_dialog_realize), parentWindow);
    g_signal_connect(dialog, "response", G_CALLBACK(on_dialog_response), &dialogResponse);

    gtk_widget_show_all(dialog);

    std::unique_lock<std::mutex> lock(dialogResponse.mtx);
    dialogResponse.cv.wait(lock, [&] { return dialogResponse.ready; });

    return dialogResponse.response;
}
