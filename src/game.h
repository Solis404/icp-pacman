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
#include <QXmlStreamWriter>
#include <exception>
#include "map_item.h"
#include "entity.h"
#include "utils.h"
#include "logic_map.h"

//Výčtové typy pro indikaci různých stavů hry/informací o hře
enum game_result{victory, defeat, log_end, input_file_err};
enum game_state{init, playing, paused, finished};

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
    game_state state;

    private:
    entity_direction desired_pacman_direction;

    QTimer* movement_timer;
    QTimer* play_timer;
    QTime elapsed_time;

    size_t keys_acquired; 
    size_t keys_needed;

    QGraphicsSimpleTextItem* elapsed_time_item;    //zobrazovaný čas
    QGraphicsSimpleTextItem* key_counter;    //zobrazované počítadlo klíčů

    QFile* log_file;    //soubor pro logování
    QXmlStreamWriter* xml_writer;    //stream writer vypisující log
    std::vector<Map_item*> keys;

    public:
    //konstruktor, destruktor
    Game(QString map_path, QString log_path = "");
    ~Game();

    void start();
    private:
    void stop(game_result result);
    void load_map(QString map_string);

    void keyPressEvent(QKeyEvent *keyEvent) override;

    void pacman_interaction_handler();

    void setup_play_time();
    void setup_counters();
    void setup_key_counter();
    void update_key_counter();

    private slots:
    void elapsed_time_handler();
    void pacman_handler();
    void logging_handler();

    signals:
    game_result game_over(game_result result);    //signál značící konec hry a její výsledek

};
#endif //GAME_H
