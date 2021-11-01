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

};

Test1::Test1()
{

}

Test1::~Test1()
{

}

void Test1::test_case1()
{

}

QTEST_APPLESS_MAIN(Test1)

#include "tst_test1.moc"
