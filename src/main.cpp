#include <QApplication>
#include <QGraphicsView>
#include <cstdlib>
#include <stdexcept>
#include "game.h"

int main(int argc, char* argv[]) {
    srand((unsigned int)time(0));

    QApplication app(argc, argv);

    Game* my_game;
    // Replay* my_game;
    try{
        // my_game = new Replay("logfile.xml");
        my_game = new Game("examples/map.txt", "logfile.xml");

    } catch(std::runtime_error& e) {
        qDebug() << e.what();
        return EXIT_FAILURE;
    }

    // my_game->initialize_entities();
    my_game->start();

    QGraphicsView view(my_game);
    view.show();
    
    app.exec();
    delete my_game;
    return EXIT_SUCCESS;
}
