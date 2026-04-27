#include <QtTest/QtTest>

class TestModels : public QObject
{
    Q_OBJECT

private slots:
    void sanity()
    {
        QVERIFY(true);
    }
};

QTEST_MAIN(TestModels)
#include "TestModels.moc"
