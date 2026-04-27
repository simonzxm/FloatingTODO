#include "controller/TodoController.h"

TodoController::TodoController(TodoService &todoService, QObject *parent)
    : QObject(parent)
    , m_todoService(todoService)
{
}

void TodoController::refresh()
{
    emit tasksChanged(m_todoService.loadTaskTree());
}

void TodoController::addRootTask(const QString &title, const QDateTime &dueAt)
{
    const int id = m_todoService.createTask(-1, title, dueAt);
    if (id < 0) {
        emit errorOccurred(QStringLiteral("新增任务失败：标题不能为空或数据库写入失败"));
        return;
    }
    refresh();
}

void TodoController::addChildTask(int parentId, const QString &title, const QDateTime &dueAt)
{
    const int id = m_todoService.createTask(parentId, title, dueAt);
    if (id < 0) {
        emit errorOccurred(QStringLiteral("新增子任务失败：标题不能为空或数据库写入失败"));
        return;
    }
    refresh();
}

void TodoController::editTask(int id, const QString &title, const QDateTime &dueAt)
{
    if (!m_todoService.updateTask(id, title, dueAt)) {
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

ChildStats TodoController::childStats(int id) const
{
    return m_todoService.childStats(id);
}
