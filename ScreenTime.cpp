#include <iostream>
#include <json/json.h>  // Подключаем jsoncpp

int main() {
    // Создаем объект JSON
    Json::Value root;
    root["name"] = "ScreenTime";
    root["version"] = "1.0";

    // Выводим его
    std::cout << root << std::endl;

    return 0;
}
