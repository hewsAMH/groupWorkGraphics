#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Model
{
public:
    Model();
    ~Model();

    void initDrone();
    void render(GLuint shaderProgram);

    // getters + setters for pos. / orientation
    void setPosition(const glm::vec3 &pos) { position = pos; }
    glm::vec3 getPosition() const { return position; }

    void setRotation(const glm::vec3 &rot) { rotation = rot; }
    glm::vec3 getRotation() const { return rotation; }

    // setting propeller animation speed
    void setPropellerSpeed(float speed) { propellerSpeed = speed; }
    float getPropellerSpeed() const { return propellerSpeed; }
    void updatePropellers(float deltaTime);

    // control drone orientation and movement - speed and angle (float)
    void moveForward(float speed);
    void moveBackward(float speed);
    void turnLeft(float angle);
    void turnRight(float angle);
    void turnUp(float angle);
    void turnDown(float angle);
    void resetPosition();

    // edge case - rolling
    void performRoll(float deltaTime);
    bool isRolling() const { return rolling; }
    void startRoll()
    {
        rolling = true;
        rollAngle = 0.0f;
    }

private:
    // initalize drone parts
    void createBody();
    void createPropellers();
    void createLandingGear();

    // drawing helper functions
    void drawBody(GLuint shaderProgram, const glm::mat4 &modelMatrix);
    void drawPropellers(GLuint shaderProgram, const glm::mat4 &modelMatrix);
    void drawLandingGear(GLuint shaderProgram, const glm::mat4 &modelMatrix);

    // position in world space, rotation in euler angles, scale factors
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    // initialize properties
    // current angle
    float propellerAngle;
    // prop. rotation speed
    float propellerSpeed;
    // whether the drone is rolling
    bool rolling;
    // curent roll angle
    float rollAngle;

    std::vector<GLfloat> bodyVertices;
    std::vector<GLfloat> propellerVertices;
    std::vector<GLfloat> landingGearVertices;

    GLuint bodyVAO, bodyVBO;
    GLuint propellerVAO, propellerVBO;
    GLuint landingGearVAO, landingGearVBO;
};

#endif