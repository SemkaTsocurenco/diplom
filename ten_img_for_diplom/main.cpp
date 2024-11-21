#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <dlfcn.h>
#include <dirent.h>



void on_trackbar(int, void*) {}


int main(int argc, char *argv[]) {

    std::vector<std::string> argList;
    for (int i = 0; i < argc; i++)
        argList.push_back(argv[i]);

    cv::Mat frame;
    cv::VideoCapture cap("/home/tsokurenkosv/Downloads/video2.mp4"); // Загрузка видеофайла
    if (!cap.isOpened()) {
        std::cerr << "Ошибка: Не удалось открыть файл видео!" << std::endl;
        return -1;
    }

    std::cout<<argList[1]<<"\n";

    size_t frame_index = 0;
    if (argList[1] == "new_img"){
        while (true) {
            cap >> frame;
            if (frame.empty()) {
                std::cerr << "Ошибка: Конец видео или не удалось захватить кадр!" << std::endl;
                break;
            }

            frame_index++;
            int count = 30;
                if (frame_index % count == 0) {
                    std::string filename = "../images/Image_" + std::to_string(frame_index / count) + ".png";
                    cv::imwrite(filename, frame);
                    std::cout<<"Изображение " + std::to_string(frame_index / count) + " сохранено\n";
                }

            if (cv::waitKey(30) == 'q') break; 
        }
    }


    if (argList[1] == "night_img"){
        cv::Mat Night_frame;
        int file_count = 0;
        DIR * dirp;
        struct dirent * entry;  
        dirp = opendir("../images/"); 
        while ((entry = readdir(dirp)) != NULL) {
            if (entry->d_type == DT_REG) {
                file_count++;
            }
        }
        std::cout<<"file count : "<<file_count<<"\n";

        for (int i = 0 ; i<file_count ; i++){
            Night_frame = cv::imread("../images/Image_" + std::to_string(i) + ".png");
            Night_frame /= 5; 
            std::string filename = "../images/Image_" + std::to_string(file_count+i) + ".png";
            cv::imwrite(filename, Night_frame);
            std::cout<<"Изображение " + std::to_string(file_count + i+1) + " сохранено\n";
        }
    }

    if (argList[1] == "get_bin"){
        cv::Mat bin_frame;
        cv::Mat combined;
        int file_count = 0;
        DIR * dirp;
        struct dirent * entry;  
        dirp = opendir("../images/"); 
        while ((entry = readdir(dirp)) != NULL) {
            if (entry->d_type == DT_REG) {
                file_count++;
            }
        }
        int h_min, h_max;
        int s_min, s_max;
        int v_min, v_max;

        for (int i = 0 ; i<file_count ; i++){

            std::string filename = "../images/Image_" + std::to_string(i) + ".png";
            std::string resname = "../results/Image_" + std::to_string(i) + ".png";
            frame = cv::imread(filename);
            cv::namedWindow("Masked", cv::WINDOW_AUTOSIZE);


            while ((cv::waitKey(30) != 's') && (cv::waitKey(30) != 'q')){
                cv::createTrackbar("H Min", "Masked", &h_min, 255, on_trackbar);
                cv::createTrackbar("H Max", "Masked", &h_max, 255, on_trackbar);
                cv::createTrackbar("S Min", "Masked", &s_min, 255, on_trackbar);
                cv::createTrackbar("S Max", "Masked", &s_max, 255, on_trackbar);
                cv::createTrackbar("V Min", "Masked", &v_min, 255, on_trackbar);
                cv::createTrackbar("V Max", "Masked", &v_max, 255, on_trackbar);

                cv::cvtColor(frame, bin_frame, cv::COLOR_BGR2HSV);
                cv::Scalar lower(h_min, s_min, v_min);
                cv::Scalar upper(h_max, s_max, v_max);

                cv::inRange(frame, lower, upper, bin_frame);
                cv::cvtColor(bin_frame, bin_frame, cv::COLOR_GRAY2BGR);  // Преобразование edges в цветное изображение
                cv::hconcat(frame, bin_frame, combined);

                cv::imshow("Masked", combined);
                if (cv::waitKey(30) == 'q') break; 
            }

            cv::imwrite(resname, bin_frame);

            std::cout<<"bin Изображение " + std::to_string(i) + " сохранено\n";
        }

    }


    cap.release();
    cv::destroyAllWindows();
    return 0;
}
