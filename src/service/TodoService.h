#pragma once

#include "repository/TodoRepository.h"

#include <QVariantList>

struct ChildStats {
    int completed = 0;
    int total = 0;
};

class TodoService
{
public:
    explicit TodoService(TodoRepository &repository);

    QVector<TodoItem> loadTaskTree() const;
    int createTask(int parentId, const QString &title, const QDateTime &dueAt);
    bool updateTask(int id, const QString &title, const QDateTime &dueAt);
    bool deleteTask(int id);
    bool toggleCompleted(int id);
    bool reorderTask(int id, int parentId, int sortOrder);
    bool reorderTasks(const QVariantList &orderedTasks);
    bool replaceTasks(const QVariantList &tasks);
    ChildStats childStats(int id) const;

private:
    bool insertSnapshotItems(const QVariantList &items, int parentId);
    QVector<TodoItem> attachChildren(const QVector<TodoItem> &allItems, int parentId) const;

    TodoRepository &m_repository;
};
