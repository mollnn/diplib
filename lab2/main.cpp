#include "widget.h"

#include <QApplication>

#include <immintrin.h>
#include <avx2intrin.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();
    return a.exec();
}

//#include "Tester.h"

//QTEST_APPLESS_MAIN(Test1)
