#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <cassert>
#include <QKeyEvent>
#include <QPainter>
#include <QDebug>
#include <QMessageBox>
#include <QFileInfo>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //(4) your code. load data from data.txt file by Snake's member function LoadPlayDataFromFile
    QFileInfo srcInfo(__FILE__);
    bool  load_result = m_snake.LoadPlayDataFromFile(srcInfo.path().toStdString() + "/../resource/data.txt");
    assert(load_result);

    //(5) your code.  set window title with "Snake Game"
    //https://zhuanlan.zhihu.com/p/669799427
    setWindowTitle(QApplication::translate("snake", "Snake Game"));

    //(6) your code. set window size by the col number and row number in Snake object.
    //every pixmap size is the same : width * height = m_pictureSize * m_pictureSize
    //use setFixedSize()
    setFixedSize(m_snake.GetCol() * m_pictureSize, m_snake.GetRow() * m_pictureSize);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    qDebug()<<"User pressed key "<<event->key();
    bool gameover = false;

    //(3) your code. the player enter a key
    //, you have to update the game by call member function Play in Snake.
    if (event->key() == Qt::Key_Up) {
        gameover = m_snake.Play('W');
    } else if (event->key() == Qt::Key_Down) {
        //(3) your code.
        gameover = m_snake.Play('S');
    } else if (event->key() == Qt::Key_Left) {
        //(3) your code.
        gameover = m_snake.Play('A');
    } else if (event->key() == Qt::Key_Right) {
        //(3) your code.
        gameover = m_snake.Play('D');
    }
    update();//this will tell Qt to call paintEvent.
    if(gameover)
    {
        //(2) your code. use QMessageBox to tell the player the game is over.
        // https://zhuanlan.zhihu.com/p/671461612
        QMessageBox messageBox;
        messageBox.setWindowTitle(tr("Snake"));
        messageBox.setText(tr("Game is over!!"));
        messageBox.setStandardButtons(QMessageBox::Yes);
        messageBox.exec();
    }
}


void MainWindow::paintEvent(QPaintEvent *event)
{
    assert(event);
    QPainter painter(this);
    //(1) your code. use Snake PrintMatrix to drew the game
    m_snake.PrintMatrix(painter, m_pictureSize);
}

