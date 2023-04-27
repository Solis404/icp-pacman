#include <QApplication>
#include <QGraphicsView>
#include "game.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    Game my_game;
    my_game.load_map("map.txt");


    // for(int i = 0; i < 20; i++) {
    //     my_game.pacman->movement_handler(entity_direction::right);
    // }
    // qDebug() << my_game.pacman->x;
    // qDebug() << my_game.pacman->y;

    QGraphicsView view(&my_game);
    view.show();
    
    return app.exec();
}
