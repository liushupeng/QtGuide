#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "snake.h"
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent* event);
    void paintEvent(QPaintEvent* event);

private:
    Snake m_snake;
    int   m_pictureSize = 100;
};
#endif   // MAINWINDOW_H
