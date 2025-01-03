#pragma once

#include <QDebug>
#include <QObject>

class Jerry : public QObject
{
    Q_OBJECT
public:
    Jerry(QObject* parent = nullptr)
        : QObject(parent)
    {}
public slots:
    void runAway() { qDebug() << "The Cat is coming, run away!!"; }
};
