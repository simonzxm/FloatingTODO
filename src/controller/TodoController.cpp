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
    QVector<TodoItem> stack = m_todoService.loadTaskTree();
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
