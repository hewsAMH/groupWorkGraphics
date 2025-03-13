#include "Controller.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

Controller *currentController = nullptr;

Controller::Controller()
    : cameraMode(GLOBAL_CAMERA),
      globalCameraYaw(0.0f),
      chopperCameraAngle(0.0f)
{
    //initialize the drone
    drone = std::make_unique<Model>();

    //reset the keys 
    for (int i = 0; i < 348; i++)
    {
        keys[i] = false;
    }

    currentController = this;
}

Controller::~Controller()
{
    //reset static pointer to default
    if (currentController == this)
    {
        currentController = nullptr;
    }
}

void Controller::init(GLFWwindow *window)
{
    glfwSetKeyCallback(window, key_callback_wrapper);

    //reset position
    drone->resetPosition();
}

void Controller::key_callback_wrapper(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (currentController)
    {
        currentController->key_callback(window, key, scancode, action, mods);
    }
}

void Controller::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    //update the current key
    if (key >= 0 && key < 348)
    {
        if (action == GLFW_PRESS)
        {
            keys[key] = true;
        }
        else if (action == GLFW_RELEASE)
        {
            keys[key] = false;
        }
    }

    //handle key presses
    if (action == GLFW_PRESS)
    {
        switch (key)
        {
        case GLFW_KEY_1:
            cameraMode = GLOBAL_CAMERA;
            break;
        case GLFW_KEY_2:
            cameraMode = CHOPPER_CAMERA;
            break;
        case GLFW_KEY_3:
            cameraMode = FIRST_PERSON_CAMERA;
            break;
        case GLFW_KEY_J:
            drone->startRoll();
            break;
        case GLFW_KEY_D:
            drone->resetPosition();
            break;
        case GLFW_KEY_S:
            drone->setPropellerSpeed(std::max(1.0f, drone->getPropellerSpeed() - 1.0f));
            std::cout << "Propeller speed: " << drone->getPropellerSpeed() << std::endl;
            break;
        case GLFW_KEY_F:
            drone->setPropellerSpeed(std::min(15.0f, drone->getPropellerSpeed() + 1.0f));
            std::cout << "Propeller speed: " << drone->getPropellerSpeed() << std::endl;
            break;
        }
    }
}

void Controller::update(float deltaTime)
{
    //update rotation
    drone->updatePropellers(deltaTime);

    //update cam. position
    chopperCameraAngle += 0.3f * deltaTime;
    if (chopperCameraAngle > 2 * M_PI)
    {
        chopperCameraAngle -= 2 * M_PI;
    }
}

void Controller::processInput(GLFWwindow *window, float deltaTime)
{
    //multiple key taps
    float rotateSpeed = 1.0f * deltaTime;
    float moveSpeed = 1.0f;

    if (!drone->isRolling())
    {

        // forward + backward movement
        if (keys[GLFW_KEY_EQUAL] || keys[GLFW_KEY_KP_ADD])
        {
            // Forward +
            drone->moveForward(moveSpeed);
        }
        if (keys[GLFW_KEY_MINUS] || keys[GLFW_KEY_KP_SUBTRACT])
        {
            // Backward -
            drone->moveBackward(moveSpeed);
        }

        // turns
        if (keys[GLFW_KEY_LEFT])
        {
            drone->turnLeft(rotateSpeed);
        }
        if (keys[GLFW_KEY_RIGHT])
        {
            drone->turnRight(rotateSpeed);
        }
        if (keys[GLFW_KEY_UP])
        {
            drone->turnUp(rotateSpeed);
        }
        if (keys[GLFW_KEY_DOWN])
        {
            drone->turnDown(rotateSpeed);
        }
    }
}

glm::mat4 Controller::getViewMatrix() const
{
    glm::vec3 cameraPos;
    glm::vec3 cameraTarget;
    glm::vec3 upVector(0.0f, 1.0f, 0.0f);

    switch (cameraMode)
    {
    case GLOBAL_CAMERA:
        cameraPos = getGlobalCameraPosition();
        cameraTarget = getGlobalCameraTarget();
        break;
    case CHOPPER_CAMERA:
        cameraPos = getChopperCameraPosition(0.0f);
        cameraTarget = getChopperCameraTarget();
        break;
    case FIRST_PERSON_CAMERA:
        cameraPos = getFirstPersonCameraPosition();
        cameraTarget = getFirstPersonCameraTarget();
        break;
    }

    return glm::lookAt(cameraPos, cameraTarget, upVector);
}

glm::mat4 Controller::getProjectionMatrix(float aspectRatio) const
{
    //perspective projection
    return glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
}

// cam. positions
glm::vec3 Controller::getGlobalCameraPosition() const
{
    //set the global camera position
    return glm::vec3(10.0f, 10.0f, 10.0f);
}

glm::vec3 Controller::getChopperCameraPosition() const
{
    //set the chopper camera position based on the drone's position
    float radius = 20.0f;
    float height = 15.0f;
    float x = sin(chopperCameraAngle) * radius;
    float z = cos(chopperCameraAngle) * radius;

    return glm::vec3(x, height, z);
}

glm::vec3 Controller::getFirstPersonCameraPosition() const
{
    //set first person cam position in front of drone
    glm::vec3 dronePos = drone->getPosition();
    glm::vec3 droneRot = drone->getRotation();

    //find position
    float yaw = droneRot.y;
    float pitch = droneRot.x;

    //find direction
    glm::vec3 direction;
    direction.x = sin(yaw) * cos(pitch);
    direction.y = sin(pitch);
    direction.z = cos(yaw) * cos(pitch);

    //normalizing
    direction = glm::normalize(direction);

    //set cam location in front of drone
    return dronePos + direction * 1.5f + glm::vec3(0.0f, 0.5f, 0.0f);
}

glm::vec3 Controller::getGlobalCameraTarget() const
{
    //global cam
    return drone->getPosition();
}

glm::vec3 Controller::getChopperCameraTarget() const
{
    //chopper cam in center
    return drone->getPosition();
}

glm::vec3 Controller::getFirstPersonCameraTarget() const
{
    //first person cam in front of drone
    glm::vec3 dronePos = drone->getPosition();
    glm::vec3 droneRot = drone->getRotation();

    // calc. direction
    float yaw = droneRot.y;
    float pitch = droneRot.x;

    glm::vec3 direction;
    direction.x = sin(yaw) * cos(pitch);
    direction.y = sin(pitch);
    direction.z = cos(yaw) * cos(pitch);

    //normalize direction
    direction = glm::normalize(direction);

    return getFirstPersonCameraPosition() + direction * 10.0f;
}