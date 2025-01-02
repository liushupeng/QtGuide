#include <QCoreApplication>
#include "Tom.h"
#include "Jerry.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    Tom tom;
    Jerry jerry;

    QObject::connect(&tom, &Tom::miao, &jerry, &Jerry::runAway);
    tom.miaow();

    return app.exec();
}

