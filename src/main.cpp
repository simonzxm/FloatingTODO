#include "view/WebShellWindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    WebShellWindow window;
    window.show();

    return app.exec();
}
