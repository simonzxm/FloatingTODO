#include "view/MainWindow.h"

#include "view/TodoEditorDialog.h"
#include "view/TodoListView.h"

#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QStatusBar>
#include <QVBoxLayout>

MainWindow::MainWindow(TodoController *controller, QWidget *parent)
    : QMainWindow(parent)
    , m_controller(controller)
{
    setWindowTitle(QStringLiteral("FloatingTODO"));

    auto *central = new QWidget(this);
    auto *layout = new QVBoxLayout(central);

    auto *addButton = new QPushButton(QStringLiteral("新增顶层任务"), central);
    m_listView = new TodoListView(central);
    m_listView->setController(m_controller);

    auto *scrollArea = new QScrollArea(central);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(m_listView);

    layout->addWidget(addButton);
    layout->addWidget(scrollArea, 1);
    setCentralWidget(central);

    connect(addButton, &QPushButton::clicked, this, &MainWindow::openAddRootDialog);
    connect(m_controller, &TodoController::tasksChanged, m_listView, &TodoListView::setTasks);
    connect(m_controller, &TodoController::errorOccurred, this, [this](const QString &message) {
        QMessageBox::warning(this, QStringLiteral("FloatingTODO"), message);
    });
    connect(m_controller, &TodoController::infoOccurred, this, [this](const QString &message) {
        statusBar()->showMessage(message, 3000);
    });
}

void MainWindow::openAddRootDialog()
{
    if (!m_controller) {
        return;
    }
    TodoEditorDialog dialog(this);
    dialog.setWindowTitle(QStringLiteral("新增顶层任务"));
    if (dialog.exec() == QDialog::Accepted) {
        m_controller->addRootTask(dialog.title(), dialog.dueAt());
    }
}
