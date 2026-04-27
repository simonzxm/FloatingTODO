#include "controller/TodoController.h"
#include "repository/TodoRepository.h"
#include "service/LaunchActionService.h"
#include "service/TodoService.h"
#include "view/MainWindow.h"

#include <QApplication>
#include <QDir>
#include <QMessageBox>
#include <QStandardPaths>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    const QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataDir);
    const QString dbPath = dataDir + "/floatingtodo.sqlite";

    TodoRepository repository(dbPath);
    if (!repository.initialize()) {
        QMessageBox::critical(nullptr, "FloatingTODO", "数据库初始化失败");
        return 1;
    }

    TodoService todoService(repository);
    LaunchActionService launchService;
    TodoController controller(todoService, launchService);

    MainWindow window(&controller);
    window.resize(900, 600);
    window.show();
    controller.refresh();

    return app.exec();
}
