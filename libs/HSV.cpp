#include "../include/main.hpp"



void processFrame(const cv::Mat& frame, cv::Mat& output, int h_min, int h_max, int s_min, int s_max, int v_min, int v_max) {
    cv::Mat hsv;
    cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);

    cv::Scalar lower(h_min, s_min, v_min);
    cv::Scalar upper(h_max, s_max, v_max);

    cv::inRange(hsv, lower, upper, output);
}

void on_trackbar(int, void*) {}

void findLines(cv::Mat inputImage){
    std::cout<< "HSV lib go on\n";

    int h_min = 0, h_max = 180;
    int s_min = 0, s_max = 255;
    int v_min = 200, v_max = 255;

    // Создаем окна
    cv::namedWindow("Original", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Masked", cv::WINDOW_AUTOSIZE);

    // Ползунки для настройки HSV
    cv::createTrackbar("H Min", "Masked", &h_min, 180, on_trackbar);
    cv::createTrackbar("H Max", "Masked", &h_max, 180, on_trackbar);
    cv::createTrackbar("S Min", "Masked", &s_min, 255, on_trackbar);
    cv::createTrackbar("S Max", "Masked", &s_max, 255, on_trackbar);
    cv::createTrackbar("V Min", "Masked", &v_min, 255, on_trackbar);
    cv::createTrackbar("V Max", "Masked", &v_max, 255, on_trackbar);

    cv::Mat frame, mask;
    // Обработка кадра с текущими параметрами HSV
    processFrame(frame, mask, h_min, h_max, s_min, s_max, v_min, v_max);

    // Отображение исходного кадра и обработанного маскированного изображения
    cv::imshow("Original", frame);
    cv::imshow("Masked", mask);
}