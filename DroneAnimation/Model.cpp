#include "Model.h"
#include "glm/fwd.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>

Model::Model()
    : position(0.0f, 5.0f, 0.0f),
      rotation(0.0f, 0.0f, 0.0f),
      scale(1.0f, 1.0f, 1.0f),
      propellerAngle(0.0f),
      propellerSpeed(5.0f),
      rolling(false),
      rollAngle(0.0f),
      bodyVAO(0), bodyVBO(0),
      propellerVAO(0), propellerVBO(0),
      landingGearVAO(0), landingGearVBO(0)
{
    // DEBUG: std::cout << "Model constructor called" << std::endl;
}

Model::~Model()
{
    if (bodyVAO != 0)
    {
        glDeleteVertexArrays(1, &bodyVAO);
        glDeleteBuffers(1, &bodyVBO);
    }

    if (propellerVAO != 0)
    {
        glDeleteVertexArrays(1, &propellerVAO);
        glDeleteBuffers(1, &propellerVBO);
    }

    if (landingGearVAO != 0)
    {
        glDeleteVertexArrays(1, &landingGearVAO);
        glDeleteBuffers(1, &landingGearVBO);
    }
}

void Model::initDrone()
{
    // DEBUG: std::cout << "Initializing drone components..." << std::endl;
    createBody();
    createPropellers();
    createLandingGear();
    // DEBUG: std::cout << "Drone components initialized" << std::endl;
}

void Model::createBody()
{
    // Create drone body geometry (fuselage)
    // positions (3 floats) and colors (3 floats)
    bodyVertices = {
        // nose of the drone (triangles)
        // front
        0.0f, 0.0f, 2.0f, 0.5f, 0.5f, 0.5f, // front
        // top right
        1.0f, 0.5f, 1.0f, 0.5f, 0.5f, 0.5f, // tr
        // bottom right
        1.0f, -0.5f, 1.0f, 0.5f, 0.5f, 0.5f, // br
        // top right
        1.0f, 0.5f, 1.0f, 0.5f, 0.5f, 0.5f, // tr

        // ... ->
        1.0f, -0.5f, 1.0f, 0.3f, 0.3f, 0.3f, // br
        -1.0f, -0.5f, 1.0f, 0.3f, 0.3f, 0.3f, // bl
        0.0f, 0.0f, 2.0f, 0.3f, 0.3f, 0.3f, // front
        -1.0f, -0.5f, 1.0f, 0.3f, 0.3f, 0.3f, // bl

        -1.0f, 0.5f, 1.0f, 0.5f, 0.5f, 0.5f, // tl
        -1.0f, -0.5f, 1.0f, 0.5f, 0.5f, 0.5f, // bl
        0.0f, 0.0f, 2.0f, 0.5f, 0.5f, 0.5f, // front
        -1.0f, -0.5f, 1.0f, 0.5f, 0.5f, 0.5f, // bl

        1.0f, 0.5f, 1.0f, 0.8f, 0.8f, 0.8f, // tr
        -1.0f, 0.5f, 1.0f, 0.8f, 0.8f, 0.8f, // tl
        0.0f, 0.0f, 2.0f, 0.8f, 0.8f, 0.8f, // front
        -1.0f, 0.5f, 1.0f, 0.8f, 0.8f, 0.8f, // tl

        // follows the order: front -> baack, top -> bottom, left -> right
        -1.0f, 0.5f, 1.0f, 0.6f, 0.6f, 0.6f,
        1.0f, 0.5f, 1.0f, 0.6f, 0.6f, 0.6f,
        1.0f, 0.5f, -1.0f, 0.6f, 0.6f, 0.6f,
        -1.0f, 0.5f, -1.0f, 0.6f, 0.6f, 0.6f,

        -1.0f, 0.5f, 1.0f, 0.4f, 0.4f, 0.4f,
        -1.0f, 0.5f, -1.0f, 0.4f, 0.4f, 0.4f,
        -1.0f, -0.5f, -1.0f, 0.4f, 0.4f, 0.4f,
        -1.0f, -0.5f, 1.0f, 0.4f, 0.4f, 0.4f,

        -1.0f, -0.5f, -1.0f, 0.2f, 0.2f, 0.2f,
        1.0f, -0.5f, -1.0f, 0.2f, 0.2f, 0.2f,
        1.0f, -0.5f, 1.0f, 0.2f, 0.2f, 0.2f,
        -1.0f, -0.5f, 1.0f, 0.2f, 0.2f, 0.2f,

        1.0f, 0.5f, 1.0f, 0.4f, 0.4f, 0.4f,
        1.0f, -0.5f, 1.0f, 0.4f, 0.4f, 0.4f,
        1.0f, -0.5f, -1.0f, 0.4f, 0.4f, 0.4f,
        1.0f, 0.5f, -1.0f, 0.4f, 0.4f, 0.4f,

        1.0f, 0.5f, -1.0f, 0.4f, 0.4f, 0.4f,
        1.0f, -0.5f, -1.0f, 0.4f, 0.4f, 0.4f,
        -1.0f, -0.5f, -1.0f, 0.4f, 0.4f, 0.4f,
        -1.0f, 0.5f, -1.0f, 0.4f, 0.4f, 0.4f};

    // initialize buffers and load data into them
    glGenVertexArrays(1, &bodyVAO);
    glGenBuffers(1, &bodyVBO);

    glBindVertexArray(bodyVAO);
    glBindBuffer(GL_ARRAY_BUFFER, bodyVBO);
    glBufferData(GL_ARRAY_BUFFER, bodyVertices.size() * sizeof(GLfloat), bodyVertices.data(), GL_STATIC_DRAW);

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void *)0);
    glEnableVertexAttribArray(0);

    // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Model::createPropellers()
{
    // create each rectangle blade for each propeller
    propellerVertices = {
        // forward blade
        -0.1f, 0.0f, 0.5f, 0.3f, 0.3f, 0.3f,
        0.1f, 0.0f, 0.5f, 0.3f, 0.3f, 0.3f,
        0.1f, 0.0f, -0.5f, 0.3f, 0.3f, 0.3f,
        -0.1f, 0.0f, -0.5f, 0.3f, 0.3f, 0.3f,

        // right blade
        0.5f, 0.0f, 0.1f, 0.3f, 0.3f, 0.3f,
        0.5f, 0.0f, -0.1f, 0.3f, 0.3f, 0.3f,
        -0.5f, 0.0f, -0.1f, 0.3f, 0.3f, 0.3f,
        -0.5f, 0.0f, 0.1f, 0.3f, 0.3f, 0.3f,

        // backward blade
        -0.1f, 0.0f, 0.5f, 0.3f, 0.3f, 0.3f,
        -0.1f, 0.0f, -0.5f, 0.3f, 0.3f, 0.3f,
        0.1f, 0.0f, -0.5f, 0.3f, 0.3f, 0.3f,
        0.1f, 0.0f, 0.5f, 0.3f, 0.3f, 0.3f,

        // left blade
        0.5f, 0.0f, 0.1f, 0.3f, 0.3f, 0.3f,
        -0.5f, 0.0f, 0.1f, 0.3f, 0.3f, 0.3f,
        -0.5f, 0.0f, -0.1f, 0.3f, 0.3f, 0.3f,
        0.5f, 0.0f, -0.1f, 0.3f, 0.3f, 0.3f,

        // middle piece
        -0.2f, 0.0f, 0.2f, 0.2f, 0.2f, 0.2f,
        0.2f, 0.0f, 0.2f, 0.2f, 0.2f, 0.2f,
        0.2f, 0.0f, -0.2f, 0.2f, 0.2f, 0.2f,
        -0.2f, 0.0f, -0.2f, 0.2f, 0.2f, 0.2f};

    // buffers
    glGenVertexArrays(1, &propellerVAO);
    glGenBuffers(1, &propellerVBO);

    glBindVertexArray(propellerVAO);
    glBindBuffer(GL_ARRAY_BUFFER, propellerVBO);
    glBufferData(GL_ARRAY_BUFFER, propellerVertices.size() * sizeof(GLfloat), propellerVertices.data(), GL_STATIC_DRAW);

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void *)0);
    glEnableVertexAttribArray(0);

    // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Model::createLandingGear()
{
    // four leg landing gear
    landingGearVertices = {
        // front left leg
        -0.8f, -0.5f, 0.8f, 0.4f, 0.4f, 0.4f,
        -0.8f, -1.0f, 0.8f, 0.4f, 0.4f, 0.4f,
        -0.9f, -1.0f, 0.9f, 0.4f, 0.4f, 0.4f,
        -0.8f, -1.0f, 0.8f, 0.4f, 0.4f, 0.4f,

        // front right leg
        0.8f, -0.5f, 0.8f, 0.4f, 0.4f, 0.4f,
        0.8f, -1.0f, 0.8f, 0.4f, 0.4f, 0.4f,
        0.9f, -1.0f, 0.9f, 0.4f, 0.4f, 0.4f,
        0.8f, -1.0f, 0.8f, 0.4f, 0.4f, 0.4f,

        // back left leg
        -0.8f, -0.5f, -0.8f, 0.4f, 0.4f, 0.4f,
        -0.8f, -1.0f, -0.8f, 0.4f, 0.4f, 0.4f,
        -0.9f, -1.0f, -0.9f, 0.4f, 0.4f, 0.4f,
        -0.8f, -1.0f, -0.8f, 0.4f, 0.4f, 0.4f,

        // back rigt leg
        0.8f, -0.5f, -0.8f, 0.4f, 0.4f, 0.4f,
        0.8f, -1.0f, -0.8f, 0.4f, 0.4f, 0.4f,
        0.9f, -1.0f, -0.9f, 0.4f, 0.4f, 0.4f,
        0.8f, -1.0f, -0.8f, 0.4f, 0.4f, 0.4f};

    // buffers
    glGenVertexArrays(1, &landingGearVAO);
    glGenBuffers(1, &landingGearVBO);

    glBindVertexArray(landingGearVAO);
    glBindBuffer(GL_ARRAY_BUFFER, landingGearVBO);
    glBufferData(GL_ARRAY_BUFFER, landingGearVertices.size() * sizeof(GLfloat), landingGearVertices.data(), GL_STATIC_DRAW);

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void *)0);
    glEnableVertexAttribArray(0);

    // corlor
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Model::render(GLuint shaderProgram)
{
    // Check if model is initialized
    if (bodyVAO == 0 || propellerVAO == 0 || landingGearVAO == 0)
    {
        std::cerr << "Warning: Attempting to render uninitialized model" << std::endl;
        return;
    }

    // set up model matrix
    glm::mat4 modelMatrix = glm::mat4(1.0f);

    // transformations - translation, rotation, scale
    modelMatrix = glm::translate(modelMatrix, position);

    modelMatrix = glm::rotate(modelMatrix, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f)); // Yaw (around Y-axis)
    modelMatrix = glm::rotate(modelMatrix, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f)); // Pitch (around X-axis)
    modelMatrix = glm::rotate(modelMatrix, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f)); // Roll (around Z-axis)

    modelMatrix = glm::scale(modelMatrix, scale);

    // draw parts
    drawBody(shaderProgram, modelMatrix);
    drawPropellers(shaderProgram, modelMatrix);
    drawLandingGear(shaderProgram, modelMatrix);
}

void Model::drawBody(GLuint shaderProgram, const glm::mat4 &modelMatrix)
{
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    glBindVertexArray(bodyVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 24, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 28, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 32, 4);
    glBindVertexArray(0);
}

void Model::drawPropellers(GLuint shaderProgram, const glm::mat4 &modelMatrix)
{
    // front left propeler
    glm::mat4 frontLeftProp = glm::translate(modelMatrix, glm::vec3(-1.2f, 0.5f, 1.2f));
    frontLeftProp = glm::rotate(frontLeftProp, propellerAngle, glm::vec3(0.0f, 1.0f, 0.0f));

    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(frontLeftProp));

    glBindVertexArray(propellerVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 20);
    glBindVertexArray(0);

    // draw front right propeller
    glm::mat4 frontRightProp = glm::translate(modelMatrix, glm::vec3(1.2f, 0.5f, 1.2f));
    frontRightProp = glm::rotate(frontRightProp, -propellerAngle, glm::vec3(0.0f, 1.0f, 0.0f));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(frontRightProp));

    glBindVertexArray(propellerVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 20);
    glBindVertexArray(0);

    // draw back left propeller
    glm::mat4 backLeftProp = glm::translate(modelMatrix, glm::vec3(-1.2f, 0.5f, -1.2f));
    backLeftProp = glm::rotate(backLeftProp, -propellerAngle, glm::vec3(0.0f, 1.0f, 0.0f));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(backLeftProp));

    glBindVertexArray(propellerVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 20);
    glBindVertexArray(0);

    // draw back right prpeller
    glm::mat4 backRightProp = glm::translate(modelMatrix, glm::vec3(1.2f, 0.5f, -1.2f));
    backRightProp = glm::rotate(backRightProp, propellerAngle, glm::vec3(0.0f, 1.0f, 0.0f));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(backRightProp));

    glBindVertexArray(propellerVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 20);
    glBindVertexArray(0);
}

void Model::drawLandingGear(GLuint shaderProgram, const glm::mat4 &modelMatrix)
{
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    glBindVertexArray(landingGearVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
    glBindVertexArray(0);
}

void Model::updatePropellers(float deltaTime)
{
    // update propeller rotation -> use time + speed
    propellerAngle += propellerSpeed * deltaTime;

    // make sure angle is within [0, 2*PI]
    if (propellerAngle > 2 * M_PI)
    {
        propellerAngle -= 2 * M_PI;
    }

    // edge case - if rolling, update
    if (rolling)
    {
        performRoll(deltaTime);
    }
}

void Model::performRoll(float deltaTime)
{
    // roll speed cooresponds to propeller speed
    float rollSpeed = propellerSpeed * 0.2f;

    // roll angle
    rollAngle += rollSpeed * deltaTime;

    // roll rotation around Z
    rotation.z = rollAngle * (float)M_PI;
    // DEBUG: std::cout << "rotation z - " << rotation.z << std::endl;
    // DEBUG: std::cout << "roll angle - " << rollAngle << std::endl;

    // checks if full roatation completed
    if (rotation.z >= 2 * M_PI)
    {
        // DEBUG: std::cout << "roll done" << std::endl;
        // reset roll
        rolling = false;
        rollAngle = 0.0f;
        rotation.z = 0.0f;
    }
}


glm::vec3 Model::getAdvancement(float scalar) const
{
    // set first person cam position in front of drone
    glm::vec3 dronePos = this->getPosition();
    glm::vec3 droneRot = this->getRotation();

    // find position
    float yaw = droneRot.y;
    float pitch = droneRot.x;

    // find direction
    glm::vec3 direction;
    direction.x = sin(yaw) * cos(pitch);
    direction.y = -sin(pitch);
    direction.z = cos(yaw) * cos(pitch);

    // normalizing
    direction = glm::normalize(direction);

    // set cam location in front of drone
    return dronePos + (direction * scalar);
}

void Model::moveForward(float speed)
{
    // move in direction drone faces
    float moveSpeed = speed * propellerSpeed * 0.02f;

    glm::vec3 newPosn = this->getAdvancement(moveSpeed * 1);
    this->position.x = newPosn.x;
    this->position.y = newPosn.y;
    this->position.z = newPosn.z;
}

void Model::moveBackward(float speed)
{
    // move opiste of direction drone faces
    float moveSpeed = speed * propellerSpeed * 0.02f;

    glm::vec3 newPosn = this->getAdvancement(moveSpeed * -1);
    this->position.x = newPosn.x;
    this->position.y = newPosn.y;
    this->position.z = newPosn.z;
}

void Model::turnLeft(float angle)
{
    // left
    rotation.y += angle;

    if (rotation.y > 2 * M_PI)
    {
        rotation.y -= 2 * M_PI;
    }
}

void Model::turnRight(float angle)
{
    // clockwise around Y
    rotation.y -= angle;

    // keep angle within 2*PI
    if (rotation.y < 0)
    {
        rotation.y += 2 * M_PI;
    }
}

void Model::turnUp(float angle)
{
    // rotate drone around X
    rotation.x -= angle;

    // avoids flipping
    if (rotation.x <= -M_PI / 4)
    {
        rotation.x = -M_PI / 4;
    }
}

void Model::turnDown(float angle)
{
    // rotate drone aorund X
    rotation.x += angle;

    // keep angle in a reasonable range
    if (rotation.x >= M_PI / 4)
    {
        rotation.x = M_PI / 4;
    }
}

void Model::resetPosition()
{
    // rest drone to original position
    position = glm::vec3(0.0f, 5.0f, 0.0f);
    rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    rolling = false;
    rollAngle = 0.0f;
}