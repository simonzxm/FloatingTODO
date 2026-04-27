#pragma once

#include "controller/TodoController.h"

#include <QMainWindow>

class TodoListView;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(TodoController *controller, QWidget *parent = nullptr);

private:
    void openAddRootDialog();

    TodoController *m_controller = nullptr;
    TodoListView *m_listView = nullptr;
};
