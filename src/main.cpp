#include <QApplication>
#include <QGraphicsView>
#include "game.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    Game my_game("map.txt");
    my_game.init_scene();

    for(int i = 0; i < 20; i++) {
        my_game.pacman->move(entity_direction::right);
    }
    qDebug() << my_game.pacman->x;
    qDebug() << my_game.pacman->y;

    QGraphicsView view(my_game.scene);
    view.show();
    
    return app.exec();
}
