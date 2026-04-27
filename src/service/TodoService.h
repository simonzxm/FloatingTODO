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
