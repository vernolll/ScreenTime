#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QWidget>
#include <QHash>
#include <QMap>
#include <QDate>

class backend : public QObject
{
    Q_OBJECT
public:
    backend();
    void get_info();
    QHash <QDate, QMap<QString, int>> data;
};

#endif // BACKEND_H
