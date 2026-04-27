#include <QtTest/QtTest>

#include "service/TodoService.h"

#include <QSqlDatabase>
#include <QUuid>

class TestTodoService : public QObject
{
    Q_OBJECT

private slots:
    void init()
    {
        m_connectionName = QString("service_test_%1").arg(QUuid::createUuid().toString(QUuid::Id128));
        m_repo = new TodoRepository(":memory:", m_connectionName);
        QVERIFY(m_repo->initialize());
        m_service = new TodoService(*m_repo);
    }

    void cleanup()
    {
        delete m_service;
        delete m_repo;
        m_service = nullptr;
        m_repo = nullptr;
        QSqlDatabase::removeDatabase(m_connectionName);
    }

    void createRootAndChildTask()
    {
        const int rootId = m_service->createTask(-1, "根任务", QDateTime(), LaunchAction());
        QVERIFY(rootId > 0);

        const int childId = m_service->createTask(rootId, "子任务", QDateTime(), LaunchAction());
        QVERIFY(childId > 0);

        const auto children = m_repo->childrenOf(rootId);
        QCOMPARE(children.size(), 1);
        QCOMPARE(children.front().title, QString("子任务"));
    }

    void toggleCompletedOnlyChangesCurrentTask()
    {
        const int id = m_service->createTask(-1, "任务", QDateTime(), LaunchAction());
        QVERIFY(m_service->toggleCompleted(id));
        QVERIFY(m_repo->findById(id)->completed);
        QVERIFY(m_service->toggleCompleted(id));
        QVERIFY(!m_repo->findById(id)->completed);
    }

    void childStatsCountsDirectChildrenOnly()
    {
        const int rootId = m_service->createTask(-1, "根任务", QDateTime(), LaunchAction());
        const int childA = m_service->createTask(rootId, "子任务 A", QDateTime(), LaunchAction());
        m_service->createTask(rootId, "子任务 B", QDateTime(), LaunchAction());
        const int grandChild = m_service->createTask(childA, "孙任务", QDateTime(), LaunchAction());

        QVERIFY(m_service->toggleCompleted(childA));
        QVERIFY(m_service->toggleCompleted(grandChild));

        const auto stats = m_service->childStats(rootId);
        QCOMPARE(stats.completed, 1);
        QCOMPARE(stats.total, 2);
    }

    void loadTaskTreeBuildsNestedChildren()
    {
        const int rootId = m_service->createTask(-1, "根任务", QDateTime(), LaunchAction());
        m_service->createTask(rootId, "子任务", QDateTime(), LaunchAction());

        const auto tree = m_service->loadTaskTree();
        QCOMPARE(tree.size(), 1);
        QCOMPARE(tree.front().children.size(), 1);
        QCOMPARE(tree.front().children.front().title, QString("子任务"));
    }

private:
    QString m_connectionName;
    TodoRepository *m_repo = nullptr;
    TodoService *m_service = nullptr;
};

QTEST_MAIN(TestTodoService)
#include "TestTodoService.moc"
