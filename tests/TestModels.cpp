#include <QtTest/QtTest>

#include "model/LaunchAction.h"
#include "model/TodoItem.h"

class TestModels : public QObject
{
    Q_OBJECT

private slots:
    void launchActionDefaultsToNone()
    {
        LaunchAction action;
        QCOMPARE(action.type, LaunchActionType::None);
        QVERIFY(action.target.isEmpty());
        QVERIFY(action.displayName.isEmpty());
        QCOMPARE(action.buttonText(), QString("启动"));
    }

    void launchActionUsesDisplayNameWhenPresent()
    {
        LaunchAction action;
        action.displayName = "打开资料";
        QCOMPARE(action.buttonText(), QString("打开资料"));
    }

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
