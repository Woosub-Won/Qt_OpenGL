#include <QCoreApplication>
#include "example_test.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    example_test(argc, argv);

    return a.exec();
}
