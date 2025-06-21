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
    QMap<QDate, QMap<QString, int>> get_info();
};

#endif // BACKEND_H
