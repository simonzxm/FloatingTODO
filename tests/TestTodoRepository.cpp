#include <QtTest/QtTest>

class TestTodoRepository : public QObject
{
    Q_OBJECT

private slots:
    void sanity()
    {
        QVERIFY(true);
    }
};

QTEST_MAIN(TestTodoRepository)
#include "TestTodoRepository.moc"
