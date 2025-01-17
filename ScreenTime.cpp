#include "ScreenTime.h"
#include <iostream>
#include <fstream>
#include <thread>
#include <filesystem>

// Конструктор класса
ScreenTimeAPI::ScreenTimeAPI(const std::string& filename)
    : outputFilename(filename), lastApp("Unknown"),
    lastSwitchTime(std::chrono::steady_clock::now()) {

    std::string currentDir = std::filesystem::current_path().string();
    
    if (currentDir.find("build") != std::string::npos) {
        // Например, поднимаемся на уровень выше, если программа запущена из папки сборки
        outputFilename = "../" + filename; // Поднимаемся в корень проекта
    }
}

// Функция для получения имени активного приложения
std::string ScreenTimeAPI::getActiveWindowTitle() {
#ifdef _WIN32
    HWND hwnd = GetForegroundWindow();
    if (hwnd) {
        DWORD processId;
        GetWindowThreadProcessId(hwnd, &processId);

        HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
        if (processHandle) {
            char processName[MAX_PATH];
            if (GetModuleBaseNameA(processHandle, NULL, processName, sizeof(processName))) {
                CloseHandle(processHandle);
                return std::string(processName);
            }
            CloseHandle(processHandle);
        }
    }
#endif
    return "Unknown";
}

// Функция для получения текущей даты в формате "DD-MM-YYYY"
std::string ScreenTimeAPI::getCurrentDayMonthYear() {
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);
    char buffer[16];
    std::strftime(buffer, sizeof(buffer), "%d-%m-%Y", localTime);
    return std::string(buffer);
}

// Функция для записи данных в JSON файл
void ScreenTimeAPI::writeToJson() {
    Json::Value root;

    // Проверка существования файла и чтение данных, если файл существует
    std::ifstream inputFile(outputFilename, std::ios::in);
    if (inputFile.is_open()) {
        Json::CharReaderBuilder reader;
        JSONCPP_STRING errs;
        if (!Json::parseFromStream(reader, inputFile, &root, &errs)) {
            std::cerr << "Error parsing existing JSON: " << errs << std::endl;
        }
        inputFile.close();
    }

    // Получаем текущий день, месяц и год
    std::string currentDayMonthYear = getCurrentDayMonthYear();

    // Обновление данных в JSON
    for (const auto& [app, time] : appTimes) {
        root[currentDayMonthYear][app] = root[currentDayMonthYear][app].asInt() + time;
    }

    // Открываем файл для записи и перезаписываем данные
    std::ofstream outputFile(outputFilename, std::ios::out | std::ios::trunc);
    if (outputFile.is_open()) {
        Json::StreamWriterBuilder writer;
        outputFile << Json::writeString(writer, root);
        outputFile.close();
    }
    else {
        std::cerr << "Unable to open file for writing." << std::endl;
    }
}

// Основной метод для отслеживания экранного времени
void ScreenTimeAPI::trackScreenTime() {
    const auto saveInterval = std::chrono::seconds(5); // Интервал сохранения
    auto lastSaveTime = std::chrono::steady_clock::now();

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1)); // Проверка каждую секунду

        std::string currentApp = getActiveWindowTitle();
        auto now = std::chrono::steady_clock::now();

        if (currentApp.empty() || currentApp == "Unknown") {
            continue; // Пропускаем неопределенные окна
        }

        if (currentApp != lastApp) {
            // Подсчёт времени для предыдущего приложения
            int duration = std::chrono::duration_cast<std::chrono::seconds>(now - lastSwitchTime).count();
            if (!lastApp.empty() && lastApp != "Unknown") {
                appTimes[lastApp] += duration; // Добавляем время для предыдущего приложения
            }

            // Переключение на новое приложение
            lastApp = currentApp;
            lastSwitchTime = now;
        }

        // Сохранение данных каждые `saveInterval`
        if (std::chrono::duration_cast<std::chrono::seconds>(now - lastSaveTime) >= saveInterval) {
            // Добавляем время для текущего приложения перед сохранением
            int currentDuration = std::chrono::duration_cast<std::chrono::seconds>(now - lastSwitchTime).count();
            if (!currentApp.empty() && currentApp != "Unknown") {
                appTimes[currentApp] += currentDuration;
            }

            writeToJson();
            lastSaveTime = now;

            // Обнуляем сохраненные интервалы для предотвращения двойного добавления
            appTimes.clear();
            lastSwitchTime = now;
        }
    }
}