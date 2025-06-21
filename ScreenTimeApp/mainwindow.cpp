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
    QMap<QDate, QMap<QString, int>> data = back.get_info();

    setWindowTitle("Screen time");
    setFixedSize(360, 480);  // Айфон-подобный размер окна (пример)

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

    btnOk = new QPushButton("OK");
    btnOk->setStyleSheet(btnStyle);

    btnCancel = new QPushButton("Cancel");
    btnCancel->setStyleSheet(btnStyle);

    set = new QBarSet("");
    series = new QBarSeries();
    series->append(set);

    chart = new QChart();
    chart->addSeries(series);
    chart->legend()->hide();
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->setBackgroundVisible(false);

    QStringList categories;
    QBarCategoryAxis* axisX = new QBarCategoryAxis();
    QValueAxis* axisY = new QValueAxis();

    axisY->setRange(0, 70);
    axisY->setTickCount(6);
    axisY->setLabelFormat("%d min");
    axisY->setGridLineColor(QColor("#EEEEEE"));
    axisY->setLabelsColor(QColor("#999999"));
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    // Получаем сегодня и данные за сегодня
    QDate today = QDate::currentDate();
    int totalUsage = 0;

    if (data.contains(today)) {
        QMap<QString, int> todayData = data[today];

        for (auto it = todayData.begin(); it != todayData.end(); ++it) {
            categories.append(it.key());
            *set << it.value();
            totalUsage += it.value();
        }
    }

    // Если нет данных за сегодня, для красоты ставим пустые категории
    if (categories.isEmpty()) {
        categories << "No data";
        *set << 0;
    }

    axisX->append(categories);
    axisX->setGridLineVisible(false);
    axisX->setLabelsColor(QColor("#999999"));
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    series->setLabelsVisible(true);
    series->setLabelsPosition(QAbstractBarSeries::LabelsInsideEnd);
    series->setLabelsFormat("%.0f min");

    QChartView* chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setStyleSheet("background: transparent;");

    label1->setText(QString("Total usage time: %1 min").arg(totalUsage));
    label1->setStyleSheet("font-size: 20px; font-weight: 600; padding: 10px 0;");
    label1->setAlignment(Qt::AlignCenter);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 20, 24, 20);
    mainLayout->setSpacing(12);

    mainLayout->addWidget(label1);
    mainLayout->addWidget(label2);
    mainLayout->addWidget(chartView, 1);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(16);
    buttonLayout->addStretch();
    buttonLayout->addWidget(btnCancel);
    buttonLayout->addWidget(btnOk);
    buttonLayout->addStretch();

    mainLayout->addLayout(buttonLayout);

    connect(btnOk, &QPushButton::clicked, this, &QDialog::accept);
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
}