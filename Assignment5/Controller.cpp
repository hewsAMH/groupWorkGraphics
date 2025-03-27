#include "Controller.h"
#include "ObjImporter.h"
#include "TextureImage.h"
#include "sgraph/GroupNode.h"
#include "sgraph/IScenegraph.h"
#include "sgraph/LeafNode.h"
#include "sgraph/ScaleTransform.h"
#include "sgraph/Scenegraph.h"
using namespace sgraph;
#include <iostream>
using namespace std;

#include "sgraph/ScenegraphExporter.h"
#include "sgraph/ScenegraphImporter.h"
#include "sgraph/TextScenegraphRenderer.h"

Controller::Controller(Model& m, View& v, vector<string> &argv) : model(m), view(v) {
    this->initLogger(m, v, argv);
    this->initScenegraph(argv);
}

void Controller::initLogger(Model& m, View& v, vector<string> &argv) {
    ourutils::Logger logger;
    /** optional arg [ -d ] enables debug printing */
    if (std::find(argv.begin(), argv.end(), "-d") != argv.end())
        logger.setDebug();
    this->logger = logger;
    this->model.setLogger(logger);
    this->view.setLogger(logger);
}

//edited to use the new text renderer (TextScenegraphRenderer.h)
void Controller::initScenegraph(vector<string> &argv) {
    std::string commandFilePath = "scenegraphmodels/courtyard-scene-commands.txt";
    /** optional arg [ -f <filepath> ] overwrites the hardcoded default */
    auto it = std::find(argv.begin(), argv.end(), "-f");
    if (it != argv.end() && it + 1 != argv.end())
        commandFilePath = *(it + 1);
    //read in the file of commands
    ifstream inFile(commandFilePath);
    sgraph::ScenegraphImporter importer;

    /** importer, parsing logic, & leafnode all changed to accommodate light */
    IScenegraph *scenegraph = importer.parse(inFile);
    model.setScenegraph(scenegraph);
    this->logger.debugPrint({"Scenegraph made"});

    //create + use the text renderer
    sgraph::TextScenegraphRenderer renderer;
    scenegraph->getRoot()->accept(&renderer);
    string textRepresentation = renderer.getOutput();
    cout << "\nScene Graph Structure:\n" << textRepresentation << endl;
    this->logger.debugPrint({"Finished printing the structure"});
}

Controller::~Controller() {}

void Controller::run() {
    sgraph::IScenegraph* scenegraph = model.getScenegraph();
    map<string,util::PolygonMesh<VertexAttrib>> meshes = scenegraph->getMeshes();
    map<string,util::TextureImage> images = scenegraph->getImages();
    view.init(this, meshes, images);
    while (!view.shouldWindowClose()) {
        view.display(scenegraph);
        promptAdjustRotation();
    }
    view.closeWindow();
    exit(EXIT_SUCCESS);
}

void Controller::onkey(int key, int scancode, int action, int mods)
{
    // cout << (char)key << " pressed" << endl;
    if (static_cast<char>(key) == 'R' || static_cast<char>(key) == 'r') {
        this->view.resetRotation();
    }
    return;
}

// called repeatedly to update state and view's knowledge of it
void Controller::promptAdjustRotation() {
    float speedModifier = 3.0f; /* arbitrary, can adjust for preferred speed */
    double xpos, ypos;
    this->view.getCursorPosn(&xpos, &ypos);
    if (this->lbutton_down) {  // actively holding down mouse
        float deltaX = -static_cast<float>(xpos - this->cursorPosnX);
        float deltaY = static_cast<float>(ypos - this->cursorPosnY);
        this->view.adjustRotation('x', glm::radians(deltaX) / speedModifier);
        this->view.adjustRotation('y', glm::radians(deltaY) / speedModifier);
    }
    this->cursorPosnX = xpos;
    this->cursorPosnY = ypos;
}

// callback added for mouse events
// toggles a flag on/off for mouse being held
void Controller::onmouse(int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if(GLFW_PRESS == action)
            this->lbutton_down = true;
        else if(GLFW_RELEASE == action)
            this->lbutton_down = false;
    }
}

void Controller::reshape(int width, int height) 
{
    cout <<"Window reshaped to width=" << width << " and height=" << height << endl;
    GLsizei viewSideLength;
    try {
        // extra logic here covers retina displays
        float xScale = 1;
        float yScale = 1;
        this->view.getWindowScalars(&xScale, &yScale);
        int widthScaled = (int)((float)width * xScale);
        int heightScaled = (int)((float)height * yScale);
        // maintains the largest square display possible
        viewSideLength = min(widthScaled, heightScaled);
        glViewport(0, 0, viewSideLength, viewSideLength);
    } catch (...) {
        viewSideLength = min(width, height);
        glViewport(0, 0, viewSideLength, viewSideLength);
    }
}

void Controller::dispose() { view.closeWindow(); }

void Controller::error_callback(int error, const char* description) {
    std::cerr << "Error: " << description << std::endl;
}
