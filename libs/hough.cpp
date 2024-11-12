#include "../include/main.hpp"

// Параметры HSV
int h_min, h_max;
int s_min, s_max;
int v_min, v_max;

// Параметры Hough Line Transform
int rho = 1;
int theta = 1;
int hough_threshold = 150;

// Функция для загрузки HSV параметров из YAML-файла
void loadHSVParams(const std::string& filename) {
    std::cout << "Загрузка HSV параметров\n";
    cv::FileStorage fs(filename, cv::FileStorage::READ);
    if (!fs.isOpened()) {
        std::cerr << "Ошибка: не удалось открыть файл " << filename << " для чтения." << std::endl;
        return;
    }
    fs["H_Min"] >> h_min;
    fs["H_Max"] >> h_max;
    fs["S_Min"] >> s_min;
    fs["S_Max"] >> s_max;
    fs["V_Min"] >> v_min;
    fs["V_Max"] >> v_max;
    fs.release();
    std::cout << "Загрузка HSV параметров завершена\n";
}

// Функция для сохранения HSV параметров в YAML-файл
void saveHSVParams(const std::string& filename) {
    std::cout << "Сохранение HSV параметров\n";
    cv::FileStorage fs(filename, cv::FileStorage::WRITE);
    if (!fs.isOpened()) {
        std::cerr << "Ошибка: не удалось открыть файл " << filename << " для записи." << std::endl;
        return;
    }
    fs << "H_Min" << h_min << "H_Max" << h_max;
    fs << "S_Min" << s_min << "S_Max" << s_max;
    fs << "V_Min" << v_min << "V_Max" << v_max;
    fs.release();
    std::cout << "Сохранение HSV параметров завершено\n";
}

// Функция для загрузки Hough параметров из YAML-файла
void loadHoughParams(const std::string& filename) {
    std::cout << "Загрузка Hough параметров\n";
    cv::FileStorage fs(filename, cv::FileStorage::READ);
    if (!fs.isOpened()) {
        std::cerr << "Ошибка: не удалось открыть файл " << filename << " для чтения." << std::endl;
        return;
    }
    fs["Rho"] >> rho;
    fs["Theta"] >> theta;
    fs["Threshold"] >> hough_threshold;
    fs.release();
    std::cout << "Загрузка Hough параметров завершена\n";
}

// Функция для сохранения Hough параметров в YAML-файл
void saveHoughParams(const std::string& filename) {
    std::cout << "Сохранение Hough параметров\n";
    cv::FileStorage fs(filename, cv::FileStorage::WRITE);
    if (!fs.isOpened()) {
        std::cerr << "Ошибка: не удалось открыть файл " << filename << " для записи." << std::endl;
        return;
    }
    fs << "Rho" << rho << "Theta" << theta << "Threshold" << hough_threshold;
    fs.release();
    std::cout << "Сохранение Hough параметров завершено\n";
}

void processFrame(const cv::Mat& frame, cv::Mat& mask) {
    cv::Mat hsv;
    cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);

    cv::Scalar lower(h_min, s_min, v_min);
    cv::Scalar upper(h_max, s_max, v_max);

    cv::inRange(hsv, lower, upper, mask);
}

void detectLines(const cv::Mat& mask, cv::Mat& output) {
    std::vector<cv::Vec2f> lines;
    double theta_rad = theta * CV_PI / 180.0; // Преобразуем theta в радианы
    cv::HoughLines(mask, lines, rho, theta_rad, hough_threshold);

    for (size_t i = 0; i < lines.size(); i++) {
        float rho = lines[i][0];
        float theta = lines[i][1];
        double a = cos(theta), b = sin(theta);
        double x0 = a * rho, y0 = b * rho;
        cv::Point pt1(cvRound(x0 + 1000 * (-b)), cvRound(y0 + 1000 * a));
        cv::Point pt2(cvRound(x0 - 1000 * (-b)), cvRound(y0 - 1000 * a));
        cv::line(output, pt1, pt2, cv::Scalar(0, 0, 255), 2, cv::LINE_AA);
    }
}

void on_trackbar(int, void*) {}

void findLines(std::string vidname) {
    cv::Mat frame, mask, result;
    loadHSVParams("../res/hsv_parameters.yaml");
    loadHoughParams("../res/hough_parameters.yaml");

    cv::VideoCapture cap(vidname);
    if (!cap.isOpened()) {
        std::cerr << "Ошибка: не удалось открыть видео!" << std::endl;
        return;
    }

    cv::namedWindow("Original", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Masked", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Detected Lines", cv::WINDOW_AUTOSIZE);

    cv::createTrackbar("Rho", "Detected Lines", &rho, 10, on_trackbar);
    cv::createTrackbar("Theta", "Detected Lines", &theta, 180, on_trackbar);
    cv::createTrackbar("Threshold", "Detected Lines", &hough_threshold, 1000, on_trackbar);

    while (true) {
        cap >> frame;
        if (frame.empty()) {
            std::cerr << "Ошибка: не удалось захватить кадр!" << std::endl;
            break;
        }

        processFrame(frame, mask);
        result = frame.clone();

        detectLines(mask, result);

        cv::imshow("Original", frame);
        cv::imshow("Masked", mask);
        cv::imshow("Detected Lines", result);

        if (cv::waitKey(30) == 'q') break;
    }

    saveHoughParams("../res/hough_parameters.yaml");
    cap.release();
    cv::destroyAllWindows();
}