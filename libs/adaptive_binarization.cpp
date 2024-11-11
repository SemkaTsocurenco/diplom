#include "../include/main.hpp"

int blockSize = 11; // Размер блока для адаптивной бинаризации (нечетное значение)
int C = 2;          // Константа для адаптивной бинаризации

// Функция для загрузки параметров адаптивной бинаризации из YAML-файла
void loadAdaptiveParams(const std::string& filename) {
    std::cout << "Загрузка параметров адаптивной бинаризации\n";
    cv::FileStorage fs(filename, cv::FileStorage::READ);
    if (!fs.isOpened()) {
        std::cerr << "Ошибка: не удалось открыть файл " << filename << " для чтения." << std::endl;
        return;
    }
    // Загрузка параметров
    fs["BlockSize"] >> blockSize;
    fs["C"] >> C;
    fs.release();
    std::cout << "Загрузка параметров адаптивной бинаризации - завершена\n";
}

// Функция для сохранения параметров адаптивной бинаризации в YAML-файл
void saveAdaptiveParams(const std::string& filename) {
    std::cout << "Сохранение параметров адаптивной бинаризации\n";
    cv::FileStorage fs(filename, cv::FileStorage::WRITE);
    if (!fs.isOpened()) {
        std::cerr << "Ошибка: не удалось открыть файл " << filename << " для записи." << std::endl;
        return;
    }
    // Сохранение параметров
    fs << "BlockSize" << blockSize;
    fs << "C" << C;
    fs.release();
    std::cout << "Сохранение параметров адаптивной бинаризации - завершено\n";
}

// Функция обработки кадра с использованием адаптивной бинаризации
void processAdaptiveThreshold(const cv::Mat& frame, cv::Mat& output) {
    cv::Mat gray;
    // Преобразование в оттенки серого
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    // Применение адаптивной бинаризации
    cv::adaptiveThreshold(gray, output, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, blockSize, C);
}

// Функция-обработчик для ползунков
void on_trackbar(int, void*) {}

// Основная функция поиска дорожной разметки
void findLines(std::string vidname) {
    cv::Mat frame;
    loadAdaptiveParams("../res/adaptive_parameters.yaml");
    cv::VideoCapture cap(vidname);

    if (!cap.isOpened()) {
        std::cerr << "Ошибка: Не удалось открыть видео!" << std::endl;
        return;
    }

    while (true) {
        cap >> frame;
        if (frame.empty()) {
            std::cerr << "Ошибка: Не удалось захватить кадр!" << std::endl;
            break;
        }

        if (cv::waitKey(30) == 'q') break;

        // Создаем окна
        cv::namedWindow("Original", cv::WINDOW_AUTOSIZE);
        cv::namedWindow("Adaptive Threshold", cv::WINDOW_AUTOSIZE);

        // Ползунки для настройки адаптивной бинаризации
        cv::createTrackbar("Block Size", "Adaptive Threshold", &blockSize, 101, on_trackbar);
        cv::createTrackbar("C", "Adaptive Threshold", &C, 20, on_trackbar);

        // Обработка кадра
        if (blockSize % 2 == 0) blockSize += 1; // Размер блока должен быть нечетным
        cv::Mat binary;
        processAdaptiveThreshold(frame, binary);

        // Отображение исходного кадра и результатов адаптивной бинаризации
        // cv::imshow("Original", frame);
        cv::imshow("Adaptive Threshold", binary);
    }

    saveAdaptiveParams("../res/adaptive_parameters.yaml");
    cap.release();
    cv::destroyAllWindows();
}