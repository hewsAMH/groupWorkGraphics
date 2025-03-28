#include "View.h"
#include <cstdio>
#include <cstdlib>
#include <vector>
using namespace std;
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "sgraph/GLScenegraphRenderer.h"
#include "VertexAttrib.h"


View::View() {

}

View::~View(){

}

void View::init(Callbacks *callbacks,map<string,util::PolygonMesh<VertexAttrib>>& meshes) 
{
    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    window = glfwCreateWindow(800, 800, "Hello GLFW: Per-vertex coloring", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
     glfwSetWindowUserPointer(window, (void *)callbacks);

    //using C++ functions as callbacks to a C-style library
    glfwSetKeyCallback(window, 
    [](GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        reinterpret_cast<Callbacks*>(glfwGetWindowUserPointer(window))->onkey(key,scancode,action,mods);
    });

    GLFWcursor* cursor = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
    glfwSetCursor(window, cursor);

    this->thetaX = 0.0f;
    this->thetaY = glm::radians(30.0f);

    glfwSetMouseButtonCallback(window,
    [](GLFWwindow* window, int button, int action, int mods)
    {
        reinterpret_cast<Callbacks*>(glfwGetWindowUserPointer(window))->onmouse(button,action,mods);
    });

    glfwSetWindowSizeCallback(window, 
    [](GLFWwindow* window, int width,int height)
    {
        reinterpret_cast<Callbacks*>(glfwGetWindowUserPointer(window))->reshape(width,height);
    });

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);

    // create the shader program
    program.createProgram(string("shaders/default.vert"),
                          string("shaders/default.frag"));
    // assuming it got created, get all the shader variables that it uses
    // so we can initialize them at some point
    // enable the shader program
    program.enable();
    shaderLocations = program.getAllShaderVariables();

    
    /* In the mesh, we have some attributes for each vertex. In the shader
     * we have variables for each vertex attribute. We have to provide a mapping
     * between attribute name in the mesh and corresponding shader variable
     name.
     *
     * This will allow us to use PolygonMesh with any shader program, without
     * assuming that the attribute names in the mesh and the names of
     * shader variables will be the same.

       We create such a shader variable -> vertex attribute mapping now
     */
    map<string, string> shaderVarsToVertexAttribs;

    shaderVarsToVertexAttribs["vPosition"] = "position";
    
    
    for (typename map<string,util::PolygonMesh<VertexAttrib> >::iterator it=meshes.begin();
           it!=meshes.end();
           it++) {
        util::ObjectInstance * obj = new util::ObjectInstance(it->first);
        obj->initPolygonMesh(shaderLocations,shaderVarsToVertexAttribs,it->second);
        objects[it->first] = obj;
    }
    
	int window_width,window_height;
    glfwGetFramebufferSize(window,&window_width,&window_height);

    //prepare the projection matrix for perspective projection
	projection = glm::perspective(glm::radians(60.0f),(float)window_width/window_height,0.1f,10000.0f);
    glViewport(0, 0, window_width,window_height);

    frames = 0;
    time = glfwGetTime();

    renderer = new sgraph::GLScenegraphRenderer(modelview,objects,shaderLocations);
    
}

// getter due to keeping window private
void View::getWindowScalars(float *scaleX, float *scaleY) {
    glfwGetWindowContentScale(window,scaleX,scaleY);
}

// getter due to keeping window private
void View::getCursorPosn(double *xpos, double *ypos) {
    glfwGetCursorPos(this->window, xpos, ypos);
}

// called externally on keypress
void View::resetRotation() {
    this->thetaX = 0.0f;
    this->thetaY = glm::radians(30.0f);
    this->upVal = 1;
}

// rotates the trackball on an axis by some radians delta
void View::adjustRotation(char axis, float delta) {
    if (axis == 'x') {
        this->thetaX += delta;
    } else if (axis == 'y') {
        // gimbal lock portion
        while (this->thetaY < 0.0f) this->thetaY+=glm::radians(360.0f);
        float thetaYDegBefore = fmod(glm::degrees(this->thetaY), 360.0f);
        float thetaYDegAfter = fmod(glm::degrees(this->thetaY + delta), 360.0f);
        if (
        ((thetaYDegBefore >= 90.0f && thetaYDegAfter < 90.0f)
            || (thetaYDegAfter >= 90.0f && thetaYDegBefore < 90.0f)
            || (thetaYDegBefore >= 270.0f && thetaYDegAfter < 270.0f)
            || (thetaYDegAfter >= 270.0f && thetaYDegBefore < 270.0f))
        && !(  (thetaYDegBefore < 90.0f && thetaYDegAfter >= 270.0f)
            || (thetaYDegAfter < 90.0f && thetaYDegBefore >= 270.0f))
        )
        {
            // std::cout << "correcting gimbal lock for " << thetaYDegBefore << " to " << thetaYDegAfter << std::endl;
            this->upVal = -this->upVal;
        }
        this->thetaY += delta;
    }
    // std::cout << "wanting to adjust in axis " << axis << " by delta " << delta << std::endl;
    // std::cout << "our x theta is now " << glm::degrees(this->thetaX) << " and y theta is now " << glm::degrees(this->thetaY) << std::endl;
    return;
}

void View::display(sgraph::IScenegraph *scenegraph) {
    
    program.enable();
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_FRONT_FACE);

    
    
    modelview.push(glm::mat4(1.0));
    float radiusView = 500.0f;
    // rotating our trackball
    glm::vec3 vRotated = {0, 0, 0};
    vRotated.x = radiusView * cos(this->thetaY) * sin(this->thetaX);
    vRotated.y = radiusView * sin(this->thetaY);
    vRotated.z = radiusView * cos(this->thetaY) * cos(this->thetaX);
    glm::vec3 up = {0, this->upVal, 0};
    modelview.top() = modelview.top() * glm::lookAt(
                                            vRotated,
                                            glm::vec3(0.0f, 0.0f, 0.0f),
                                            up
                                        );
    // send projection matrix to GPU
    glUniformMatrix4fv(shaderLocations.getLocation("projection"), 1, GL_FALSE, glm::value_ptr(projection));
    

    //draw scene graph here
    scenegraph->getRoot()->accept(renderer);

    
    
    modelview.pop();
    glFlush();
    program.disable();
    
    glfwSwapBuffers(window);
    glfwPollEvents();
    frames++;
    double currenttime = glfwGetTime();
    if ((currenttime-time)>1.0) {
        printf("Framerate: %2.0f\r",frames/(currenttime-time));
        frames = 0;
        time = currenttime;
    }
    

}

bool View::shouldWindowClose() {
    return glfwWindowShouldClose(window);
}

void View::closeWindow() {
    for (map<string,util::ObjectInstance *>::iterator it=objects.begin();
           it!=objects.end();
           it++) {
          it->second->cleanup();
          delete it->second;
    } 
    glfwDestroyWindow(window);

    glfwTerminate();
}





