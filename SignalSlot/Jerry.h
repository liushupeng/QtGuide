#pragma once

#include <QObject>
#include <QDebug>

class Jerry : public QObject
{
    Q_OBJECT
public:
    Jerry(QObject *parent = nullptr) : QObject(parent)
    {
    }
public slots:
    void runAway()
    {
        qDebug() << "The Cat is coming, run away!!" ;
    }
};

