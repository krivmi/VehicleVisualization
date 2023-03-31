#include <QApplication>

#include "mainmapwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainMapWindow mainMapWindow;
    mainMapWindow.resize(1520, 740);
    mainMapWindow.setMinimumSize(1024, 640);
    mainMapWindow.setWindowTitle("Vehicle Vizualization");
    mainMapWindow.show();

    return app.exec();
}
