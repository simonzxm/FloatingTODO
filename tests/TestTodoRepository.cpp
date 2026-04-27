#include <QtTest/QtTest>

#include "repository/TodoRepository.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QUuid>

class TestTodoRepository : public QObject
{
    Q_OBJECT

private slots:
    void init()
    {
        m_connectionName = QString("repo_test_%1").arg(QUuid::createUuid().toString(QUuid::Id128));
        m_repo = new TodoRepository(":memory:", m_connectionName);
        QVERIFY(m_repo->initialize());
    }

    void cleanup()
    {
        delete m_repo;
        m_repo = nullptr;
        QSqlDatabase::removeDatabase(m_connectionName);
    }

    void addAndFindTask()
    {
        TodoItem item;
        item.title = "写模型层";
        item.completed = false;

        const int id = m_repo->add(item);
        QVERIFY(id > 0);

        const auto loaded = m_repo->findById(id);
        QVERIFY(loaded.has_value());
        QCOMPARE(loaded->title, QString("写模型层"));
    }

    void schemaDoesNotContainLaunchColumns()
    {
        QSqlQuery query(QSqlDatabase::database(m_connectionName));
        QVERIFY(query.exec("PRAGMA table_info(todos)"));

        QStringList columnNames;
        while (query.next()) {
            columnNames.push_back(query.value("name").toString());
        }

        QVERIFY(!columnNames.contains("launch_type"));
        QVERIFY(!columnNames.contains("launch_target"));
        QVERIFY(!columnNames.contains("launch_display_name"));
    }

    void updateTask()
    {
        TodoItem item;
        item.title = "旧标题";
        const int id = m_repo->add(item);

        auto loaded = m_repo->findById(id).value();
        loaded.title = "新标题";
        loaded.completed = true;
        QVERIFY(m_repo->update(loaded));

        const auto updated = m_repo->findById(id);
        QVERIFY(updated.has_value());
        QCOMPARE(updated->title, QString("新标题"));
        QVERIFY(updated->completed);
    }

    void childrenOfReturnsDirectChildren()
    {
        TodoItem parent;
        parent.title = "父任务";
        const int parentId = m_repo->add(parent);

        TodoItem child;
        child.parentId = parentId;
        child.title = "子任务";
        const int childId = m_repo->add(child);

        TodoItem grandChild;
        grandChild.parentId = childId;
        grandChild.title = "孙任务";
        m_repo->add(grandChild);

        const auto children = m_repo->childrenOf(parentId);
        QCOMPARE(children.size(), 1);
        QCOMPARE(children.front().title, QString("子任务"));
    }

    void removeSubtreeDeletesDescendants()
    {
        TodoItem parent;
        parent.title = "父任务";
        const int parentId = m_repo->add(parent);

        TodoItem child;
        child.parentId = parentId;
        child.title = "子任务";
        const int childId = m_repo->add(child);

        QVERIFY(m_repo->removeSubtree(parentId));
        QVERIFY(!m_repo->findById(parentId).has_value());
        QVERIFY(!m_repo->findById(childId).has_value());
    }

private:
    QString m_connectionName;
    TodoRepository *m_repo = nullptr;
};

QTEST_MAIN(TestTodoRepository)
#include "TestTodoRepository.moc"
