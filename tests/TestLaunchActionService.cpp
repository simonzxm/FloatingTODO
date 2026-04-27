#include <QtTest/QtTest>

class TestLaunchActionService : public QObject
{
    Q_OBJECT

private slots:
    void sanity()
    {
        QVERIFY(true);
    }
};

QTEST_MAIN(TestLaunchActionService)
#include "TestLaunchActionService.moc"
