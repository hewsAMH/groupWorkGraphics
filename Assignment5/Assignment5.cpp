#include "View.h"
#include "Controller.h"
#include "Model.h"
#include <glad/glad.h>


int main(int argc, char* argv[]) {
    Model model;
    View view;
    // added for debug flag, specifying scenegraph command file name/path, etc.
    std::vector<std::string> args(argv + 1, argv + argc);
    Controller controller(model, view, args);
    controller.run();
}
