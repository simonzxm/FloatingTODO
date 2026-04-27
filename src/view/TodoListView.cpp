#include "view/TodoListView.h"

#include "view/TodoEditorDialog.h"
#include "view/TodoItemWidget.h"

#include <QMessageBox>
#include <QVBoxLayout>

TodoListView::TodoListView(QWidget *parent)
    : QWidget(parent)
    , m_layout(new QVBoxLayout(this))
{
    m_layout->setAlignment(Qt::AlignTop);
    m_layout->setSpacing(2);
}

void TodoListView::setController(TodoController *controller)
{
    m_controller = controller;
}

void TodoListView::setTasks(const QVector<TodoItem> &tasks)
{
    m_tasks = tasks;
    clearItems();
    for (const auto &task : m_tasks) {
        renderItem(task, 0);
    }
    m_layout->addStretch(1);
}

void TodoListView::clearItems()
{
    while (QLayoutItem *item = m_layout->takeAt(0)) {
        delete item->widget();
        delete item;
    }
}

void TodoListView::renderItem(const TodoItem &item, int depth)
{
    auto *widget = new TodoItemWidget(item, statsFor(item), depth, this);
    m_layout->addWidget(widget);

    connect(widget, &TodoItemWidget::toggleRequested, this, [this](int id) {
        if (m_controller) {
            m_controller->toggleTaskCompleted(id);
        }
    });
    connect(widget, &TodoItemWidget::addChildRequested, this, &TodoListView::openAddChildDialog);
    connect(widget, &TodoItemWidget::editRequested, this, &TodoListView::openEditDialog);
    connect(widget, &TodoItemWidget::deleteRequested, this, [this](int id) {
        if (!m_controller) {
            return;
        }
        const auto result = QMessageBox::question(
            this,
            QStringLiteral("删除任务"),
            QStringLiteral("确定删除该任务及其所有子任务吗？")
        );
        if (result == QMessageBox::Yes) {
            m_controller->deleteTask(id);
        }
    });

    for (const auto &child : item.children) {
        renderItem(child, depth + 1);
    }
}

const TodoItem *TodoListView::findTask(int id) const
{
    return findTaskIn(m_tasks, id);
}

const TodoItem *TodoListView::findTaskIn(const QVector<TodoItem> &items, int id) const
{
    for (const auto &item : items) {
        if (item.id == id) {
            return &item;
        }
        if (const TodoItem *found = findTaskIn(item.children, id)) {
            return found;
        }
    }
    return nullptr;
}

ChildStats TodoListView::statsFor(const TodoItem &item) const
{
    ChildStats stats;
    stats.total = item.children.size();
    for (const auto &child : item.children) {
        if (child.completed) {
            ++stats.completed;
        }
    }
    return stats;
}

void TodoListView::openAddChildDialog(int parentId)
{
    if (!m_controller) {
        return;
    }
    TodoEditorDialog dialog(this);
    dialog.setWindowTitle(QStringLiteral("新增子任务"));
    if (dialog.exec() == QDialog::Accepted) {
        m_controller->addChildTask(parentId, dialog.title(), dialog.dueAt());
    }
}

void TodoListView::openEditDialog(int id)
{
    if (!m_controller) {
        return;
    }
    const TodoItem *task = findTask(id);
    if (!task) {
        return;
    }

    TodoEditorDialog dialog(*task, this);
    dialog.setWindowTitle(QStringLiteral("编辑任务"));
    if (dialog.exec() == QDialog::Accepted) {
        m_controller->editTask(id, dialog.title(), dialog.dueAt());
    }
}
