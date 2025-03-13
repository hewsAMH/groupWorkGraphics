#include <glad/glad.h>
#include "View.h"
#include "Model.h"
#include "Controller.h"
#include <iostream>

int main(int argc, char *argv[])
{
    // DEBUG: std::cout << "Starting program..." << std::endl;

    // Create view
    // DEBUG: std::cout << "Creating view..." << std::endl;
    View view;

    // Initialize view (creates window, sets up OpenGL)
    // DEBUG: std::cout << "Initializing view..." << std::endl;
    if (!view.init())
    {
        std::cerr << "Failed to initialize view" << std::endl;
        return -1;
    }

    // Run the main loop
    // DEBUG: std::cout << "Running main loop..." << std::endl;
    view.run();

    // DEBUG: std::cout << "Program completed successfully" << std::endl;
    return 0;
}
