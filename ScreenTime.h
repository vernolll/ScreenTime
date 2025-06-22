#ifndef SCREENTIMEAPI_H
#define SCREENTIMEAPI_H

#include <string>
#include <unordered_map>
#include <json/json.h>
#include <chrono>

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#endif

class ScreenTimeAPI 
{
public:
    ScreenTimeAPI(const std::string& filename);
    void trackScreenTime();

private:
    std::string getActiveWindowTitle();
    std::string getCurrentDayMonthYear();
    void writeToJson();

    std::string outputFilename;
    std::unordered_map<std::string, int> appTimes;
    std::string lastApp;
    std::chrono::steady_clock::time_point lastSwitchTime; 
};

#endif
