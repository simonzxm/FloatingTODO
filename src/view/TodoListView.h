#pragma once

#include "controller/TodoController.h"

#include <QWidget>

class QVBoxLayout;

class TodoListView : public QWidget
{
    Q_OBJECT

public:
    explicit TodoListView(QWidget *parent = nullptr);

    void setController(TodoController *controller);
    void setTasks(const QVector<TodoItem> &tasks);

private:
    void clearItems();
    void renderItem(const TodoItem &item, int depth);
    const TodoItem *findTask(int id) const;
    const TodoItem *findTaskIn(const QVector<TodoItem> &items, int id) const;
    ChildStats statsFor(const TodoItem &item) const;
    void openAddChildDialog(int parentId);
    void openEditDialog(int id);

    TodoController *m_controller = nullptr;
    QVector<TodoItem> m_tasks;
    QVBoxLayout *m_layout = nullptr;
};
