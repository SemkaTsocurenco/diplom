// #include "../include/main.hpp"

// // Параметры HSV
// int h_min, h_max;
// int s_min, s_max;
// int v_min, v_max;

// // Параметры Hough Line Transform
// int rho = 1;
// int theta = 1;
// int hough_threshold = 150;

// // Функция для загрузки HSV параметров из YAML-файла
// void loadHSVParams(const std::string& filename) {
//     std::cout << "Загрузка HSV параметров\n";
//     cv::FileStorage fs(filename, cv::FileStorage::READ);
//     if (!fs.isOpened()) {
//         std::cerr << "Ошибка: не удалось открыть файл " << filename << " для чтения." << std::endl;
//         return;
//     }
//     fs["H_Min"] >> h_min;
//     fs["H_Max"] >> h_max;
//     fs["S_Min"] >> s_min;
//     fs["S_Max"] >> s_max;
//     fs["V_Min"] >> v_min;
//     fs["V_Max"] >> v_max;
//     fs.release();
//     std::cout << "Загрузка HSV параметров завершена\n";
// }

// // Функция для сохранения HSV параметров в YAML-файл
// void saveHSVParams(const std::string& filename) {
//     std::cout << "Сохранение HSV параметров\n";
//     cv::FileStorage fs(filename, cv::FileStorage::WRITE);
//     if (!fs.isOpened()) {
//         std::cerr << "Ошибка: не удалось открыть файл " << filename << " для записи." << std::endl;
//         return;
//     }
//     fs << "H_Min" << h_min << "H_Max" << h_max;
//     fs << "S_Min" << s_min << "S_Max" << s_max;
//     fs << "V_Min" << v_min << "V_Max" << v_max;
//     fs.release();
//     std::cout << "Сохранение HSV параметров завершено\n";
// }

// // Функция для загрузки Hough параметров из YAML-файла
// void loadHoughParams(const std::string& filename) {
//     std::cout << "Загрузка Hough параметров\n";
//     cv::FileStorage fs(filename, cv::FileStorage::READ);
//     if (!fs.isOpened()) {
//         std::cerr << "Ошибка: не удалось открыть файл " << filename << " для чтения." << std::endl;
//         return;
//     }
//     fs["Rho"] >> rho;
//     fs["Theta"] >> theta;
//     fs["Threshold"] >> hough_threshold;
//     fs.release();
//     std::cout << "Загрузка Hough параметров завершена\n";
// }

// // Функция для сохранения Hough параметров в YAML-файл
// void saveHoughParams(const std::string& filename) {
//     std::cout << "Сохранение Hough параметров\n";
//     cv::FileStorage fs(filename, cv::FileStorage::WRITE);
//     if (!fs.isOpened()) {
//         std::cerr << "Ошибка: не удалось открыть файл " << filename << " для записи." << std::endl;
//         return;
//     }
//     fs << "Rho" << rho << "Theta" << theta << "Threshold" << hough_threshold;
//     fs.release();
//     std::cout << "Сохранение Hough параметров завершено\n";
// }

// void processFrame(const cv::Mat& frame, cv::Mat& mask) {
//     cv::Mat hsv;
//     cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);

//     cv::Scalar lower(h_min, s_min, v_min);
//     cv::Scalar upper(h_max, s_max, v_max);

//     cv::inRange(hsv, lower, upper, mask);
// }

// void detectLines(const cv::Mat& mask, cv::Mat& output) {
//     std::vector<cv::Vec2f> lines;
//     double theta_rad = theta * CV_PI / 180.0; // Преобразуем theta в радианы
//     cv::HoughLines(mask, lines, rho, theta_rad, hough_threshold);

//     for (size_t i = 0; i < lines.size(); i++) {
//         float rho = lines[i][0];
//         float theta = lines[i][1];
//         double a = cos(theta), b = sin(theta);
//         double x0 = a * rho, y0 = b * rho;
//         cv::Point pt1(cvRound(x0 + 1000 * (-b)), cvRound(y0 + 1000 * a));
//         cv::Point pt2(cvRound(x0 - 1000 * (-b)), cvRound(y0 - 1000 * a));
//         cv::line(output, pt1, pt2, cv::Scalar(0, 0, 255), 2, cv::LINE_AA);
//     }
// }

// void on_trackbar(int, void*) {}

// void findLines(std::string vidname) {
//     cv::Mat frame, mask, result;
//     loadHSVParams("../res/hsv_parameters.yaml");
//     loadHoughParams("../res/hough_parameters.yaml");

//     cv::VideoCapture cap(vidname);
//     if (!cap.isOpened()) {
//         std::cerr << "Ошибка: не удалось открыть видео!" << std::endl;
//         return;
//     }

//     cv::namedWindow("Original", cv::WINDOW_AUTOSIZE);
//     cv::namedWindow("Masked", cv::WINDOW_AUTOSIZE);
//     cv::namedWindow("Detected Lines", cv::WINDOW_AUTOSIZE);

//     cv::createTrackbar("Rho", "Detected Lines", &rho, 10, on_trackbar);
//     cv::createTrackbar("Theta", "Detected Lines", &theta, 180, on_trackbar);
//     cv::createTrackbar("Threshold", "Detected Lines", &hough_threshold, 1000, on_trackbar);

//     while (true) {
//         cap >> frame;
//         if (frame.empty()) {
//             std::cerr << "Ошибка: не удалось захватить кадр!" << std::endl;
//             break;
//         }

//         processFrame(frame, mask);
//         result = frame.clone();

//         detectLines(mask, result);

//         cv::imshow("Original", frame);
//         cv::imshow("Masked", mask);
//         cv::imshow("Detected Lines", result);

//         if (cv::waitKey(30) == 'q') break;
//     }

//     saveHoughParams("../res/hough_parameters.yaml");
//     cap.release();
//     cv::destroyAllWindows();
// }

#include "../include/hough.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <filesystem> // Для работы с директориями

using namespace cv;
using namespace std;
namespace fs = std::filesystem;

// Конструктор класса
Hough::Hough(const std::string& imagesDir, const std::string& configPath)
    : imagesDir(imagesDir), configFile(configPath) {

    if (!fs::exists(imagesDir) || !fs::is_directory(imagesDir)) {
        cerr << "Директория не найдена: " << imagesDir << endl;
        exit(-1);
    }

    loadConfig();

    namedWindow("Birdview", WINDOW_AUTOSIZE);
    namedWindow("Canny", WINDOW_AUTOSIZE);

    createTrackbars();
}

// Загрузка конфигурации из YAML файла
bool Hough::loadConfig() {
    FileStorage fs(configFile, FileStorage::READ);

    points.resize(4);
    fs["xOffsettop"] >> xOffsettop;
    fs["xOffsetdown"] >> xOffsetdown;
    fs["topY"] >> topY;
    fs["bottomY"] >> bottomY;
    fs["width"] >> width;
    fs["height"] >> height;
    fs["Rho"] >> Rho;
    fs["Theta"] >> Theta;
    fs["Threshold"] >> Threshold;
    fs["threshold"] >> threshold;
    fs["size"] >> size;

    fs.release();
    return true;
}

// Сохранение конфигурации в YAML файл
void Hough::saveConfig() {
    FileStorage fs(configFile, FileStorage::WRITE);

    fs << "xOffsettop" << xOffsettop;
    fs << "xOffsetdown" << xOffsetdown;
    fs << "topY" << topY;
    fs << "bottomY" << bottomY;
    fs << "width" << width;
    fs << "height" << height;
    fs << "Rho" << Rho;
    fs << "Theta" << Theta;
    fs << "Threshold" << Threshold;
    fs << "threshold" << threshold;
    fs << "size" << size;

    fs.release();
}

// Обновление birdview
void Hough::updateBirdview() {
    vector<Point2f> dstPoints = {
        Point2f(0, 0),
        Point2f(width - 1, 0),
        Point2f(width - 1, height - 1),
        Point2f(0, height - 1)
    };

    frameSize = frame.size();
    Mat transformMatrix = getPerspectiveTransform(points, dstPoints);
    InverTtransformMatrix = getPerspectiveTransform(dstPoints, points);

    warpPerspective(frame, birdview, transformMatrix, Size(width, height));
}

// Обнаружение линий методом Hough
void Hough::detectLaneLines() {
    Mat gray;
    cvtColor(birdview, gray, COLOR_BGR2GRAY);

    Canny(gray, gray, size, threshold);

    vector<Vec2f> lines;

    double theta_rad = (Theta+1) * CV_PI / 180.0; // Преобразуем theta в радианы
    cv::HoughLines(gray, lines, Rho+1, theta_rad, Threshold+1);

    Mat result = Mat::zeros(birdview.size(), birdview.type());
    
    for (size_t i = 0; i < lines.size(); i++) {
        float rho = lines[i][0];
        float theta = lines[i][1];
        double a = cos(theta), b = sin(theta);
        double x0 = a * rho, y0 = b * rho;
        cv::Point pt1(cvRound(x0 + 1000 * (-b)), cvRound(y0 + 1000 * a));
        cv::Point pt2(cvRound(x0 - 1000 * (-b)), cvRound(y0 - 1000 * a));
        cv::line(result, pt1, pt2, cv::Scalar(255, 255, 255), 2, cv::LINE_AA);
    }
    warpPerspective(result, invertBird, InverTtransformMatrix, frame.size());

    imshow("Birdview", birdview);
    imshow("invert", invertBird);
    imshow("Canny", gray);
}

// Callback функция для трекбаров
void Hough::onTrackbar(int, void* userdata) {
    auto* processor = reinterpret_cast<Hough*>(userdata);

    int centerX = processor->frame.cols / 2;

    // Обновляем точки
    processor->points[0].x = centerX - processor->xOffsettop; // Левая верхняя
    processor->points[0].y = processor->topY;
    processor->points[1].x = centerX + processor->xOffsettop; // Правая верхняя
    processor->points[1].y = processor->topY;

    processor->points[2].x = centerX + processor->xOffsetdown; // Правая нижняя
    processor->points[2].y = processor->bottomY;
    processor->points[3].x = centerX - processor->xOffsetdown; // Левая нижняя
    processor->points[3].y = processor->bottomY;

    processor->updateBirdview();
    processor->detectLaneLines();

}


void Hough::createTrackbars() {
    int maxCols = frame.cols > 0 ? frame.cols - 1 : 1000;
    int maxRows = frame.rows > 0 ? frame.rows - 1 : 1000;

    createTrackbar("X Offset top", "Birdview", &xOffsettop, maxCols+400, onTrackbar, this);
    createTrackbar("X Offset down", "Birdview", &xOffsetdown, maxCols+400, onTrackbar, this);

    createTrackbar("Top Y", "Birdview", &topY, maxRows, onTrackbar, this);
    createTrackbar("Bottom Y", "Birdview", &bottomY, maxRows, onTrackbar, this);

    // Параметры Hough
    createTrackbar("width", "Birdview", &width, 500, onTrackbar, this);
    createTrackbar("height", "Birdview", &height, 500, onTrackbar, this);

    createTrackbar("Rho", "Birdview", &Rho, 500, onTrackbar, this);
    createTrackbar("Theta", "Birdview", &Theta, 50, onTrackbar, this);
    createTrackbar("Threshold", "Birdview", &Threshold, 200, onTrackbar, this);

    createTrackbar("Canny threshold", "Canny", &threshold, 1000, onTrackbar, this);
    createTrackbar("Canny size", "Canny", &size, 500, onTrackbar, this);
}

// Запуск обработки изображений
void Hough::run() {
    string outputDir = "../images_hough";
    if (!fs::exists(outputDir)) {
        fs::create_directory(outputDir);
    }

    int index = 0;
    for (const auto& entry : fs::directory_iterator(imagesDir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".png") {
            frame = imread(entry.path().string());
            if (frame.empty()) {
                cerr << "Не удалось загрузить изображение: " << entry.path() << endl;
                continue;
            }

            updateBirdview();
            detectLaneLines();

            
        }
        cv::waitKey();
                    // Сохранение результата
            string outputFilename = outputDir + "/Images_" + to_string(index++) + ".png";
            imwrite(outputFilename, invertBird);
    }
}

// Функция для вызова из динамической библиотеки
extern "C" void findLines(const std::string imagesDir) {
    const std::string& configPath = "../res/hough_parameters.yaml";

    Hough processor(imagesDir, configPath);
    processor.run();
}



