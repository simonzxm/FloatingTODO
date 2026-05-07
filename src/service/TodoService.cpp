#include "service/TodoService.h"

#include <QVariantMap>

TodoService::TodoService(TodoRepository &repository)
    : m_repository(repository)
{
}

QVector<TodoItem> TodoService::loadTaskTree() const
{
    return attachChildren(m_repository.findAll(), -1);
}

int TodoService::createTask(int parentId, const QString &title, const QDateTime &dueAt)
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
    return m_repository.add(item);
}

bool TodoService::updateTask(int id, const QString &title, const QDateTime &dueAt)
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

bool TodoService::reorderTask(int id, int parentId, int sortOrder)
{
    if (id < 0 || sortOrder < 0 || id == parentId || !m_repository.findById(id).has_value()) {
        return false;
    }
    if (parentId >= 0 && !m_repository.findById(parentId).has_value()) {
        return false;
    }
    return m_repository.updateParentAndOrder(id, parentId, sortOrder);
}

bool TodoService::reorderTasks(const QVariantList &orderedTasks)
{
    for (int index = 0; index < orderedTasks.size(); ++index) {
        const QVariantMap item = orderedTasks.at(index).toMap();
        if (!reorderTask(item.value(QStringLiteral("id")).toInt(),
                         item.value(QStringLiteral("parentId"), -1).toInt(),
                         index)) {
            return false;
        }
    }
    return true;
}

bool TodoService::replaceTasks(const QVariantList &tasks)
{
    if (!m_repository.removeAll()) {
        return false;
    }
    return insertSnapshotItems(tasks, -1);
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

bool TodoService::insertSnapshotItems(const QVariantList &items, int parentId)
{
    for (int index = 0; index < items.size(); ++index) {
        const QVariantMap snapshot = items.at(index).toMap();
        const QString title = snapshot.value(QStringLiteral("title")).toString().trimmed();
        if (title.isEmpty()) {
            continue;
        }

        TodoItem item;
        item.parentId = parentId;
        item.title = title;
        item.completed = snapshot.value(QStringLiteral("completed")).toBool();
        item.sortOrder = snapshot.value(QStringLiteral("sortOrder"), index).toInt();
        const int newId = m_repository.add(item);
        if (newId < 0) {
            return false;
        }

        if (!insertSnapshotItems(snapshot.value(QStringLiteral("children")).toList(), newId)) {
            return false;
        }
    }
    return true;
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
