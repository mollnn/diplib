#ifndef TESTER_H
#define TESTER_H

#include <QtTest>

// add necessary includes here

class Test1 : public QObject
{
    Q_OBJECT

public:
    Test1();
    ~Test1();

private slots:
    void test_case1();
    void test_case2();
    void test_case3();

//    void test_case4();
};



#endif // TESTER_H
