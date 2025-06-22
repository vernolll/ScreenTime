#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QString>
#include <QApplication>

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

    QPushButton* btnDay;
    QPushButton* btnWeek;
    QMap<QDate, QMap<QString, int>> data;

    QChartView* chartView;
    QBarSet* set;
    QBarSeries* series;
    QBarCategoryAxis* axisX;
    QValueAxis* axisY;
    QChart* chart;

    backend back;

    void setupUi();
    void showDayChart();
    void showWeekChart();
    void updateChart(const QMap<QString, int>& usage, const QString& labelText);
};
#endif // MAINWINDOW_H
