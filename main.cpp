#include <Application.h>
#include "Views/MainWindow.h"

int main(int argc, char *argv[])
{
    Application::setAttribute(Qt::AA_EnableHighDpiScaling);
    Application app(argc, argv);

    MainWindow w;
    w.show();

    return app.exec();
}
