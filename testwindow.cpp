#include "chartcomment.h"
#include "testchart.h"
#include "testwindow.h"
#include "ui_testwindow.h"

#include <QChartView>
#include <QFileDialog>
#include <QLineSeries>
#include <QMessageBox>
#include <QValueAxis>
#include <qchart.h>

TestWindow::TestWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::TestWindow)
    , mtQss(new MTQss(this))
{
    ui->setupUi(this);
}

TestWindow::~TestWindow()
{
    delete ui;
}

void TestWindow::on_actionLoad_QSS_triggered()
{
    QString defaultFileName = "*.qss";
    QString filter= tr("QSS files (*.qss)");

    QString fileName = QFileDialog::getOpenFileName(this, tr("Import QSS"), defaultFileName, filter);

    if (fileName.isEmpty()) {
        return;
    }

    if (!fileName.endsWith(".qss")) {
        fileName += ".qss";
    }

    mtQss->importQss(fileName);
}


void TestWindow::on_actionSaveQSS_triggered()
{
    QString defaultFileName = "styles.qss";
    QString filter = tr("QSS files (*.qss)");

    QString fileName = QFileDialog::getSaveFileName(this, tr("Export QSS"), defaultFileName, filter);

    if (fileName.isEmpty()) {
        return;
    }

    if (!fileName.endsWith(".qss")) {
        fileName += ".qss";
    }

    mtQss->exportQss(fileName);
}

void TestWindow::testChartComment() {

    // Tworzenie wykresu z użyciem klasy TestChart
    TestChart *chart = new TestChart();

    chart->setTitle("Test wykresu z komentarzem");

    // Tworzenie widoku wykresu
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    // Tworzenie sceny i dodanie komentarza
    ChartComment *comment = new ChartComment(QPointF(QDateTime::currentDateTime().toMSecsSinceEpoch(), 0), "Komentarz testowy", chart);

    // Ustawienie widoku jako centralnego widżetu
    this->setCentralWidget(chartView);
    this->resize(800, 600);
}

void TestWindow::on_actionChart_Comment_triggered()
{
    testChartComment();
}


void TestWindow::on_actionustal_triggered()
{
    updateGeometry();
}

