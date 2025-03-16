#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include "src/flatvectors/FlatVectors.h"
#include "src/flatvectors/FlatFont.h"
#include "src/flatvectors/FlatFontRender.h"
#include "src/flatvectors/FlatPaints.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
unsigned int SCR_WIDTH = 1200;
unsigned int SCR_HEIGHT = 800;

unsigned char* readFile(const std::string& filename, std::size_t& fileSize) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    if (!file.is_open()) {
        std::cerr << "Font file error!" << std::endl;
        return nullptr;
    }

    fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    unsigned char* buffer = new unsigned char[fileSize];

    if (!file.read(reinterpret_cast<char*>(buffer), fileSize)) {
        std::cerr << "Font file error!" << std::endl;
        delete[] buffer;
        return nullptr;
    }

    file.close();
    return buffer;
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 8);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "FlatVectors Test", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    float vertices[] = {
            0.5f,  0.5f, 0.0f,  // top right
            0.5f, -0.5f, 0.0f,  // bottom right
            -0.5f, -0.5f, 0.0f,  // bottom left
            -0.5f,  0.5f, 0.0f   // top left
    };
    unsigned int indices[] = {  // note that we start from 0!
            0, 1, 3,  // first Triangle
            1, 2, 3   // second Triangle
    };

    unsigned int VBO, VAO, EBO, EBO2;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &EBO2);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    //glBindBuffer(GL_ARRAY_BUFFER, 0);

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);
    GLint id = 0;
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &id);
    std::cout << "bbe " << id << std::endl;
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &id);
    std::cout << "bba " << id << std::endl;
    glBindVertexArray(VAO);
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &id);
    std::cout << "bbe " << id << std::endl;
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &id);
    std::cout << "bba " << id << std::endl;
    glBindVertexArray(0);
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &id);
    std::cout << "bbe " << id << std::endl;
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &id);
    std::cout << "bba " << id << std::endl;

    /*if (trgBB == GL_ARRAY_BUFFER) {
        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &id);
    } else if (trgBB == GL_ELEMENT_ARRAY_BUFFER) {
        glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &id);
    }*/


    fvUniform color = {};
    FlatPaints::setColorPaint(color, 0xFF0000FF);

    size_t size = 0;
    unsigned char * fontFile = readFile("C:/Nova/Roboto-Regular.ttf", size);
    FlatVectors * fv = new FlatVectors();
    FlatFont* font = new FlatFont(fontFile, size, 48, true);

    FlatStroke stroke = FlatStroke();

    std::cout << "Font : " << font->isValid() << std::endl;
    FlatFontRender* fontRender = new FlatFontRender(font, fv->getRender());

    FlatVectors::setDebug(true);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    double x, y;
    int t = 0;
    glfwSwapInterval(0);
    while (!glfwWindowShouldClose(window)) {
        // input
        // -----
        processInput(window);
        glfwGetCursorPos((GLFWwindow*) window, &x, &y);
        float ix = static_cast<int>(x) / 2.0f;
        float iy = static_cast<int>(y) / 2.0f;

        // render
        // ------
        glClearColor(0.0, 0.0, 0.0, 1.0f);
        glClearStencil(0x00);
        glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        fv->setAntiAliasing(0);
        fv->setTransform(1, 0, 0, 1, 0, 0);
        FlatPaints::setColorPaint(color, 0xFF0000FF);
        fv->setColor(color, 0);
        fv->beginFrame(SCR_WIDTH / 2, SCR_HEIGHT / 2);

        fv->begin(CLIP, EVEN_ODD);
        fv->moveTo(25, 25);
        fv->lineTo(50, 25);
        fv->lineTo(50, 50);
        fv->lineTo(25, 50);
        fv->close();
        fv->moveTo(30, 30);
        fv->lineTo(45, 30);
        fv->lineTo(45, 45);
        fv->lineTo(30, 45);
        fv->close();
        fv->end();

        fv->begin(UNCLIP, EVEN_ODD);
        fv->moveTo(35, 35);
        fv->lineTo(60, 35);
        fv->lineTo(60, 60);
        fv->lineTo(35, 60);
        fv->close();
        fv->moveTo(40, 40);
        fv->lineTo(55, 40);
        fv->lineTo(55, 55);
        fv->lineTo(40, 55);
        fv->close();
        fv->end();

        fv->begin(FILL, EVEN_ODD);
        fv->moveTo(0, 0);
        fv->lineTo(0, 100);
        fv->lineTo(100, 100);
        fv->lineTo(100, 0);
        fv->close();
        fv->end();

        //fv->ellipse(300, 0, 100, 100);
        fv->roundRect(300, 0, 100, 100, 30, 10, 40, 20);

        FlatPaints::setColorPaint(color, 0xFFFF00FF);
        fv->setColor(color, 0);
        fv->setFont(fontRender);
        fv->text("ABCDEFGHIJKLMNOPQRSTUVWXYZ", 23, 10, 20, 0, 0);
        fv->text("abcdefghijklmnopqrstuvwxyz", 23, 10, 50, 0, 0);

        stroke = FlatStroke(
                10, 1.5, fvCap::CAP_BUTT, fvJoin::JOIN_BEVEL
        );
        fv->setStroke(stroke);
        fv->begin(FILL, EVEN_ODD);
        fv->moveTo(100, 100);
        fv->quadTo(50, 150, 100, 200);
        fv->lineTo(105, 150);
        fv->lineTo(150, 200);
        fv->flush();
        fv->lineTo(195, 150);
        fv->lineTo(200, 200);
        fv->cubicTo(150, 150, 300, 150, 200, 100);
        fv->close();
        fv->end();

        /*stroke = {
                10, 1.5, fvCap::CAP_BUTT, fvJoin::JOIN_MITER
        };
        fv->setStroke(stroke);
        fv->begin(STROKE, EVEN_ODD);
        fv->moveTo(100 + 150, 100);
        fv->lineTo(100 + 150, 200);
        fv->lineTo(105 + 150, 150);
        fv->lineTo(150 + 150, 200);
        fv->lineTo(195 + 150, 150);
        fv->lineTo(200 + 150, 200);
        fv->lineTo(200 + 150, 100);
        fv->close();
        fv->end();*/

        stroke = FlatStroke(
                10, 1.5, fvCap::CAP_ROUND, fvJoin::JOIN_BEVEL
        );
        stroke.dash.push_back(30);
        stroke.dash.push_back(30);
        fv->setStroke(stroke);
        fv->begin(STROKE, EVEN_ODD);
        // fv->moveTo(100 + 300, 100);
        fv->moveTo(100 + 150, 200);
        fv->cubicTo(200 + 150, 100, 100 + 150, 100, ix, iy);
        //fv->lineTo(200 + 300, 100);
        fv->end();

        stroke = FlatStroke(
                10, 1.5, fvCap::CAP_ROUND, fvJoin::JOIN_BEVEL
        );
        fv->setStroke(stroke);
        fv->begin(STROKE, EVEN_ODD);
        fv->moveTo(300 + 150, 200);
        fv->lineTo(300 + 300, 200);
        fv->close();
        fv->end();

        stroke = FlatStroke(
                20, 1.5, fvCap::CAP_ROUND, fvJoin::JOIN_MITER
        );
        fv->setStroke(stroke);
        fv->begin(STROKE, EVEN_ODD);
        fv->moveTo(200 + 300, 150);
        fv->end();

        stroke = FlatStroke(
                10, 1.5, fvCap::CAP_BUTT, fvJoin::JOIN_ROUND
        );
        fv->setStroke(stroke);
        fv->begin(STROKE, EVEN_ODD);
        fv->moveTo(100, 100 + 150);
        fv->lineTo(100, 200 + 150);
        fv->lineTo(105, 150 + 150);
        fv->lineTo(150, 200 + 150);
        fv->lineTo(195, 150 + 150);
        fv->lineTo(200, 200 + 150);
        fv->lineTo(200, 100 + 150);
        fv->close();
        fv->end();

        FlatPaints::setColorPaint(color, 0xFF0000FF);
        fv->setColor(color, 0);

        fv->setStroke(stroke);
        fv->begin(STROKE, EVEN_ODD);
        fv->moveTo(200 + 150, 100 + 150);
        fv->lineTo(200 + 150, 200 + 150);
        fv->lineTo(195 + 150, 150 + 150);
        fv->lineTo(150 + 150, 200 + 150);
        fv->lineTo(105 + 150, 150 + 150);
        fv->lineTo(100 + 150, 200 + 150);
        fv->lineTo(100 + 150, 100 + 150);
        fv->close();
        fv->end();

        stroke = FlatStroke(
                10, 10, fvCap::CAP_ROUND, fvJoin::JOIN_MITER
        );
        fv->setStroke(stroke);
        fv->begin(STROKE, EVEN_ODD);
        fv->moveTo(ix, iy);
        fv->lineTo(200 + 300, 200 + 150);
        fv->lineTo(195 + 300, 150 + 150);
        fv->lineTo(150 + 300, 200 + 150);
        fv->lineTo(105 + 300, 150 + 150);
        fv->lineTo(100 + 300, 200 + 150);
        fv->lineTo(100 + 300, 100 + 150);
        fv->end();

        fv->endFrame();
        int error = glGetError();
        if (error != 0) {
            std::cout << "GL Error: " << error << std::endl;
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    delete fontRender;
    delete font;
    delete fv;
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, SCR_WIDTH = width, SCR_HEIGHT = height);
}