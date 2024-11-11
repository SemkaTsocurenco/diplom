#include "../include/main.hpp"

int maxCorners = 100;     // Максимальное количество углов для обнаружения
double qualityLevel = 2; // Уровень качества для углов
double minDistance = 7;  // Минимальное расстояние между углами

// Функция для загрузки параметров оптического потока из YAML-файла
void loadOpticalFlowParams(const std::string& filename) {
    std::cout << "Загрузка параметров оптического потока\n";
    cv::FileStorage fs(filename, cv::FileStorage::READ);
    if (!fs.isOpened()) {
        std::cerr << "Ошибка: не удалось открыть файл " << filename << " для чтения." << std::endl;
        return;
    }
    fs["MaxCorners"] >> maxCorners;
    fs["QualityLevel"] >> qualityLevel;
    fs["MinDistance"] >> minDistance;
    fs.release();
    std::cout << "Загрузка параметров оптического потока - завершена\n";
}

// Функция для сохранения параметров оптического потока в YAML-файл
void saveOpticalFlowParams(const std::string& filename) {
    std::cout << "Сохранение параметров оптического потока\n";
    cv::FileStorage fs(filename, cv::FileStorage::WRITE);
    if (!fs.isOpened()) {
        std::cerr << "Ошибка: не удалось открыть файл " << filename << " для записи." << std::endl;
        return;
    }
    fs << "MaxCorners" << maxCorners;
    fs << "QualityLevel" << qualityLevel;
    fs << "MinDistance" << minDistance;
    fs.release();
    std::cout << "Сохранение параметров оптического потока - завершено\n";
}

// Функция для обработки кадра с использованием оптического потока
void processOpticalFlow(const cv::Mat& prevFrame, const cv::Mat& currFrame, cv::Mat& output) {
    std::vector<cv::Point2f> prevPoints, currPoints;
    std::vector<uchar> status;
    std::vector<float> err;

    // Обнаружение углов на предыдущем кадре
    cv::goodFeaturesToTrack(prevFrame, prevPoints, maxCorners, qualityLevel, minDistance);

    if (prevPoints.empty()) {
        std::cerr << "Не удалось обнаружить интересные точки на кадре." << std::endl;
        return;
    } else {

    // Вычисление оптического потока
    cv::calcOpticalFlowPyrLK(prevFrame, currFrame, prevPoints, currPoints, status, err);
    
    output = currFrame.clone();
    // Отрисовка обнаруженных точек движения
    for (size_t i = 0; i < currPoints.size(); i++) {
        if (status[i]) {
            cv::line(output, prevPoints[i], currPoints[i], cv::Scalar(0, 255, 0), 2);
            cv::circle(output, currPoints[i], 5, cv::Scalar(0, 0, 255), -1);
        }
    }
    }
}

// Функция-обработчик для ползунков
void on_trackbar(int, void*) {}

// Основная функция поиска дорожной разметки с использованием оптического потока
void findLines(std::string vidname) {
    cv::Mat prevFrame, currFrame, grayPrev, grayCurr;
    loadOpticalFlowParams("../res/optical_flow_parameters.yaml");
    cv::VideoCapture cap(vidname);

    if (!cap.isOpened()) {
        std::cerr << "Ошибка: Не удалось открыть видео!" << std::endl;
        return;
    }

    // Захватываем первый кадр
    cap >> prevFrame;
    if (prevFrame.empty()) {
        std::cerr << "Ошибка: Не удалось захватить первый кадр!" << std::endl;
        return;
    }
    cv::cvtColor(prevFrame, grayPrev, cv::COLOR_BGR2GRAY);

    while (true) {
        cap >> currFrame;
        if (currFrame.empty()) {
            std::cerr << "Ошибка: Не удалось захватить кадр!" << std::endl;
            break;
        }

        if (cv::waitKey(30) == 'q') break;

        // Преобразование текущего кадра в оттенки серого
        cv::cvtColor(currFrame, grayCurr, cv::COLOR_BGR2GRAY);

        // Создаем окна
        cv::namedWindow("Original", cv::WINDOW_AUTOSIZE);
        cv::namedWindow("Optical Flow", cv::WINDOW_AUTOSIZE);

        // Ползунки для настройки параметров оптического потока
        cv::createTrackbar("Max Corners", "Optical Flow", &maxCorners, 500, on_trackbar);
        cv::createTrackbar("Quality Level", "Optical Flow", nullptr, 100.0, on_trackbar);
        cv::createTrackbar("Min Distance", "Optical Flow", nullptr, 100.0, on_trackbar);

        // Обработка кадра с использованием оптического потока
        cv::Mat flowOutput;
        processOpticalFlow(grayPrev, grayCurr, flowOutput);

        // Отображение исходного кадра и результатов оптического потока
        cv::imshow("Original", currFrame);
        cv::imshow("Optical Flow", flowOutput);

        // Обновляем предыдущий кадр
        grayPrev = grayCurr.clone();
    }

    saveOpticalFlowParams("../res/optical_flow_parameters.yaml");
    cap.release();
    cv::destroyAllWindows();
}