#include "View.h"
#include "TextureImage.h"
#include <cstdio>
#include <cstdlib>
#include <vector>
using namespace std;
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "sgraph/GLScenegraphRenderer.h"
#include "VertexAttrib.h"

View::View() {}

View::~View() {}

void View::initGlfw()
{
    if (!glfwInit())
        exit(EXIT_FAILURE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    this->window = glfwCreateWindow(800, 800, "3D Scene with Lighting and Textures", NULL, NULL);
    if (!this->window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(this->window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);
}

void View::initCallbacks(Callbacks *callbacks)
{
    glfwSetWindowUserPointer(this->window, (void *)callbacks);
    // using C++ functions as callbacks to a C-style library
    glfwSetKeyCallback(this->window, [](GLFWwindow *window, int key, int scancode, int action, int mods)
                       { reinterpret_cast<Callbacks *>(glfwGetWindowUserPointer(window))->onkey(key, scancode, action, mods); });
    GLFWcursor *cursor = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
    glfwSetCursor(this->window, cursor);
    glfwSetMouseButtonCallback(this->window, [](GLFWwindow *window, int button, int action, int mods)
                               { reinterpret_cast<Callbacks *>(glfwGetWindowUserPointer(window))->onmouse(button, action, mods); });
    glfwSetWindowSizeCallback(this->window, [](GLFWwindow *window, int width, int height)
                              { reinterpret_cast<Callbacks *>(glfwGetWindowUserPointer(window))->reshape(width, height); });
}

void View::init(Callbacks *callbacks, map<string, util::PolygonMesh<VertexAttrib>> &meshes, map<string, util::TextureImage> &images)
{
    this->initGlfw();
    this->initCallbacks(callbacks);

    // create the shader program with support for lighting and textures
    program.createProgram(string("shaders/phong-multiple.vert"),
                          string("shaders/phong-multiple.frag"));
    program.enable();
    shaderLocations = program.getAllShaderVariables();

    // Associate shader variables with vertex attributes
    map<string, string> shaderVarsToVertexAttribs;
    shaderVarsToVertexAttribs["vPosition"] = "position";
    shaderVarsToVertexAttribs["vNormal"] = "normal";
    shaderVarsToVertexAttribs["vTexCoord"] = "texcoord";

    // Initialize object instances
    for (typename map<string, util::PolygonMesh<VertexAttrib>>::iterator it = meshes.begin();
         it != meshes.end();
         it++)
    {
        util::ObjectInstance *obj = new util::ObjectInstance(it->first);
        obj->initPolygonMesh(shaderLocations, shaderVarsToVertexAttribs, it->second);
        objects[it->first] = obj;
    }

    // Get window dimensions for projection matrix
    int window_width, window_height;
    glfwGetFramebufferSize(window, &window_width, &window_height);

    // Prepare the projection matrix for perspective projection
    projection = glm::perspective(glm::radians(60.0f),
                                  (float)window_width / window_height,
                                  0.1f, 10000.0f);
    glViewport(0, 0, window_width, window_height);

    // Initialize rendering
    frames = 0;
    time = glfwGetTime();

    // Initialize camera position
    this->thetaX = 0.0f;
    this->thetaY = glm::radians(30.0f);

    // Create the scene graph renderer with textures
    renderer = new sgraph::GLScenegraphRenderer(modelview, objects, images, shaderLocations);

    // Configure OpenGL for 3D rendering
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}

void View::display(sgraph::IScenegraph *scenegraph)
{
    program.enable();
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f); // Dark gray background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set up the view matrix
    modelview.push(glm::mat4(1.0));
    float radiusView = 500.0f;

    // Calculate camera position based on trackball rotation
    glm::vec3 vRotated = {0, 0, 0};
    vRotated.x = radiusView * cos(this->thetaY) * sin(this->thetaX);
    vRotated.y = radiusView * sin(this->thetaY);
    vRotated.z = radiusView * cos(this->thetaY) * cos(this->thetaX);
    glm::vec3 up = {0, this->upVal, 0};

    // Create the view matrix
    glm::mat4 viewMatrix = glm::lookAt(
        vRotated,
        glm::vec3(0.0f, 0.0f, 0.0f), // Look at origin
        up);

    modelview.top() = modelview.top() * viewMatrix;

    // Send projection matrix to GPU
    glUniformMatrix4fv(
        shaderLocations.getLocation("projection"),
        1,
        GL_FALSE,
        glm::value_ptr(projection));

    // Get all lights in view space
    vector<util::Light> lightsInViewSpace = scenegraph->getAllLightsInViewSpace(viewMatrix);

    // Set lights in the renderer
    sgraph::GLScenegraphRenderer *glRenderer = dynamic_cast<sgraph::GLScenegraphRenderer *>(renderer);
    if (glRenderer != nullptr)
    {
        glRenderer->setLights(lightsInViewSpace);
    }

    // Draw the scene graph
    scenegraph->getRoot()->accept(renderer);

    // Clean up
    modelview.pop();
    glFlush();
    program.disable();

    // Swap buffers and handle events
    glfwSwapBuffers(window);
    glfwPollEvents();

    // Calculate framerate
    frames++;
    double currenttime = glfwGetTime();
    if ((currenttime - time) > 1.0)
    {
        printf("Framerate: %2.0f\r", frames / (currenttime - time));
        frames = 0;
        time = currenttime;
    }
}

void View::getWindowScalars(float *scaleX, float *scaleY)
{
    glfwGetWindowContentScale(window, scaleX, scaleY);
}

void View::getCursorPosn(double *xpos, double *ypos)
{
    glfwGetCursorPos(this->window, xpos, ypos);
}

void View::resetRotation()
{
    this->thetaX = 0.0f;
    this->thetaY = glm::radians(30.0f);
    this->upVal = 1;
}

void View::adjustRotation(char axis, float delta)
{
    if (axis == 'x')
    {
        this->thetaX += delta;
    }
    else if (axis == 'y')
    {
        // gimbal lock portion
        while (this->thetaY < 0.0f)
            this->thetaY += glm::radians(360.0f);
        float thetaYDegBefore = fmod(glm::degrees(this->thetaY), 360.0f);
        float thetaYDegAfter = fmod(glm::degrees(this->thetaY + delta), 360.0f);
        if (
            ((thetaYDegBefore >= 90.0f && thetaYDegAfter < 90.0f) || (thetaYDegAfter >= 90.0f && thetaYDegBefore < 90.0f) || (thetaYDegBefore >= 270.0f && thetaYDegAfter < 270.0f) || (thetaYDegAfter >= 270.0f && thetaYDegBefore < 270.0f)) && !((thetaYDegBefore < 90.0f && thetaYDegAfter >= 270.0f) || (thetaYDegAfter < 90.0f && thetaYDegBefore >= 270.0f)))
        {
            this->upVal = -this->upVal;
        }
        this->thetaY += delta;
    }
    return;
}

bool View::shouldWindowClose()
{
    return glfwWindowShouldClose(window);
}

void View::closeWindow()
{
    for (map<string, util::ObjectInstance *>::iterator it = objects.begin();
         it != objects.end();
         it++)
    {
        it->second->cleanup();
        delete it->second;
    }
    glfwDestroyWindow(window);

    glfwTerminate();
}

void View::setLogger(ourutils::Logger &logger)
{
    this->logger = logger;
}