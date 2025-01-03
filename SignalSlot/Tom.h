#pragma once

#include <QDebug>
#include <QObject>

class Tom : public QObject
{
    Q_OBJECT
public:
    Tom(QObject* parent = nullptr)
        : QObject(parent)
    {}
    void miaow()
    {
        qDebug() << "~~Miao~~";
        emit miao();
    }
signals:
    void miao();
};
