#include "qobjectdefs.h"
#include <QGraphicsScene>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include "map_item.h"
#include "entity.h"

/**
@brief Třída reprezentující samotnou hru

Detailní popis (TODO)
*/
class Game {
    Q_OBJECT
    
    public:
    QGraphicsScene* scene;
    Map_item*** matrix_repr;    //2d pole ukazatelů
    unsigned map_height;
    unsigned map_width;
    Entity* pacman;

    public:
    //konstruktor, destruktor
    Game(QString);
    ~Game();
    void init_scene();

    public slots:
    void pacman_handler(entity_direction dir);
};
