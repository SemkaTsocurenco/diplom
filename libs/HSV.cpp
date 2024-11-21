#include "../include/main.hpp"


int h_min, h_max;
int s_min, s_max;
int v_min, v_max;


// Функция для загрузки HSV параметров из YAML-файла
void loadHSVParams(const std::string& filename) {    
    std::cout<<"load HSV parameters\n";
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
    std::cout<<"load HSV parameters - DONE\n";

}
    
// Функция для сохранения текущих HSV параметров в YAML-файл
void saveHSVParams(const std::string& filename) {
    std::cout<<"save HSV parameters\n";
    cv::FileStorage fs(filename, cv::FileStorage::WRITE);    
    if (!fs.isOpened()) {
        std::cerr << "Ошибка: не удалось открыть файл " << filename << " для записи." << std::endl;        
        return;
    }
    fs << "H_Min" << h_min;    fs << "H_Max" << h_max;
    fs << "S_Min" << s_min;    fs << "S_Max" << s_max;
    fs << "V_Min" << v_min;    fs << "V_Max" << v_max;
    fs.release();
    std::cout<<"save HSV parameters - DONE\n";
}


void processFrame(const cv::Mat& frame, cv::Mat& output) {
    cv::Mat hsv;
    cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);

    cv::Scalar lower(h_min, s_min, v_min);
    cv::Scalar upper(h_max, s_max, v_max);

    cv::inRange(hsv, lower, upper, output);
}

void on_trackbar(int, void*) {}

extern "C" void findLines(const std::string imagesDir) {

    std::string outputDir = "../images_HSV";
    if (!fs::exists(outputDir)) {
        fs::create_directory(outputDir);
    }
            cv::Mat mask;
    int index = 0;
    for (const auto& entry : fs::directory_iterator(imagesDir)) {
        while (entry.is_regular_file() && entry.path().extension() == ".png") {
            cv::Mat frame = cv::imread(entry.path().string());
            if (frame.empty()) {
                std::cerr << "Не удалось загрузить изображение: " << entry.path() << std::endl;
                continue;
            }

            std::cout<< "HSV lib go on\n";

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
            // Обработка кадра с текущими параметрами HSV
            processFrame(frame, mask);

            // Отображение исходного кадра и обработанного маскированного изображения
            cv::imshow("Original", frame);
            cv::imshow("Masked", mask);
            // Сохранение результата

            char key = (char)cv::waitKey(10);

            if (key == 'q') {
                break;
            }
        }
        std::string outputFilename = outputDir + "/Images_" + std::to_string(index++) + ".png";
        cv::imwrite(outputFilename, mask);
        cv::waitKey();

    }

}