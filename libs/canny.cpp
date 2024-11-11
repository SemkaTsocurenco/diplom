#include "../include/main.hpp"


void findLines(std::string vidname){
    cv::Mat frame;
    cv::VideoCapture cap(vidname); // Используем камеру по умолчанию
    
    if (!cap.isOpened()) {
        std::cerr << "Ошибка: Не удалось открыть камеру!" << std::endl;
    }


    while (true) {
        cap >> frame;

		if (frame.empty()) {
            std::cerr << "Ошибка: Не удалось захватить кадр!" << std::endl;
            break;
        }
		if (cv::waitKey(30) == 'q') break;
        cv::imshow("original Image" , frame);
    }
}