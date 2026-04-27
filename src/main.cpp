#include <QApplication>
#include <QLabel>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QLabel label("FloatingTODO MVC skeleton");
    label.show();
    return app.exec();
}
