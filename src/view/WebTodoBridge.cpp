#include "view/WebTodoBridge.h"

#include <QDateTime>
#include <QVariantMap>

namespace {
QDateTime parseIsoDateTime(const QString &value)
{
    if (value.trimmed().isEmpty()) {
        return {};
    }
    return QDateTime::fromString(value, Qt::ISODate);
}
}

WebTodoBridge::WebTodoBridge(TodoController *controller, QObject *parent)
    : QObject(parent)
    , m_controller(controller)
{
    if (!m_controller) {
        return;
    }

    connect(m_controller, &TodoController::tasksChanged, this, [this](const QVector<TodoItem> &items) {
        emit tasksChanged(toVariantList(items));
    });
}

QVariantList WebTodoBridge::tasks() const
{
    if (!m_controller) {
        return {};
    }

    QVariantList currentTasks;
    QMetaObject::Connection connection;
    connection = connect(m_controller, &TodoController::tasksChanged, this, [&](const QVector<TodoItem> &items) {
        currentTasks = toVariantList(items);
        disconnect(connection);
    });
    m_controller->refresh();
    return currentTasks;
}

void WebTodoBridge::addRootTask(const QString &title, const QString &dueAtIso)
{
    if (!m_controller) {
        return;
    }
    m_controller->addRootTask(title, parseIsoDateTime(dueAtIso));
}

void WebTodoBridge::addChildTask(int parentId, const QString &title, const QString &dueAtIso)
{
    if (!m_controller) {
        return;
    }
    m_controller->addChildTask(parentId, title, parseIsoDateTime(dueAtIso));
}

void WebTodoBridge::editTask(int id, const QString &title, const QString &dueAtIso)
{
    if (!m_controller) {
        return;
    }
    m_controller->editTask(id, title, parseIsoDateTime(dueAtIso));
}

void WebTodoBridge::toggleTaskCompleted(int id)
{
    if (!m_controller) {
        return;
    }
    m_controller->toggleTaskCompleted(id);
}

void WebTodoBridge::deleteTask(int id)
{
    if (!m_controller) {
        return;
    }
    m_controller->deleteTask(id);
}

void WebTodoBridge::reorderTasks(const QVariantList &orderedTasks)
{
    if (!m_controller) {
        return;
    }
    m_controller->reorderTasks(orderedTasks);
}

void WebTodoBridge::replaceTasks(const QVariantList &tasks)
{
    if (!m_controller) {
        return;
    }
    m_controller->replaceTasks(tasks);
}

QVariantList WebTodoBridge::toVariantList(const QVector<TodoItem> &items) const
{
    QVariantList result;
    result.reserve(items.size());
    for (const auto &item : items) {
        result.push_back(toVariantMap(item));
    }
    return result;
}

QVariantMap WebTodoBridge::toVariantMap(const TodoItem &item) const
{
    QVariantMap map;
    map.insert(QStringLiteral("id"), item.id);
    map.insert(QStringLiteral("parentId"), item.parentId);
    map.insert(QStringLiteral("title"), item.title);
    map.insert(QStringLiteral("dueAt"), item.dueAt.isValid() ? item.dueAt.toUTC().toString(Qt::ISODate) : QString());
    map.insert(QStringLiteral("completed"), item.completed);
    map.insert(QStringLiteral("sortOrder"), item.sortOrder);
    map.insert(QStringLiteral("createdAt"), item.createdAt.isValid() ? item.createdAt.toUTC().toString(Qt::ISODate) : QString());
    map.insert(QStringLiteral("updatedAt"), item.updatedAt.isValid() ? item.updatedAt.toUTC().toString(Qt::ISODate) : QString());
    map.insert(QStringLiteral("children"), toVariantList(item.children));
    return map;
}
