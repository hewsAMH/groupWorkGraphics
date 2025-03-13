#include "View.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

View::View() : window(nullptr), shaderProgram(0), groundVAO(0), groundVBO(0)
{
    controller = std::make_unique<Controller>();
}

View::~View()
{
    cleanup();
}

bool View::init()
{
    if (!initGLFW() || !initGLEW())
    {
        return false;
    }

    //initializee shaders
    if (!initShaders())
    {
        return false;
    }

    //initialize ground plane
    initGround();

    //initialize controller
    controller->init(window);

    //configure OpenGL settings
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    return true;
}

bool View::initGLFW()
{
    //initialize GLFW
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    //configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    window = glfwCreateWindow(800, 600, "Drone Simulation", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);

    return true;
}

bool View::initGLEW()
{
    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return false;
    }

    return true;
}

//vertex shader for view
const char *vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 fragColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    fragColor = aColor;
}
)";

//frag shader for view
const char *fragmentShaderSource = R"(
#version 330 core
in vec3 fragColor;
out vec4 FragColor;

void main() {
    FragColor = vec4(fragColor, 1.0);
}
)";

bool View::initShaders()
{
    //complie shaders
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    //failsafe for shader errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "Vertex shader compilation failed: " << infoLog << std::endl;
        return false;
    }

    //compile frag shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    //check for frag shader errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "Fragment shader compilation failed: " << infoLog << std::endl;
        return false;
    }

    //initalize shader program
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    //error checking for shader
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "Shader program linking failed: " << infoLog << std::endl;
        return false;
    }

    //cleanup - deelte used shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return true;
}

void View::initGround()
{
    std::vector<GLfloat> groundVertices;

    //ground plane
    float size = 20.0f;
    float gridSize = 1.0f;
    float gridColor[3] = {0.0f, 0.3f, 0.0f};

    //could mqake a helper to factor this out more for DRY
    // horizontal grid lines
    //x,y,z,r,g,b
    for (float i = -size; i <= size; i += gridSize)
    {
        groundVertices.push_back(-size);
        groundVertices.push_back(0.0f);
        groundVertices.push_back(i);
        groundVertices.push_back(gridColor[0]);
        groundVertices.push_back(gridColor[1]);
        groundVertices.push_back(gridColor[2]);

        groundVertices.push_back(size);
        groundVertices.push_back(0.0f);
        groundVertices.push_back(i);
        groundVertices.push_back(gridColor[0]);
        groundVertices.push_back(gridColor[1]);
        groundVertices.push_back(gridColor[2]);
    }

    // vertical grid lines
    for (float i = -size; i <= size; i += gridSize)
    {
        groundVertices.push_back(i);
        groundVertices.push_back(0.0f);
        groundVertices.push_back(-size);
        groundVertices.push_back(gridColor[0]);
        groundVertices.push_back(gridColor[1]);
        groundVertices.push_back(gridColor[2]);

        groundVertices.push_back(i);
        groundVertices.push_back(0.0f);
        groundVertices.push_back(size);
        groundVertices.push_back(gridColor[0]);
        groundVertices.push_back(gridColor[1]);
        groundVertices.push_back(gridColor[2]);
    }

    //center marker
    float markerColor[3] = {1.0f, 0.0f, 0.0f};

    //x
    groundVertices.push_back(0.0f);
    groundVertices.push_back(0.0f);
    groundVertices.push_back(0.0f);
    groundVertices.push_back(1.0f);
    groundVertices.push_back(0.0f);
    groundVertices.push_back(0.0f);

    groundVertices.push_back(2.0f);
    groundVertices.push_back(0.0f);
    groundVertices.push_back(0.0f);
    groundVertices.push_back(1.0f);
    groundVertices.push_back(0.0f);
    groundVertices.push_back(0.0f);

    //z
    groundVertices.push_back(0.0f);
    groundVertices.push_back(0.0f);
    groundVertices.push_back(0.0f);
    groundVertices.push_back(0.0f);
    groundVertices.push_back(0.0f);
    groundVertices.push_back(1.0f);

    groundVertices.push_back(0.0f);
    groundVertices.push_back(0.0f);
    groundVertices.push_back(2.0f);
    groundVertices.push_back(0.0f);
    groundVertices.push_back(0.0f);
    groundVertices.push_back(1.0f);

    //y
    groundVertices.push_back(0.0f);
    groundVertices.push_back(0.0f);
    groundVertices.push_back(0.0f);
    groundVertices.push_back(0.0f);
    groundVertices.push_back(1.0f);
    groundVertices.push_back(0.0f);

    groundVertices.push_back(0.0f);
    groundVertices.push_back(2.0f);
    groundVertices.push_back(0.0f);
    groundVertices.push_back(0.0f);
    groundVertices.push_back(1.0f);
    groundVertices.push_back(0.0f);

    //buffers
    glGenVertexArrays(1, &groundVAO);
    glGenBuffers(1, &groundVBO);

    glBindVertexArray(groundVAO);
    glBindBuffer(GL_ARRAY_BUFFER, groundVBO);
    glBufferData(GL_ARRAY_BUFFER, groundVertices.size() * sizeof(GLfloat), groundVertices.data(), GL_STATIC_DRAW);

    //position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void *)0);
    glEnableVertexAttribArray(0);

    //color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void View::run()
{

    float lastFrameTime = 0.0f;

    // main render loop
    while (!glfwWindowShouldClose(window))
    {

        float currentTime = glfwGetTime();
        float deltaTime = currentTime - lastFrameTime;
        lastFrameTime = currentTime;
        controller->processInput(window, deltaTime);
        controller->update(deltaTime);
        render(deltaTime);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void View::render(float deltaTime)
{
    //clear screen
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgram);

    //get window size
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    float aspectRatio = (float)width / (float)height;

    //view / projection matrices from controller
    glm::mat4 viewMatrix = controller->getViewMatrix();
    glm::mat4 projectionMatrix = controller->getProjectionMatrix(aspectRatio);

    //Set vals for view and projection matrices
    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // Draw ground plane
    glm::mat4 groundModel = glm::mat4(1.0f);
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(groundModel));

    glBindVertexArray(groundVAO);

    int numGridLines = int(2 * 20.0f / 1.0f) * 2 + 2;
    glDrawArrays(GL_LINES, 0, numGridLines * 2);
    glDrawArrays(GL_LINES, numGridLines * 2, 6);
    glBindVertexArray(0);
    controller->getDrone().render(shaderProgram);
}

void View::cleanup()
{
    if (groundVAO)
        glDeleteVertexArrays(1, &groundVAO);
    if (groundVBO)
        glDeleteBuffers(1, &groundVBO);
    if (shaderProgram)
        glDeleteProgram(shaderProgram);

    if (window)
    {
        glfwDestroyWindow(window);
    }
    glfwTerminate();
}