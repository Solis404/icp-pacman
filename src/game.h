#ifndef GAME_H
#define GAME_H
#include <QGraphicsScene>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QKeyEvent>
#include <QTimer>
#include <vector>
#include "map_item.h"
#include "entity.h"
#include "utils.h"
#include "logic_map.h"
#include "path_node.h"

/**
@brief Třída reprezentující samotnou hru

Detailní popis (TODO)
*/
class Game : public QGraphicsScene {
    Q_OBJECT
    typedef unsigned int uint;
    
    public:
    unsigned map_height;
    unsigned map_width;
    Entity* pacman;
    Logic_map map_representation;

    private:
    entity_direction desired_pacman_direction;
    std::vector<Entity *> ghosts;
    std::vector<QTimer *> ghost_timers;
    int ghost_id;
    bool expandable(Cords c, std::vector<Path_node>);
    QTimer* pacman_timer;
    unsigned keys_needed;

    public:
    Game(QString file_name);
    ~Game();
    void load_map(QString file_name);
    void keyPressEvent(QKeyEvent *keyEvent) override;
    void start();
    void stop();
    void pacman_interaction_handler();

    private slots:
    void pacman_handler();
    void ghost_handler();
    std::vector<entity_direction> ghost_pathfind(std::vector<Path_node> *path_nodes, Cords end);
};
#endif //GAME_H
