#include <QApplication>
#include <QGraphicsView>
#include "game.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    Game my_game;
    my_game.addText("hello_world");

    QGraphicsView view(&my_game);
    view.show();
    
    //inicializace grafického rozhraní
    
    return app.exec();
}
