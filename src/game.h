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

//Výčtové typy pro indikaci různých stavů hry/informací o hře
enum game_result{victory, defeat};
enum game_state{init, playing, paused, finished};
enum game_mode{manual, replay};

/**
@brief Třída reprezentující samotnou hru, její logiku a reprezentaci

Dědí od QT třídy QGraphicsScene, tudíž v sobě zakomponovává i grafickou reprezentaci,
kterou lze vizualizovat pomocí QGraphicsView. Při její konstrukci dojde k vytvoření
mapy. Metodou start() se hra spustí a TODO
*/
class Game : public QGraphicsScene {
    Q_OBJECT
    
    public:
    unsigned map_height;
    unsigned map_width;
    Entity* pacman;
    Logical_map* map_representation;
    game_mode mode;
    game_state state;

    private:
    entity_direction desired_pacman_direction;

    QTimer* pacman_timer;
    QTimer* play_timer;
    QTime elapsed_time;

    size_t keys_acquired; 
    size_t keys_needed;

    QGraphicsSimpleTextItem* elapsed_time_item;    //zobrazovaný čas
    QGraphicsSimpleTextItem* key_counter;    //zobrazované počítadlo klíčů

    QFile* log_file;    //soubor pro logování
    QString movement_log;    //místo pro uložení logu pohybu

    public:
    //konstruktor, destruktor
    Game(game_mode mode, QString input_path, QString log_path = "");
    ~Game();

    void start();
    void stop();
    private:
    void load_map(QString input);

    void keyPressEvent(QKeyEvent *keyEvent) override;

    void pacman_interaction_handler();

    void setup_play_time();
    void setup_counters();
    void setup_key_counter();
    void update_key_counter();
    private slots:
    void elapsed_time_handler();
    void pacman_handler();
};
#endif //GAME_H
