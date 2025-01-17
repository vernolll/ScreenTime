#include "backend.h"
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QDebug>

backend::backend() {}

void backend::get_info() {
    QString filePath = "../../../screen_time.json";

    QFile file(filePath);
    if (!file.exists()) {
        qDebug() << "File does not exist at:" << filePath;
        return;
    }

    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open JSON file at:" << filePath;
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
    if (jsonDoc.isNull() || !jsonDoc.isObject()) {
        qDebug() << "Failed to parse JSON file.";
        return;
    }

    QJsonObject jsonObj = jsonDoc.object();

    for (auto it = jsonObj.begin(); it != jsonObj.end(); ++it) {
        QString dateStr = it.key();
        if (dateStr == "Date") continue;

        QDate date = QDate::fromString(dateStr, "dd-MM-yyyy");

        QJsonValue value = it.value();
        if (value.isObject()) {
            QJsonObject appObj = value.toObject();
            QMap<QString, int> appUsage;

            for (auto appIt = appObj.begin(); appIt != appObj.end(); ++appIt) {
                QString appName = appIt.key();
                int timeSpent = appIt.value().toInt();
                appUsage.insert(appName, timeSpent);
            }

            this->data[date] = appUsage;
        }
    }

    for (auto dateIt = this->data.begin(); dateIt != this->data.end(); ++dateIt) {
        qDebug() << "Date:" << dateIt.key().toString("dd-MM-yyyy");
        QMap<QString, int> appUsage = dateIt.value();
        for (auto appIt = appUsage.begin(); appIt != appUsage.end(); ++appIt) {
            qDebug() << "  App:" << appIt.key() << ", Time:" << appIt.value();
        }
    }
}
