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
