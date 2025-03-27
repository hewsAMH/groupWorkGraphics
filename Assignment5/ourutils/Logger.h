#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <iostream>
#include <vector>


namespace ourutils {

class Logger {
    public:
        Logger() {};
        ~Logger() {};

        void setDebug() { this->debugFlag = true; }

        void print(const std::vector<std::string>& args) {
            if (args.empty()) {
                std::cout << std::endl;
                return;
            }
            if (args.size() == 1) {
                std::cout << args.back() << std::endl;
                return;
            }
            for (auto it = args.begin(); it != args.end() - 1; ++it) {
                std::cout << *it << " ";
            }
            std::cout << args.back() << std::endl;
        }

        void debugPrint(const std::vector<std::string>& args) {
            if (this->debugFlag) {
                std::cout << "DEBUG: ";
                this->print(args);
            }
        }
    
    private:
        bool debugFlag = false;
};

} // namespace ourutils

#endif
