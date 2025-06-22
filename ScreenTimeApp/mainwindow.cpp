#include "mainwindow.h"


MainWindow::MainWindow(QWidget* parent) : QDialog(parent)
{
    setupUi();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUi()
{
    backend back;
    data = back.get_info();

    setWindowTitle("Screen time");
    this->resize(QApplication::primaryScreen()->availableGeometry().size());
    this->move(0, 0);

    this->setStyleSheet("background-color: #FAFAFA; color: #333333; font-family: 'SF Pro Text', 'Segoe UI', Arial;");

    label1 = new QLabel;
    label2 = new QLabel("Today");
    label2->setStyleSheet("font-size: 14px; color: #666666;");
    label2->setAlignment(Qt::AlignCenter);

    QString btnStyle = R"(
        QPushButton {
            background-color: #E0E0E0;
            border-radius: 12px;
            padding: 8px 16px;
            font-weight: 600;
            color: #555555;
            min-width: 80px;
        }
        QPushButton:hover {
            background-color: #D0D0D0;
        }
        QPushButton:pressed {
            background-color: #B0B0B0;
        }
    )";

    btnDay = new QPushButton("Day");
    btnWeek = new QPushButton("Week");
    btnDay->setStyleSheet(btnStyle);
    btnWeek->setStyleSheet(btnStyle);

    connect(btnDay, &QPushButton::clicked, this, &MainWindow::showDayChart);
    connect(btnWeek, &QPushButton::clicked, this, &MainWindow::showWeekChart);

    set = new QBarSet("");
    series = new QBarSeries();
    series->append(set);

    chart = new QChart();
    chart->addSeries(series);
    chart->legend()->hide();
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->setBackgroundVisible(false);

    axisX = new QBarCategoryAxis();
    axisY = new QValueAxis();

    axisY->setRange(0, 70);
    axisY->setTickCount(6);
    axisY->setLabelFormat("%d min");
    axisY->setGridLineColor(QColor("#EEEEEE"));
    axisY->setLabelsColor(QColor("#999999"));

    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setStyleSheet("background: transparent;");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 20, 24, 20);
    mainLayout->setSpacing(12);

    mainLayout->addWidget(label1);
    mainLayout->addWidget(label2);

    QHBoxLayout* modeLayout = new QHBoxLayout();
    modeLayout->addStretch();
    modeLayout->addWidget(btnDay);
    modeLayout->addWidget(btnWeek);
    modeLayout->addStretch();

    mainLayout->addLayout(modeLayout);
    mainLayout->addWidget(chartView, 1);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(16);
    buttonLayout->addStretch();
    buttonLayout->addStretch();

    mainLayout->addLayout(buttonLayout);

    showDayChart();
}


void MainWindow::showWeekChart()
{
    chart->removeAllSeries();
    if (axisX) 
    {
        chart->removeAxis(axisX);
        delete axisX;
    }
    axisX = new QBarCategoryAxis();

    QDate today = QDate::currentDate();
    QList<QDate> last7Days;
    for (int i = 6; i >= 0; --i)
        last7Days.append(today.addDays(-i));

    QSet<QString> allApps;
    for (const QDate& day : last7Days)
    {
        if (data.contains(day))
        {
            QSet<QString> keysSet;
            for (const QString& key : data[day].keys())
                keysSet.insert(key);
            allApps.unite(keysSet);
        }
    }

    QBarSeries* weekSeries = new QBarSeries();
    QMap<QString, QBarSet*> appBarSets;
    for (const QString& app : allApps)
    {
        QBarSet* barSet = new QBarSet(app);
        appBarSets[app] = barSet;
        weekSeries->append(barSet);
    }

    for (const QDate& day : last7Days)
    {
        for (const QString& app : allApps) 
        {
            int usage = data.value(day).value(app, 0);
            appBarSets[app]->append(usage);
        }
    }

    QStringList categories;
    for (const QDate& day : last7Days)
    {
        categories << QLocale().dayName(day.dayOfWeek(), QLocale::ShortFormat);
    }

    axisX->append(categories);
    axisX->setLabelsColor(QColor("#999999"));

    chart->addSeries(weekSeries);
    chart->setAxisX(axisX, weekSeries);
    chart->setAxisY(axisY, weekSeries);

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);
    label2->setText("Last 7 days");

    int total = 0;
    for (const QString& app : allApps)
    {
        for (const QDate& day : last7Days) 
        {
            total += data.value(day).value(app, 0);
        }
    }
    label1->setText(QString("Total usage time: %1 min").arg(total));
}

void MainWindow::showDayChart()
{
    chart->removeAllSeries();
    chart->removeAxis(axisX);
    axisX = new QBarCategoryAxis();

    QDate today = QDate::currentDate();
    QMap<QString, int> todayData = data.value(today);

    QBarSet* daySet = new QBarSet("");
    QStringList categories;
    int total = 0;
    for (auto it = todayData.begin(); it != todayData.end(); ++it)
    {
        categories << it.key();
        daySet->append(it.value());
        total += it.value();
    }
    if (categories.isEmpty())
    {
        categories << "No data";
        daySet->append(0);
    }

    QBarSeries* daySeries = new QBarSeries();
    daySeries->append(daySet);

    axisX->append(categories);
    axisX->setLabelsColor(QColor("#999999"));

    chart->addSeries(daySeries);
    chart->setAxisX(axisX, daySeries);
    chart->setAxisY(axisY, daySeries);

    chart->legend()->setVisible(false);

    label1->setText(QString("Total usage time: %1 min").arg(total));
    label2->setText("Today");
}

void MainWindow::updateChart(const QMap<QString, int>& usage, const QString& labelText)
{
    set->remove(0, set->count());
    axisX->clear();             

    QStringList categories;
    int total = 0;

    for (auto it = usage.begin(); it != usage.end(); ++it) 
    {
        categories << it.key();
        *set << it.value();
        total += it.value();
    }

    if (categories.isEmpty()) 
    {
        categories << "No data";
        *set << 0;
    }

    axisX->append(categories);
    axisX->setLabelsColor(QColor("#999999"));
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    label1->setText(QString("Total usage time: %1 min").arg(total));
    label2->setText(labelText);
}
