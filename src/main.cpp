#include <QApplication>
#include <QGraphicsView>
#include "game.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    Game my_game(game_mode::replay, "logfile");

    my_game.start();

    QGraphicsView view(&my_game);
    view.show();
    
    return app.exec();
}
