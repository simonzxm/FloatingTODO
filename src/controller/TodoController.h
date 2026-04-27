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
