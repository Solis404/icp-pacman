#ifndef GAME_H
#define GAME_H
#include "qgraphicsscene.h"
#include "qobjectdefs.h"
#include <QGraphicsScene>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QKeyEvent>
#include <QTimer>
#include <QTime>
#include <QGraphicsSimpleTextItem>
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
    QTimer* pacman_timer;
    QTimer* play_timer;
    QTime elapsed_time;
    size_t keys_acquired; 
    size_t keys_needed;
    QGraphicsSimpleTextItem* elapsed_time_item;
    QGraphicsSimpleTextItem* key_counter;


    public:
    //konstruktor, destruktor
    Game(QString file_name);
    ~Game();
    void start();
    void stop();
    private:
    void load_map(QString file_name);

    void keyPressEvent(QKeyEvent *keyEvent) override;

    void pacman_interaction_handler();

    void setup_play_time();
    void setup_key_counter();
    void update_key_counter();
    private slots:
    void elapsed_time_handler();
    void pacman_handler();
};
#endif //GAME_H
