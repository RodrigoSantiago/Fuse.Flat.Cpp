/*
  Native File Dialog

  User API

  http://www.frogtoss.com/labs
 */


#ifndef _NFD_H
#define _NFD_H

#include <iostream>
#include <stddef.h>

/* denotes UTF-8 char */
typedef char nfdchar_t;

/* opaque data structure -- see NFD_PathSet_* */
typedef struct {
    nfdchar_t *buf;
    size_t *indices; /* byte offsets into buf */
    size_t count;    /* number of indices into buf */
}nfdpathset_t;

typedef enum {
    NFD_ERROR,       /* programmatic error */
    NFD_OKAY,        /* user pressed okay, or successful return */
    NFD_CANCEL      /* user pressed cancel */
}nfdresult_t;


/* some system need a thread modal loop */
void NFD_Loop();

/* single file open dialog */    
std::string NFD_OpenDialog( const nfdchar_t *filterList,
                            const nfdchar_t *defaultPath,
                            void* parentWindow);

/* multiple file open dialog */
std::string NFD_OpenDialogMultiple( const nfdchar_t *filterList,
                                    const nfdchar_t *defaultPath,
                                    void* parentWindow );

/* save dialog */
std::string NFD_SaveDialog( const nfdchar_t *filterList,
                            const nfdchar_t *defaultPath,
                            void* parentWindow );


/* select folder dialog */
std::string NFD_PickFolder( const nfdchar_t *defaultPath,
                            void* parentWindow );

/* nfd_common.c */

/* get last error -- set when nfdresult_t returns NFD_ERROR */
const char *NFD_GetError( void );
/* get the number of entries stored in pathSet */
size_t      NFD_PathSet_GetCount( const nfdpathset_t *pathSet );
/* Get the UTF-8 path at offset index */
nfdchar_t  *NFD_PathSet_GetPath( const nfdpathset_t *pathSet, size_t index );
/* Free the pathSet */    
void        NFD_PathSet_Free( nfdpathset_t *pathSet );

#endif
