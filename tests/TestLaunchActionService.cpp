#include <QtTest/QtTest>

#include "service/LaunchActionService.h"

class TestLaunchActionService : public QObject
{
    Q_OBJECT

private slots:
    void noneActionFailsWithMessage()
    {
        LaunchActionService service;
        LaunchAction action;
        const auto result = service.launch(action);
        QVERIFY(!result.success);
        QVERIFY(result.message.contains("没有设置"));
    }

    void emptyTargetFails()
    {
        LaunchActionService service;
        LaunchAction action;
        action.type = LaunchActionType::Url;
        const auto result = service.launch(action);
        QVERIFY(!result.success);
        QVERIFY(result.message.contains("为空"));
    }
};

QTEST_MAIN(TestLaunchActionService)
#include "TestLaunchActionService.moc"
