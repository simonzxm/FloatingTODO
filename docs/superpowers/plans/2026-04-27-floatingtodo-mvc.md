# FloatingTODO MVC Skeleton Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** 构建 FloatingTODO 第一阶段 MVC 骨架：SQLite 持久化、多级任务、启动动作、简陋 Qt Widgets 视图。

**Architecture:** 使用手写 MVC。`model/`、`repository/`、`service/` 组成广义 Model 层，`controller/` 是 View 的唯一业务入口，`view/` 只负责展示和收集用户操作。

**Tech Stack:** C++17、Qt6 Widgets、Qt6 Sql、Qt Test、CMake。

---

## 0. 当前约束

当前目录不是 Git 仓库，因此每个任务末尾只标记“可提交边界”。如果后续初始化 Git，可按任务边界提交。

通用验证命令：

```powershell
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

预期结果：

```text
Build files have been written to: .../build
... Built target todo_hello
... Built target test_models
... Built target test_todo_repository
... Built target test_todo_service
... Built target test_launch_action_service
100% tests passed
```

## 文件结构总览

需要创建或修改：

```text
CMakeLists.txt
src/main.cpp

src/model/LaunchAction.h
src/model/LaunchAction.cpp
src/model/TodoItem.h
src/model/TodoItem.cpp

src/repository/TodoRepository.h
src/repository/TodoRepository.cpp

src/service/TodoService.h
src/service/TodoService.cpp
src/service/LaunchActionService.h
src/service/LaunchActionService.cpp

src/controller/TodoController.h
src/controller/TodoController.cpp

src/view/MainWindow.h
src/view/MainWindow.cpp
src/view/TodoListView.h
src/view/TodoListView.cpp
src/view/TodoItemWidget.h
src/view/TodoItemWidget.cpp
src/view/TodoEditorDialog.h
src/view/TodoEditorDialog.cpp

tests/TestModels.cpp
tests/TestTodoRepository.cpp
tests/TestTodoService.cpp
tests/TestLaunchActionService.cpp
```

---

### Task 1: 项目结构与 CMake

**Files:**
- Modify: `CMakeLists.txt`
- Move/Create: `src/main.cpp`
- Create directories: `src/model`, `src/repository`, `src/service`, `src/controller`, `src/view`, `tests`

- [ ] **Step 1: 创建目录**

Run:

```powershell
New-Item -ItemType Directory -Force -Path src,src/model,src/repository,src/service,src/controller,src/view,tests
Move-Item -Path main.cpp -Destination src/main.cpp
```

Expected:

```text
src/main.cpp exists
main.cpp no longer exists at project root
```

- [ ] **Step 2: 替换 CMakeLists.txt**

Write `CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.16)

project(todo_hello LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Sql Test)

qt_standard_project_setup()
enable_testing()

set(FLOATINGTODO_CORE_SOURCES
    src/model/LaunchAction.h
    src/model/LaunchAction.cpp
    src/model/TodoItem.h
    src/model/TodoItem.cpp
    src/repository/TodoRepository.h
    src/repository/TodoRepository.cpp
    src/service/TodoService.h
    src/service/TodoService.cpp
    src/service/LaunchActionService.h
    src/service/LaunchActionService.cpp
    src/controller/TodoController.h
    src/controller/TodoController.cpp
)

set(FLOATINGTODO_VIEW_SOURCES
    src/view/MainWindow.h
    src/view/MainWindow.cpp
    src/view/TodoListView.h
    src/view/TodoListView.cpp
    src/view/TodoItemWidget.h
    src/view/TodoItemWidget.cpp
    src/view/TodoEditorDialog.h
    src/view/TodoEditorDialog.cpp
)

qt_add_library(floatingtodo_core STATIC
    ${FLOATINGTODO_CORE_SOURCES}
)

target_include_directories(floatingtodo_core PUBLIC src)
target_link_libraries(floatingtodo_core PUBLIC Qt6::Core Qt6::Gui Qt6::Sql)

qt_add_executable(todo_hello
    src/main.cpp
    ${FLOATINGTODO_VIEW_SOURCES}
)

target_link_libraries(todo_hello PRIVATE floatingtodo_core Qt6::Widgets)

set_target_properties(todo_hello PROPERTIES
    WIN32_EXECUTABLE ON
)

function(add_floatingtodo_test target source)
    qt_add_executable(${target}
        ${source}
    )
    target_link_libraries(${target} PRIVATE floatingtodo_core Qt6::Test Qt6::Widgets)
    add_test(NAME ${target} COMMAND ${target})
endfunction()

add_floatingtodo_test(test_models tests/TestModels.cpp)
add_floatingtodo_test(test_todo_repository tests/TestTodoRepository.cpp)
add_floatingtodo_test(test_todo_service tests/TestTodoService.cpp)
add_floatingtodo_test(test_launch_action_service tests/TestLaunchActionService.cpp)
```

- [ ] **Step 3: 放入临时空实现以验证构建结构**

Create each planned `.h/.cpp` with minimal valid definitions so CMake can configure. Example for `src/model/LaunchAction.h`:

```cpp
#pragma once
```

Example for `src/model/LaunchAction.cpp`:

```cpp
#include "model/LaunchAction.h"
```

Temporarily rewrite `src/main.cpp`:

```cpp
#include <QApplication>
#include <QLabel>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QLabel label("FloatingTODO MVC skeleton");
    label.show();
    return app.exec();
}
```

- [ ] **Step 4: 添加临时测试入口**

Create `tests/TestModels.cpp`:

```cpp
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
```

Create `tests/TestTodoRepository.cpp`:

```cpp
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
```

Create `tests/TestTodoService.cpp`:

```cpp
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
```

Create `tests/TestLaunchActionService.cpp`:

```cpp
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
```

- [ ] **Step 5: 验证构建**

Run:

```powershell
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

Expected:

```text
100% tests passed
```

可提交边界：

```text
build: create Qt MVC project structure
```

---

### Task 2: 实现基础模型 LaunchAction 与 TodoItem

**Files:**
- Modify: `src/model/LaunchAction.h`
- Modify: `src/model/LaunchAction.cpp`
- Modify: `src/model/TodoItem.h`
- Modify: `src/model/TodoItem.cpp`
- Modify: `tests/TestModels.cpp`

- [ ] **Step 1: 写模型测试**

Replace `tests/TestModels.cpp`:

```cpp
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
```

- [ ] **Step 2: 运行测试，确认失败**

Run:

```powershell
cmake --build build
ctest --test-dir build --output-on-failure
```

Expected: build fails because `LaunchActionType`, `LaunchAction`, and `TodoItem` are not defined.

- [ ] **Step 3: 实现 LaunchAction**

Write `src/model/LaunchAction.h`:

```cpp
#pragma once

#include <QString>

enum class LaunchActionType {
    None,
    Url,
    File,
    Folder,
    Application
};

struct LaunchAction {
    LaunchActionType type = LaunchActionType::None;
    QString target;
    QString displayName;

    QString buttonText() const;
};
```

Write `src/model/LaunchAction.cpp`:

```cpp
#include "model/LaunchAction.h"

QString LaunchAction::buttonText() const
{
    return displayName.trimmed().isEmpty() ? QStringLiteral("启动") : displayName;
}
```

- [ ] **Step 4: 实现 TodoItem**

Write `src/model/TodoItem.h`:

```cpp
#pragma once

#include "model/LaunchAction.h"

#include <QDateTime>
#include <QString>
#include <QVector>

struct TodoItem {
    int id = -1;
    int parentId = -1;
    QString title;
    QDateTime dueAt;
    bool completed = false;
    LaunchAction launchAction;
    QDateTime createdAt;
    QDateTime updatedAt;
    QVector<TodoItem> children;

    bool hasParent() const;
    bool hasChildren() const;
};
```

Write `src/model/TodoItem.cpp`:

```cpp
#include "model/TodoItem.h"

bool TodoItem::hasParent() const
{
    return parentId >= 0;
}

bool TodoItem::hasChildren() const
{
    return !children.isEmpty();
}
```

- [ ] **Step 5: 验证测试通过**

Run:

```powershell
cmake --build build
ctest --test-dir build --output-on-failure
```

Expected:

```text
100% tests passed
```

可提交边界：

```text
model: add todo item and launch action types
```

---

### Task 3: 实现 SQLite Repository

**Files:**
- Modify: `src/repository/TodoRepository.h`
- Modify: `src/repository/TodoRepository.cpp`
- Modify: `tests/TestTodoRepository.cpp`

- [ ] **Step 1: 写 Repository 测试**

Replace `tests/TestTodoRepository.cpp`:

```cpp
#include <QtTest/QtTest>

#include "repository/TodoRepository.h"

#include <QSqlDatabase>
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
        item.launchAction.type = LaunchActionType::Url;
        item.launchAction.target = "https://example.com";

        const int id = m_repo->add(item);
        QVERIFY(id > 0);

        const auto loaded = m_repo->findById(id);
        QVERIFY(loaded.has_value());
        QCOMPARE(loaded->title, QString("写模型层"));
        QCOMPARE(loaded->launchAction.type, LaunchActionType::Url);
        QCOMPARE(loaded->launchAction.target, QString("https://example.com"));
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
```

- [ ] **Step 2: 运行测试，确认失败**

Run:

```powershell
cmake --build build
ctest --test-dir build --output-on-failure
```

Expected: build fails because `TodoRepository` is not defined.

- [ ] **Step 3: 实现 Repository 接口**

Write `src/repository/TodoRepository.h`:

```cpp
#pragma once

#include "model/TodoItem.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>
#include <QVector>
#include <optional>

class TodoRepository
{
public:
    explicit TodoRepository(QString databasePath, QString connectionName = QString());
    ~TodoRepository();

    bool initialize();
    QVector<TodoItem> findAll() const;
    std::optional<TodoItem> findById(int id) const;
    int add(const TodoItem &item);
    bool update(const TodoItem &item);
    bool remove(int id);
    QVector<TodoItem> childrenOf(int parentId) const;
    bool removeSubtree(int id);

private:
    TodoItem itemFromQuery(const QSqlQuery &query) const;
    QString typeToString(LaunchActionType type) const;
    LaunchActionType typeFromString(const QString &value) const;

    QString m_connectionName;
    QSqlDatabase m_db;
};
```

- [ ] **Step 4: 实现 Repository 主要逻辑**

Write `src/repository/TodoRepository.cpp` with these required behaviors:

```cpp
#include "repository/TodoRepository.h"

#include <QDateTime>
#include <QSqlError>
#include <QSqlQuery>
#include <QUuid>
#include <QVariant>

TodoRepository::TodoRepository(QString databasePath, QString connectionName)
{
    m_connectionName = connectionName.isEmpty()
        ? QString("floatingtodo_%1").arg(QUuid::createUuid().toString(QUuid::Id128))
        : connectionName;
    m_db = QSqlDatabase::addDatabase("QSQLITE", m_connectionName);
    m_db.setDatabaseName(databasePath);
}

TodoRepository::~TodoRepository()
{
    if (m_db.isOpen()) {
        m_db.close();
    }
}

bool TodoRepository::initialize()
{
    if (!m_db.open()) {
        return false;
    }

    QSqlQuery query(m_db);
    return query.exec(
        "CREATE TABLE IF NOT EXISTS todos ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "parent_id INTEGER NULL,"
        "title TEXT NOT NULL,"
        "due_at TEXT NULL,"
        "completed INTEGER NOT NULL DEFAULT 0,"
        "launch_type TEXT NOT NULL DEFAULT 'none',"
        "launch_target TEXT,"
        "launch_display_name TEXT,"
        "created_at TEXT NOT NULL,"
        "updated_at TEXT NOT NULL"
        ")"
    );
}

QVector<TodoItem> TodoRepository::findAll() const
{
    QVector<TodoItem> items;
    QSqlQuery query(m_db);
    query.exec("SELECT * FROM todos ORDER BY id ASC");
    while (query.next()) {
        items.push_back(itemFromQuery(query));
    }
    return items;
}

std::optional<TodoItem> TodoRepository::findById(int id) const
{
    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM todos WHERE id = ?");
    query.addBindValue(id);
    if (!query.exec() || !query.next()) {
        return std::nullopt;
    }
    return itemFromQuery(query);
}

int TodoRepository::add(const TodoItem &item)
{
    const auto now = QDateTime::currentDateTimeUtc();
    QSqlQuery query(m_db);
    query.prepare(
        "INSERT INTO todos "
        "(parent_id, title, due_at, completed, launch_type, launch_target, launch_display_name, created_at, updated_at) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)"
    );
    query.addBindValue(item.parentId >= 0 ? QVariant(item.parentId) : QVariant(QVariant::Int));
    query.addBindValue(item.title);
    query.addBindValue(item.dueAt.isValid() ? item.dueAt.toUTC().toString(Qt::ISODate) : QVariant(QVariant::String));
    query.addBindValue(item.completed ? 1 : 0);
    query.addBindValue(typeToString(item.launchAction.type));
    query.addBindValue(item.launchAction.target);
    query.addBindValue(item.launchAction.displayName);
    query.addBindValue(now.toString(Qt::ISODate));
    query.addBindValue(now.toString(Qt::ISODate));
    if (!query.exec()) {
        return -1;
    }
    return query.lastInsertId().toInt();
}

bool TodoRepository::update(const TodoItem &item)
{
    QSqlQuery query(m_db);
    query.prepare(
        "UPDATE todos SET parent_id = ?, title = ?, due_at = ?, completed = ?, "
        "launch_type = ?, launch_target = ?, launch_display_name = ?, updated_at = ? WHERE id = ?"
    );
    query.addBindValue(item.parentId >= 0 ? QVariant(item.parentId) : QVariant(QVariant::Int));
    query.addBindValue(item.title);
    query.addBindValue(item.dueAt.isValid() ? item.dueAt.toUTC().toString(Qt::ISODate) : QVariant(QVariant::String));
    query.addBindValue(item.completed ? 1 : 0);
    query.addBindValue(typeToString(item.launchAction.type));
    query.addBindValue(item.launchAction.target);
    query.addBindValue(item.launchAction.displayName);
    query.addBindValue(QDateTime::currentDateTimeUtc().toString(Qt::ISODate));
    query.addBindValue(item.id);
    return query.exec();
}

bool TodoRepository::remove(int id)
{
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM todos WHERE id = ?");
    query.addBindValue(id);
    return query.exec();
}

QVector<TodoItem> TodoRepository::childrenOf(int parentId) const
{
    QVector<TodoItem> items;
    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM todos WHERE parent_id = ? ORDER BY id ASC");
    query.addBindValue(parentId);
    query.exec();
    while (query.next()) {
        items.push_back(itemFromQuery(query));
    }
    return items;
}

bool TodoRepository::removeSubtree(int id)
{
    for (const auto &child : childrenOf(id)) {
        if (!removeSubtree(child.id)) {
            return false;
        }
    }
    return remove(id);
}
```

Continue the same file with mapping helpers:

```cpp
TodoItem TodoRepository::itemFromQuery(const QSqlQuery &query) const
{
    TodoItem item;
    item.id = query.value("id").toInt();
    item.parentId = query.value("parent_id").isNull() ? -1 : query.value("parent_id").toInt();
    item.title = query.value("title").toString();
    item.dueAt = QDateTime::fromString(query.value("due_at").toString(), Qt::ISODate);
    item.completed = query.value("completed").toInt() != 0;
    item.launchAction.type = typeFromString(query.value("launch_type").toString());
    item.launchAction.target = query.value("launch_target").toString();
    item.launchAction.displayName = query.value("launch_display_name").toString();
    item.createdAt = QDateTime::fromString(query.value("created_at").toString(), Qt::ISODate);
    item.updatedAt = QDateTime::fromString(query.value("updated_at").toString(), Qt::ISODate);
    return item;
}

QString TodoRepository::typeToString(LaunchActionType type) const
{
    switch (type) {
    case LaunchActionType::Url: return "url";
    case LaunchActionType::File: return "file";
    case LaunchActionType::Folder: return "folder";
    case LaunchActionType::Application: return "application";
    case LaunchActionType::None: return "none";
    }
    return "none";
}

LaunchActionType TodoRepository::typeFromString(const QString &value) const
{
    if (value == "url") return LaunchActionType::Url;
    if (value == "file") return LaunchActionType::File;
    if (value == "folder") return LaunchActionType::Folder;
    if (value == "application") return LaunchActionType::Application;
    return LaunchActionType::None;
}
```

- [ ] **Step 5: 验证测试通过**

Run:

```powershell
cmake --build build
ctest --test-dir build --output-on-failure
```

Expected:

```text
100% tests passed
```

可提交边界：

```text
repository: persist todo items in sqlite
```

---

### Task 4: 实现 TodoService 业务规则

**Files:**
- Modify: `src/service/TodoService.h`
- Modify: `src/service/TodoService.cpp`
- Modify: `tests/TestTodoService.cpp`

- [ ] **Step 1: 写 Service 测试**

Replace `tests/TestTodoService.cpp`:

```cpp
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
```

- [ ] **Step 2: 运行测试，确认失败**

Run:

```powershell
cmake --build build
ctest --test-dir build --output-on-failure
```

Expected: build fails because `TodoService` and `ChildStats` are not defined.

- [ ] **Step 3: 实现 TodoService 接口**

Write `src/service/TodoService.h`:

```cpp
#pragma once

#include "repository/TodoRepository.h"

struct ChildStats {
    int completed = 0;
    int total = 0;
};

class TodoService
{
public:
    explicit TodoService(TodoRepository &repository);

    QVector<TodoItem> loadTaskTree() const;
    int createTask(int parentId, const QString &title, const QDateTime &dueAt, const LaunchAction &launchAction);
    bool updateTask(int id, const QString &title, const QDateTime &dueAt, const LaunchAction &launchAction);
    bool deleteTask(int id);
    bool toggleCompleted(int id);
    ChildStats childStats(int id) const;

private:
    QVector<TodoItem> attachChildren(const QVector<TodoItem> &allItems, int parentId) const;

    TodoRepository &m_repository;
};
```

- [ ] **Step 4: 实现 TodoService**

Write `src/service/TodoService.cpp`:

```cpp
#include "service/TodoService.h"

TodoService::TodoService(TodoRepository &repository)
    : m_repository(repository)
{
}

QVector<TodoItem> TodoService::loadTaskTree() const
{
    return attachChildren(m_repository.findAll(), -1);
}

int TodoService::createTask(int parentId, const QString &title, const QDateTime &dueAt, const LaunchAction &launchAction)
{
    const QString trimmedTitle = title.trimmed();
    if (trimmedTitle.isEmpty()) {
        return -1;
    }

    TodoItem item;
    item.parentId = parentId;
    item.title = trimmedTitle;
    item.dueAt = dueAt;
    item.completed = false;
    item.launchAction = launchAction;
    return m_repository.add(item);
}

bool TodoService::updateTask(int id, const QString &title, const QDateTime &dueAt, const LaunchAction &launchAction)
{
    const QString trimmedTitle = title.trimmed();
    if (trimmedTitle.isEmpty()) {
        return false;
    }

    auto item = m_repository.findById(id);
    if (!item.has_value()) {
        return false;
    }

    item->title = trimmedTitle;
    item->dueAt = dueAt;
    item->launchAction = launchAction;
    return m_repository.update(*item);
}

bool TodoService::deleteTask(int id)
{
    return m_repository.removeSubtree(id);
}

bool TodoService::toggleCompleted(int id)
{
    auto item = m_repository.findById(id);
    if (!item.has_value()) {
        return false;
    }

    item->completed = !item->completed;
    return m_repository.update(*item);
}

ChildStats TodoService::childStats(int id) const
{
    ChildStats stats;
    const auto children = m_repository.childrenOf(id);
    stats.total = children.size();
    for (const auto &child : children) {
        if (child.completed) {
            ++stats.completed;
        }
    }
    return stats;
}

QVector<TodoItem> TodoService::attachChildren(const QVector<TodoItem> &allItems, int parentId) const
{
    QVector<TodoItem> result;
    for (const auto &item : allItems) {
        if (item.parentId == parentId) {
            TodoItem copy = item;
            copy.children = attachChildren(allItems, item.id);
            result.push_back(copy);
        }
    }
    return result;
}
```

- [ ] **Step 5: 验证测试通过**

Run:

```powershell
cmake --build build
ctest --test-dir build --output-on-failure
```

Expected:

```text
100% tests passed
```

可提交边界：

```text
service: add todo tree business rules
```

---

### Task 5: 实现 LaunchActionService

**Files:**
- Modify: `src/service/LaunchActionService.h`
- Modify: `src/service/LaunchActionService.cpp`
- Modify: `tests/TestLaunchActionService.cpp`

- [ ] **Step 1: 写启动动作测试**

Replace `tests/TestLaunchActionService.cpp`:

```cpp
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
```

- [ ] **Step 2: 运行测试，确认失败**

Run:

```powershell
cmake --build build
ctest --test-dir build --output-on-failure
```

Expected: build fails because `LaunchActionService` and `LaunchResult` are not defined.

- [ ] **Step 3: 实现接口**

Write `src/service/LaunchActionService.h`:

```cpp
#pragma once

#include "model/LaunchAction.h"

#include <QString>

struct LaunchResult {
    bool success = false;
    QString message;
};

class LaunchActionService
{
public:
    LaunchResult launch(const LaunchAction &action) const;

private:
    LaunchResult openWithDesktopServices(const QString &target) const;
};
```

- [ ] **Step 4: 实现启动逻辑**

Write `src/service/LaunchActionService.cpp`:

```cpp
#include "service/LaunchActionService.h"

#include <QDesktopServices>
#include <QProcess>
#include <QUrl>

LaunchResult LaunchActionService::launch(const LaunchAction &action) const
{
    if (action.type == LaunchActionType::None) {
        return {false, QStringLiteral("该任务没有设置启动动作")};
    }

    const QString target = action.target.trimmed();
    if (target.isEmpty()) {
        return {false, QStringLiteral("启动目标为空")};
    }

    if (action.type == LaunchActionType::Application) {
        const bool started = QProcess::startDetached(target);
        return started
            ? LaunchResult{true, QStringLiteral("已启动应用程序")}
            : LaunchResult{false, QStringLiteral("无法启动应用程序")};
    }

    return openWithDesktopServices(target);
}

LaunchResult LaunchActionService::openWithDesktopServices(const QString &target) const
{
    QUrl url = QUrl::fromUserInput(target);
    const bool opened = QDesktopServices::openUrl(url);
    return opened
        ? LaunchResult{true, QStringLiteral("已打开启动目标")}
        : LaunchResult{false, QStringLiteral("无法打开启动目标")};
}
```

- [ ] **Step 5: 验证测试通过**

Run:

```powershell
cmake --build build
ctest --test-dir build --output-on-failure
```

Expected:

```text
100% tests passed
```

可提交边界：

```text
service: add launch action execution
```

---

### Task 6: 实现 TodoController

**Files:**
- Modify: `src/controller/TodoController.h`
- Modify: `src/controller/TodoController.cpp`

- [ ] **Step 1: 实现 Controller 接口**

Write `src/controller/TodoController.h`:

```cpp
#pragma once

#include "service/LaunchActionService.h"
#include "service/TodoService.h"

#include <QObject>

class TodoController : public QObject
{
    Q_OBJECT

public:
    TodoController(TodoService &todoService, LaunchActionService &launchService, QObject *parent = nullptr);

    void refresh();
    void addRootTask(const QString &title, const QDateTime &dueAt, const LaunchAction &launchAction);
    void addChildTask(int parentId, const QString &title, const QDateTime &dueAt, const LaunchAction &launchAction);
    void editTask(int id, const QString &title, const QDateTime &dueAt, const LaunchAction &launchAction);
    void deleteTask(int id);
    void toggleTaskCompleted(int id);
    void launchTask(int id);
    ChildStats childStats(int id) const;

signals:
    void tasksChanged(const QVector<TodoItem> &tasks);
    void errorOccurred(const QString &message);
    void infoOccurred(const QString &message);

private:
    TodoService &m_todoService;
    LaunchActionService &m_launchService;
};
```

- [ ] **Step 2: 实现 Controller**

Write `src/controller/TodoController.cpp`:

```cpp
#include "controller/TodoController.h"

TodoController::TodoController(TodoService &todoService, LaunchActionService &launchService, QObject *parent)
    : QObject(parent)
    , m_todoService(todoService)
    , m_launchService(launchService)
{
}

void TodoController::refresh()
{
    emit tasksChanged(m_todoService.loadTaskTree());
}

void TodoController::addRootTask(const QString &title, const QDateTime &dueAt, const LaunchAction &launchAction)
{
    const int id = m_todoService.createTask(-1, title, dueAt, launchAction);
    if (id < 0) {
        emit errorOccurred(QStringLiteral("新增任务失败：标题不能为空或数据库写入失败"));
        return;
    }
    refresh();
}

void TodoController::addChildTask(int parentId, const QString &title, const QDateTime &dueAt, const LaunchAction &launchAction)
{
    const int id = m_todoService.createTask(parentId, title, dueAt, launchAction);
    if (id < 0) {
        emit errorOccurred(QStringLiteral("新增子任务失败：标题不能为空或数据库写入失败"));
        return;
    }
    refresh();
}

void TodoController::editTask(int id, const QString &title, const QDateTime &dueAt, const LaunchAction &launchAction)
{
    if (!m_todoService.updateTask(id, title, dueAt, launchAction)) {
        emit errorOccurred(QStringLiteral("编辑任务失败"));
        return;
    }
    refresh();
}

void TodoController::deleteTask(int id)
{
    if (!m_todoService.deleteTask(id)) {
        emit errorOccurred(QStringLiteral("删除任务失败"));
        return;
    }
    refresh();
}

void TodoController::toggleTaskCompleted(int id)
{
    if (!m_todoService.toggleCompleted(id)) {
        emit errorOccurred(QStringLiteral("切换任务完成状态失败"));
        return;
    }
    refresh();
}

void TodoController::launchTask(int id)
{
    const auto tasks = m_todoService.loadTaskTree();
    QVector<TodoItem> stack = tasks;
    while (!stack.isEmpty()) {
        const TodoItem item = stack.takeLast();
        if (item.id == id) {
            const auto result = m_launchService.launch(item.launchAction);
            if (result.success) {
                emit infoOccurred(result.message);
            } else {
                emit errorOccurred(result.message);
            }
            return;
        }
        for (const auto &child : item.children) {
            stack.push_back(child);
        }
    }
    emit errorOccurred(QStringLiteral("找不到任务"));
}

ChildStats TodoController::childStats(int id) const
{
    return m_todoService.childStats(id);
}
```

- [ ] **Step 3: 验证构建**

Run:

```powershell
cmake --build build
ctest --test-dir build --output-on-failure
```

Expected:

```text
100% tests passed
```

可提交边界：

```text
controller: expose todo operations to views
```

---

### Task 7: 实现简陋 Qt Widgets View

**Files:**
- Modify: `src/view/MainWindow.h`
- Modify: `src/view/MainWindow.cpp`
- Modify: `src/view/TodoListView.h`
- Modify: `src/view/TodoListView.cpp`
- Modify: `src/view/TodoItemWidget.h`
- Modify: `src/view/TodoItemWidget.cpp`
- Modify: `src/view/TodoEditorDialog.h`
- Modify: `src/view/TodoEditorDialog.cpp`
- Modify: `src/main.cpp`

- [ ] **Step 1: 实现 TodoEditorDialog**

`TodoEditorDialog` must expose:

```cpp
QString title() const;
QDateTime dueAt() const;
LaunchAction launchAction() const;
```

The dialog contains:

```text
QLineEdit titleEdit
QDateTimeEdit dueEdit
QComboBox launchTypeCombo
QLineEdit launchTargetEdit
QLineEdit launchDisplayNameEdit
QDialogButtonBox Ok/Cancel
```

Validation rule:

```text
Ok clicked with empty title -> QMessageBox::warning and dialog stays open
```

- [ ] **Step 2: 实现 TodoItemWidget**

`TodoItemWidget` constructor:

```cpp
TodoItemWidget(const TodoItem &item, const ChildStats &stats, int depth, QWidget *parent = nullptr);
```

Signals:

```cpp
void toggleRequested(int id);
void launchRequested(int id);
void addChildRequested(int id);
void editRequested(int id);
void deleteRequested(int id);
```

Display rules:

```text
depth controls left margin: depth * 24 px
completed true displays "✓"
completed false displays "□"
dueAt invalid displays "无截止时间"
stats.total > 0 displays "子任务：completed/total"
```

- [ ] **Step 3: 实现 TodoListView**

`TodoListView` public API:

```cpp
void setController(TodoController *controller);
void setTasks(const QVector<TodoItem> &tasks);
```

Rendering rule:

```text
clear layout
for each root task, recursively add TodoItemWidget
child widgets are rendered immediately below parent with depth + 1
```

When widget signals fire, call matching Controller method. For add/edit, open `TodoEditorDialog`.

- [ ] **Step 4: 实现 MainWindow**

`MainWindow` responsibilities:

```text
create Add Root Task button
contain TodoListView inside QScrollArea
connect controller tasksChanged to list setTasks
connect errorOccurred to QMessageBox::warning
connect infoOccurred to statusBar()->showMessage
```

- [ ] **Step 5: 实现 main.cpp**

Write `src/main.cpp`:

```cpp
#include "controller/TodoController.h"
#include "repository/TodoRepository.h"
#include "service/LaunchActionService.h"
#include "service/TodoService.h"
#include "view/MainWindow.h"

#include <QApplication>
#include <QDir>
#include <QMessageBox>
#include <QStandardPaths>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    const QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataDir);
    const QString dbPath = dataDir + "/floatingtodo.sqlite";

    TodoRepository repository(dbPath);
    if (!repository.initialize()) {
        QMessageBox::critical(nullptr, "FloatingTODO", "数据库初始化失败");
        return 1;
    }

    TodoService todoService(repository);
    LaunchActionService launchService;
    TodoController controller(todoService, launchService);

    MainWindow window(&controller);
    window.resize(900, 600);
    window.show();
    controller.refresh();

    return app.exec();
}
```

- [ ] **Step 6: 验证构建和手动运行**

Run:

```powershell
cmake --build build
ctest --test-dir build --output-on-failure
.\build\todo_hello.exe
```

Manual expected:

```text
App opens.
Can add a root task.
Can add child and grandchild tasks.
Tasks appear with increasing indentation.
Can toggle complete/incomplete.
Parent with children displays direct child completion count.
Can edit and delete tasks.
Launch without action shows an error.
```

可提交边界：

```text
view: add simple recursive widgets interface
```

---

### Task 8: 最终集成检查与文档同步

**Files:**
- Review: `docs/superpowers/specs/2026-04-27-floatingtodo-mvc-design.md`
- Modify: `notes/FloatingTODO 开发（MVC 大作业）.md`

- [ ] **Step 1: 全量验证**

Run:

```powershell
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
.\build\todo_hello.exe
```

Expected:

```text
All tests pass.
Application starts.
Manual MVC flow works.
```

- [ ] **Step 2: 对照规格检查**

Check these requirements manually:

```text
SQLite persistence exists.
Task supports parentId.
UI recursively displays all levels by indentation.
Task has completed/incomplete only.
Parent with direct children displays completed/total direct children.
Parent completion is manually toggled.
Deleting parent deletes descendants.
Launch action supports None/Url/File/Folder/Application in model and service.
View only calls Controller.
Controller calls Service.
Repository owns SQLite access.
```

- [ ] **Step 3: 更新开发笔记**

Append to `notes/FloatingTODO 开发（MVC 大作业）.md`:

```markdown
## MVC 第一阶段实现记录

第一阶段采用手写 MVC：

- Model 层：`TodoItem`、`LaunchAction`、`TodoRepository`、`TodoService`、`LaunchActionService`
- Controller 层：`TodoController`
- View 层：简陋 Qt Widgets，包括 `MainWindow`、`TodoListView`、`TodoItemWidget`、`TodoEditorDialog`

当前版本优先验证架构和功能闭环，后续再重写 View 层以实现更好的桌面组件体验和动画。
```

可提交边界：

```text
docs: record mvc skeleton implementation
```

---

## 自检清单

规格覆盖：

```text
模型层：Task 2, Task 3, Task 4, Task 5
SQLite：Task 3
多级任务：Task 4, Task 7
完成/未完成：Task 2, Task 4, Task 7
直接子任务统计：Task 4, Task 7
启动动作：Task 2, Task 5, Task 7
Controller 边界：Task 6
简陋 View：Task 7
最终手动验证：Task 8
```

类型一致性：

```text
TodoItem.id: int
TodoItem.parentId: int, -1 means no parent
LaunchActionType: None, Url, File, Folder, Application
ChildStats: completed, total
Controller signal: tasksChanged(QVector<TodoItem>)
```

范围控制：

```text
本计划不实现拖拽排序、搜索筛选、提醒、重复任务、动画、主题、云同步、账户系统。
```
