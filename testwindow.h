#ifndef TESTWINDOW_H
#define TESTWINDOW_H

#include "mtqss.h"
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class TestWindow;
}
QT_END_NAMESPACE

class TestWindow : public QMainWindow
{
    Q_OBJECT

public:
    TestWindow(QWidget *parent = nullptr);
    ~TestWindow();

private slots:
    void on_actionLoad_QSS_triggered();
    void on_actionSaveQSS_triggered();

    void on_actionChart_Comment_triggered();

    void on_actionustal_triggered();

private:
    Ui::TestWindow *ui;
    MTQss *mtQss;

    void testChartComment();
};
#endif // TESTWINDOW_H
