#include "ScreenTime.h"
#include <iostream>
#include <fstream>
#include <thread>
#include <filesystem>

ScreenTimeAPI::ScreenTimeAPI(const std::string& filename)
    : outputFilename(filename), lastApp("Unknown"),
    lastSwitchTime(std::chrono::steady_clock::now()) 
{
}


std::string ScreenTimeAPI::getActiveWindowTitle() 
{
#ifdef _WIN32
    HWND hwnd = GetForegroundWindow();
    if (hwnd) 
    {
        DWORD processId;
        GetWindowThreadProcessId(hwnd, &processId);

        HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
        if (processHandle) 
        {
            char processName[MAX_PATH];
            if (GetModuleBaseNameA(processHandle, NULL, processName, sizeof(processName)))
            {
                CloseHandle(processHandle);
                return std::string(processName);
            }
            CloseHandle(processHandle);
        }
    }
#endif
    return "Unknown";
}

std::string ScreenTimeAPI::getCurrentDayMonthYear() 
{
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);
    char buffer[16];
    std::strftime(buffer, sizeof(buffer), "%d-%m-%Y", localTime);
    return std::string(buffer);
}

void ScreenTimeAPI::writeToJson()
{
    Json::Value root;

    std::ifstream inputFile(outputFilename, std::ios::in);
    if (inputFile.is_open()) 
    {
        Json::CharReaderBuilder reader;
        JSONCPP_STRING errs;
        if (!Json::parseFromStream(reader, inputFile, &root, &errs)) 
        {
            std::cerr << "Error parsing existing JSON: " << errs << std::endl;
        }
        inputFile.close();
    }

    std::string currentDayMonthYear = getCurrentDayMonthYear();

    for (const auto& [app, time] : appTimes) 
    {
        root[currentDayMonthYear][app] = root[currentDayMonthYear][app].asInt() + time;
    }

    std::ofstream outputFile(outputFilename, std::ios::out | std::ios::trunc);
    if (outputFile.is_open())
    {
        Json::StreamWriterBuilder writer;
        outputFile << Json::writeString(writer, root);
        outputFile.close();
    }
    else 
    {
        std::cerr << "Unable to open file for writing." << std::endl;
    }
}

void ScreenTimeAPI::trackScreenTime() 
{
    const auto saveInterval = std::chrono::seconds(5); 
    auto lastSaveTime = std::chrono::steady_clock::now();

    while (true) 
    {
        std::this_thread::sleep_for(std::chrono::seconds(1)); 

        std::string currentApp = getActiveWindowTitle();
        auto now = std::chrono::steady_clock::now();

        if (currentApp.empty() || currentApp == "Unknown")
        {
            continue; 
        }

        if (currentApp != lastApp)
        {
            int duration = std::chrono::duration_cast<std::chrono::seconds>(now - lastSwitchTime).count();
            if (!lastApp.empty() && lastApp != "Unknown") 
            {
                appTimes[lastApp] += duration;
            }

            lastApp = currentApp;
            lastSwitchTime = now;
        }

        if (std::chrono::duration_cast<std::chrono::seconds>(now - lastSaveTime) >= saveInterval) 
        {
            int currentDuration = std::chrono::duration_cast<std::chrono::seconds>(now - lastSwitchTime).count();
            if (!currentApp.empty() && currentApp != "Unknown") 
            {
                appTimes[currentApp] += currentDuration;
            }

            writeToJson();
            lastSaveTime = now;

            appTimes.clear();
            lastSwitchTime = now;
        }
    }
}