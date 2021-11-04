#include "widget.h"

#include <QApplication>

#define _AUTOTEST_FLAG

#ifndef _AUTOTEST_FLAG

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.showMaximized();
    return a.exec();
}

#else

#include "Tester.h"

QTEST_APPLESS_MAIN(Test1)

#endif
