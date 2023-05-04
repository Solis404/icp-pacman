#ifndef GAME_H
#define GAME_H
#include <QGraphicsScene>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QKeyEvent>
#include <QTimer>
#include "map_item.h"
#include "entity.h"
#include "utils.h"
#include "logic_map.h"

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
    Logical_map map_representation;


    private:
    entity_direction desired_pacman_direction;
    std::vector<Entity *> ghosts;
    int ghost_id;
    QTimer* pacman_timer;
    unsigned keys_needed;

    public:
    //konstruktor, destruktor
    Game(QString file_name);
    ~Game();
    void load_map(QString file_name);
    void keyPressEvent(QKeyEvent *keyEvent) override;
    void start();
    void stop();
    void pacman_interaction_handler();
    private slots:
    void pacman_handler();
};
#endif //GAME_H
