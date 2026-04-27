#pragma once

#include "service/TodoService.h"

#include <QObject>

class TodoController : public QObject
{
    Q_OBJECT

public:
    explicit TodoController(TodoService &todoService, QObject *parent = nullptr);

    void refresh();
    void addRootTask(const QString &title, const QDateTime &dueAt);
    void addChildTask(int parentId, const QString &title, const QDateTime &dueAt);
    void editTask(int id, const QString &title, const QDateTime &dueAt);
    void deleteTask(int id);
    void toggleTaskCompleted(int id);
    ChildStats childStats(int id) const;

signals:
    void tasksChanged(const QVector<TodoItem> &tasks);
    void errorOccurred(const QString &message);
    void infoOccurred(const QString &message);

private:
    TodoService &m_todoService;
};
