#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QString>

#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include "backend.h"

class backend;

class MainWindow : public QDialog
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent);
    ~MainWindow();

private:
    QLabel* label1;
    QLabel* label2;

    QPushButton* btnOk;
    QPushButton* btnCancel;

    QBarSet* set;
    QChart* chart;

    QBarSeries* series;
    backend back;

    void setupUi();

};
#endif // MAINWINDOW_H
