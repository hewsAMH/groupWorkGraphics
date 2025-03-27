#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include "Callbacks.h"
#include "Model.h"
#include "View.h"
#include "ourutils/Logger.h"


class Controller: public Callbacks {
    public:
        Controller(Model& m, View& v, vector<string>& argv);
        ~Controller();
        void run();
        void promptAdjustRotation();

        virtual void reshape(int width, int height);
        virtual void dispose();
        virtual void onkey(int key, int scancode, int action, int mods);
        virtual void onmouse(int button, int action, int mods);
        virtual void error_callback(int error, const char* description);

    private:
        void initLogger(Model& m, View& v, vector<string> &argv);
        void initScenegraph(vector<string> &argv);

        View view;
        Model model;
        bool lbutton_down = false;
        double cursorPosnX;
        double cursorPosnY;

        ourutils::Logger logger;
};

#endif