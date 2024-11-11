#include <iostream>
#include <vector>
#include <dlfcn.h>
#include <opencv2/opencv.hpp>
#include "./include/main.hpp"

void (*func)(cv::Mat);

int main(int argc, char *argv[]) {
    void* method;
    cv::Mat frame;



    std::vector<std::string> argList;
    for (int i = 0; i < argc; i++)
        argList.push_back(argv[i]);

    method = dlopen(("../build/lib" + argList[1] + ".so").c_str(), RTLD_LAZY);
    if (!method) {
        std::cerr << "Failed to load library: " << dlerror() << "\n";
        return 1;
    }



	cv::VideoCapture cap("/home/tsokurenkosv/CLionProjects/bird_v1/video.mp4"); // Используем камеру по умолчанию
    if (!cap.isOpened()) {
        std::cerr << "Ошибка: Не удалось открыть камеру!" << std::endl;
        return -1;
    }



    func = (void (*)(cv::Mat)) dlsym(method, "findLines");
    if (!func) {
        std::cerr << "Failed to load function: " << dlerror() << "\n";
        return 1;
    }

    while (true) {
        cap >> frame;
		if (frame.empty()) {
            std::cerr << "Ошибка: Не удалось захватить кадр!" << std::endl;
            break;
        }
		if (cv::waitKey(30) == 'q') break;
    	func(frame);
	}
	cap.release();
    cv::destroyAllWindows();

    return 0;
}
