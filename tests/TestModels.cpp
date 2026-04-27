#include <QtTest/QtTest>

#include "model/TodoItem.h"

class TestModels : public QObject
{
    Q_OBJECT

private slots:
    void todoItemDefaultsToIncompleteRootTask()
    {
        TodoItem item;
        QCOMPARE(item.id, -1);
        QCOMPARE(item.parentId, -1);
        QVERIFY(item.title.isEmpty());
        QVERIFY(!item.completed);
        QVERIFY(!item.hasParent());
    }

    void todoItemDetectsChildTask()
    {
        TodoItem item;
        item.parentId = 42;
        QVERIFY(item.hasParent());
    }
};

QTEST_MAIN(TestModels)
#include "TestModels.moc"
