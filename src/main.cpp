#include <QApplication>
#include <QGraphicsView>
#include <cstdlib>
#include <stdexcept>
#include "main_ui.h"

int main(int argc, char* argv[]) {
    srand((unsigned int)time(0));

    QApplication app(argc, argv);
    
    Ui_MainWindow mwin(&app);
    mwin.MainWindow->show();

    return app.exec();
}
