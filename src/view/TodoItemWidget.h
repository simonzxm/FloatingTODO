#pragma once

#include "model/TodoItem.h"
#include "service/TodoService.h"

#include <QWidget>

class TodoItemWidget : public QWidget
{
    Q_OBJECT

public:
    TodoItemWidget(const TodoItem &item, const ChildStats &stats, int depth, QWidget *parent = nullptr);

signals:
    void toggleRequested(int id);
    void addChildRequested(int id);
    void editRequested(int id);
    void deleteRequested(int id);

private:
    int m_id = -1;
};
