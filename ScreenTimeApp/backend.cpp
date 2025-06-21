#include "backend.h"
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QDebug>

backend::backend() 
{
    get_info();
}

QMap<QDate, QMap<QString, int>> backend::get_info() {
    QMap<QDate, QMap<QString, int>> result;

    QString filePath = "../../screen_time.json";
    qDebug() << "Current working directory:" << QDir::currentPath();

    QFile file(filePath);
    if (!file.exists()) {
        qDebug() << "File does not exist at:" << filePath;
        return result;
    }

    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open JSON file at:" << filePath;
        return result;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
    if (jsonDoc.isNull() || !jsonDoc.isObject()) {
        qDebug() << "Failed to parse JSON file.";
        return result;
    }

    QJsonObject jsonObj = jsonDoc.object();

    for (auto it = jsonObj.begin(); it != jsonObj.end(); ++it) {
        QString dateStr = it.key();
        if (dateStr == "Date") continue;

        QDate date = QDate::fromString(dateStr, "dd-MM-yyyy");
        if (!date.isValid()) {
            qDebug() << "Invalid date format in JSON key:" << dateStr;
            continue;
        }

        QJsonValue value = it.value();
        if (value.isObject()) {
            QJsonObject appObj = value.toObject();
            QMap<QString, int> appUsage;

            for (auto appIt = appObj.begin(); appIt != appObj.end(); ++appIt) {
                QString appName = appIt.key();
                int timeSpent = appIt.value().toInt();
                appUsage.insert(appName, timeSpent);
            }

            result[date] = appUsage;
        }
    }

    return result;
}
