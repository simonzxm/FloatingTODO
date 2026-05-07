#pragma once

#include "controller/TodoController.h"

#include <QObject>
#include <QVariantList>

class WebTodoBridge : public QObject
{
    Q_OBJECT

public:
    explicit WebTodoBridge(TodoController *controller, QObject *parent = nullptr);

    Q_INVOKABLE QVariantList tasks() const;
    Q_INVOKABLE void addRootTask(const QString &title, const QString &dueAtIso = QString());
    Q_INVOKABLE void addChildTask(int parentId, const QString &title, const QString &dueAtIso = QString());
    Q_INVOKABLE void editTask(int id, const QString &title, const QString &dueAtIso = QString());
    Q_INVOKABLE void toggleTaskCompleted(int id);
    Q_INVOKABLE void deleteTask(int id);
    Q_INVOKABLE void reorderTasks(const QVariantList &orderedTasks);
    Q_INVOKABLE void replaceTasks(const QVariantList &tasks);

signals:
    void tasksChanged(const QVariantList &tasks);

private:
    QVariantList toVariantList(const QVector<TodoItem> &items) const;
    QVariantMap toVariantMap(const TodoItem &item) const;

    TodoController *m_controller = nullptr;
};
