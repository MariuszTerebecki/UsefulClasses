#include "testwindow.h"
#include "ui_testwindow.h"

#include <QFileDialog>
#include <QMessageBox>

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

