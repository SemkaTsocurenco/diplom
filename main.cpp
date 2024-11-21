#include <iostream>
#include <vector>
#include <dlfcn.h>
#include <opencv2/opencv.hpp>
#include "./include/main.hpp"

void (*func)(std::string);



int main(int argc, char *argv[]) {
    void* method;
    
    std::vector<std::string> argList;
    for (int i = 0; i < argc; i++)
        argList.push_back(argv[i]);

    method = dlopen(("../build/lib" + argList[1] + ".so").c_str(), RTLD_LAZY);
    if (!method) {
        std::cerr << "Failed to load library: " << dlerror() << "\n";
        return 1;
    }
    func = (void (*)(const std::string)) dlsym(method, "findLines");
    if (!func) {
        std::cerr << "Failed to load function: " << dlerror() << "\n";
        return 1;
    }
    func("../images/");



    return 0;
}
