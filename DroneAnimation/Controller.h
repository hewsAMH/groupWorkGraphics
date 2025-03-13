#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "Model.h"
#include <GLFW/glfw3.h>
#include <memory>

// cam. modes
enum CameraMode
{
    GLOBAL_CAMERA = 0,
    CHOPPER_CAMERA = 1,
    FIRST_PERSON_CAMERA = 2
};

class Controller
{
public:
    Controller();
    ~Controller();

    void init(GLFWwindow *window);
    void update(float deltaTime);
    void processInput(GLFWwindow *window, float deltaTime);

    Model &getDrone() { return *drone; }

    // control cam.
    void setCameraMode(CameraMode mode) { cameraMode = mode; }
    CameraMode getCameraMode() const { return cameraMode; }

    // get matricies for rendering
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix(float aspectRatio) const;

private:
    // drone mod.
    std::unique_ptr<Model> drone;

    // cam. properties
    CameraMode cameraMode;
    float globalCameraYaw;
    float chopperCameraAngle;

    bool keys[348];
    void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

    // cam. position helpers
    glm::vec3 getGlobalCameraPosition() const;
    glm::vec3 getChopperCameraPosition(float deltaTime) const;
    glm::vec3 getFirstPersonCameraPosition() const;
    glm::vec3 getGlobalCameraTarget() const;
    glm::vec3 getChopperCameraTarget() const;
    glm::vec3 getFirstPersonCameraTarget() const;

    static void key_callback_wrapper(GLFWwindow *window, int key, int scancode, int action, int mods);
};

#endif