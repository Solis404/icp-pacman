#ifndef GAME_H
#define GAME_H
#include "qgraphicsscene.h"
#include "qobjectdefs.h"
#include <QGraphicsScene>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QKeyEvent>
#include "map_item.h"
#include "entity.h"

/**
@brief Třída reprezentující samotnou hru

Detailní popis (TODO)
*/
class Game : public QGraphicsScene {
    Q_OBJECT
    
    public:
    unsigned map_height;
    unsigned map_width;
    Entity* pacman;

    public:
    //konstruktor, destruktor
    Game();
    ~Game();
    void load_map(QString file_name);
    void keyPressEvent(QKeyEvent *keyEvent) override;
    // public slots:
    // void pacman_handler(entity_direction dir);
};
#endif //GAME_H
