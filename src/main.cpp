#include <QApplication>
#include <QGraphicsView>
#include "game.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    Game my_game("map.txt");
    my_game.init_scene();

    QGraphicsView view(my_game.scene);
    view.show();
    
    return app.exec();
}
