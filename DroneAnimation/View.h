#ifndef VIEW_H
#define VIEW_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Controller.h"
#include <memory>

class View
{
public:
    View();
    ~View();

    bool init();
    void run();

private:
    //window
    GLFWwindow *window;

    //controller
    std::unique_ptr<Controller> controller;

    //shader program
    GLuint shaderProgram;

    //ground plane
    GLuint groundVAO, groundVBO;

    //helpers
    bool initGLFW();
    bool initGLEW();
    bool initShaders();
    void initGround();
    void render(float deltaTime);
    void cleanup();

    GLuint loadShaders(const char *vertexShaderPath, const char *fragmentShaderPath);
};

#endif // VIEW_H