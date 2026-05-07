#include <QtTest/QtTest>

#include "controller/TodoController.h"
#include "repository/TodoRepository.h"
#include "service/TodoService.h"
#include "view/WebTodoBridge.h"

#include <QSignalSpy>
#include <QSqlDatabase>
#include <QUuid>

class TestWebTodoBridge : public QObject
{
    Q_OBJECT

private slots:
    void init()
    {
        m_connectionName = QString("bridge_test_%1").arg(QUuid::createUuid().toString(QUuid::Id128));
        m_repo = new TodoRepository(":memory:", m_connectionName);
        QVERIFY(m_repo->initialize());
        m_service = new TodoService(*m_repo);
        m_controller = new TodoController(*m_service);
        m_bridge = new WebTodoBridge(m_controller);
    }

    void cleanup()
    {
        delete m_bridge;
        delete m_controller;
        delete m_service;
        delete m_repo;
        m_bridge = nullptr;
        m_controller = nullptr;
        m_service = nullptr;
        m_repo = nullptr;
        QSqlDatabase::removeDatabase(m_connectionName);
    }

    void exposesNestedTasksForJavaScript()
    {
        const int rootId = m_service->createTask(-1, "Root", QDateTime());
        const int childId = m_service->createTask(rootId, "Child", QDateTime());
        QVERIFY(m_service->toggleCompleted(childId));

        const QVariantList tasks = m_bridge->tasks();

        QCOMPARE(tasks.size(), 1);
        const QVariantMap root = tasks.first().toMap();
        QCOMPARE(root.value("id").toInt(), rootId);
        QCOMPARE(root.value("title").toString(), QString("Root"));
        QCOMPARE(root.value("completed").toBool(), false);

        const QVariantList children = root.value("children").toList();
        QCOMPARE(children.size(), 1);
        const QVariantMap child = children.first().toMap();
        QCOMPARE(child.value("id").toInt(), childId);
        QCOMPARE(child.value("title").toString(), QString("Child"));
        QCOMPARE(child.value("completed").toBool(), true);
    }

    void emitsTasksChangedWhenControllerRefreshes()
    {
        QSignalSpy spy(m_bridge, &WebTodoBridge::tasksChanged);

        m_bridge->addRootTask("New task");

        QCOMPARE(spy.count(), 1);
        const QVariantList tasks = spy.takeFirst().at(0).toList();
        QCOMPARE(tasks.size(), 1);
        QCOMPARE(tasks.first().toMap().value("title").toString(), QString("New task"));
    }

    void reordersTasksAndChangesParents()
    {
        const int rootA = m_service->createTask(-1, "Root A", QDateTime());
        const int rootB = m_service->createTask(-1, "Root B", QDateTime());
        const int rootC = m_service->createTask(-1, "Root C", QDateTime());

        QVariantList ordered;
        QVariantMap first;
        first.insert("id", rootC);
        first.insert("parentId", -1);
        ordered.push_back(first);
        QVariantMap second;
        second.insert("id", rootA);
        second.insert("parentId", -1);
        ordered.push_back(second);
        QVariantMap child;
        child.insert("id", rootB);
        child.insert("parentId", rootA);
        ordered.push_back(child);

        m_bridge->reorderTasks(ordered);

        const QVariantList tasks = m_bridge->tasks();
        QCOMPARE(tasks.size(), 2);
        QCOMPARE(tasks.at(0).toMap().value("id").toInt(), rootC);
        const QVariantMap parent = tasks.at(1).toMap();
        QCOMPARE(parent.value("id").toInt(), rootA);
        const QVariantList children = parent.value("children").toList();
        QCOMPARE(children.size(), 1);
        QCOMPARE(children.first().toMap().value("id").toInt(), rootB);
    }

    void replacesTasksFromFrontendSnapshot()
    {
        m_service->createTask(-1, "Old task", QDateTime());

        QVariantMap child;
        child.insert("id", -2);
        child.insert("title", "New child");
        child.insert("completed", true);
        child.insert("sortOrder", 0);
        child.insert("children", QVariantList());

        QVariantMap root;
        root.insert("id", -1);
        root.insert("title", "New root");
        root.insert("completed", false);
        root.insert("sortOrder", 0);
        root.insert("children", QVariantList{child});

        m_bridge->replaceTasks(QVariantList{root});

        const QVariantList tasks = m_bridge->tasks();
        QCOMPARE(tasks.size(), 1);
        const QVariantMap savedRoot = tasks.first().toMap();
        QCOMPARE(savedRoot.value("title").toString(), QString("New root"));
        const QVariantList children = savedRoot.value("children").toList();
        QCOMPARE(children.size(), 1);
        QCOMPARE(children.first().toMap().value("title").toString(), QString("New child"));
        QCOMPARE(children.first().toMap().value("completed").toBool(), true);
    }

private:
    QString m_connectionName;
    TodoRepository *m_repo = nullptr;
    TodoService *m_service = nullptr;
    TodoController *m_controller = nullptr;
    WebTodoBridge *m_bridge = nullptr;
};

QTEST_MAIN(TestWebTodoBridge)
#include "TestWebTodoBridge.moc"
