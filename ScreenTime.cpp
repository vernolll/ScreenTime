#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <chrono>
#include <thread>
#include <json/json.h>
#include <ctime>
#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#endif

// Функция для получения имени активного приложения
std::string getActiveWindowTitle() {
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

// Функция для получения текущего месяца и года в формате "MM-YYYY"
std::string getCurrentMonthYear() {
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);
    char buffer[16];
    std::strftime(buffer, sizeof(buffer), "%m-%Y", localTime);
    return std::string(buffer);
}

// Функция для записи данных в JSON файл
void writeToJson(const std::unordered_map<std::string, int>& appTimes, const std::string& filename) {
    Json::Value root;

    // Проверка существования файла и чтение данных, если файл существует
    std::ifstream inputFile(filename, std::ios::in);
    if (inputFile.is_open()) {
        Json::CharReaderBuilder reader;
        JSONCPP_STRING errs;
        if (!Json::parseFromStream(reader, inputFile, &root, &errs)) {
            std::cerr << "Error parsing existing JSON: " << errs << std::endl;
        }
        inputFile.close();
    }
    else {
        // Если файла нет, создать пустую структуру JSON
        std::ofstream outputFile(filename, std::ios::out);
        if (outputFile.is_open()) {
            Json::StreamWriterBuilder writer;
            outputFile << Json::writeString(writer, root);
            outputFile.close();
        }
        else {
            std::cerr << "Unable to create file." << std::endl;
        }
    }

    // Получаем текущий месяц и год
    std::string currentMonthYear = getCurrentMonthYear();

    // Обновление данных в JSON
    for (const auto& [app, time] : appTimes) {
        // Проверка, если запись для приложения уже существует
        if (!root.isMember(currentMonthYear)) {
            root[currentMonthYear] = Json::Value(Json::objectValue);  // Если нет, создаем структуру для месяца
        }

        if (root[currentMonthYear].isMember(app)) {
            // Если приложение уже есть, добавляем к существующему времени
            int currentTime = root[currentMonthYear][app].asInt();
            root[currentMonthYear][app] = currentTime + time;
        }
        else {
            // Если приложения нет, создаем новую запись с текущим временем
            root[currentMonthYear][app] = time;
        }
    }

    // Открываем файл для записи и перезаписываем данные
    std::ofstream outputFile(filename, std::ios::out | std::ios::trunc);
    if (outputFile.is_open()) {
        Json::StreamWriterBuilder writer;
        outputFile << Json::writeString(writer, root);
        outputFile.close();
    }
    else {
        std::cerr << "Unable to open file for writing." << std::endl;
    }
}

int main() {
    const std::string outputFilename = "screen_time.json";
    std::unordered_map<std::string, int> appTimes; // Хранит время для каждого приложения

    std::string lastApp;
    auto lastSwitchTime = std::chrono::steady_clock::now();
    const auto saveInterval = std::chrono::seconds(5); // Интервал сохранения в файл
    auto lastSaveTime = std::chrono::steady_clock::now();

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1)); // Проверка каждую секунду

        std::string currentApp = getActiveWindowTitle();
        auto now = std::chrono::steady_clock::now();

        if (currentApp != lastApp) {
            // Подсчёт времени для предыдущего приложения
            int duration = std::chrono::duration_cast<std::chrono::seconds>(now - lastSwitchTime).count();
            if (!lastApp.empty()) {
                appTimes[lastApp] += duration; // Добавляем время для предыдущего приложения
            }

            // Переключение на новое приложение
            lastApp = currentApp;
            lastSwitchTime = now;
        }

        // Сохранение данных каждые `saveInterval` минут
        if (std::chrono::duration_cast<std::chrono::seconds>(now - lastSaveTime) >= saveInterval) {
            writeToJson(appTimes, outputFilename);
            lastSaveTime = now;
        }
    }

    return 0;
}
