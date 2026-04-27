#include <QtTest/QtTest>

class TestTodoService : public QObject
{
    Q_OBJECT

private slots:
    void sanity()
    {
        QVERIFY(true);
    }
};

QTEST_MAIN(TestTodoService)
#include "TestTodoService.moc"
