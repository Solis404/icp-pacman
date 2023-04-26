#include "qobjectdefs.h"
#include <QGraphicsScene>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include "map_item.h"

/**
@brief Třída reprezentující samotnou hru

Detailní popis (TODO)
*/
class Game {
    public:
    QGraphicsScene* scene;
    Map_item*** matrix_repr;    //2d pole ukazatelů
    unsigned map_height;
    unsigned map_width;

    public:
    //konstruktor, destruktor
    Game(QString);
    ~Game();
    void init_scene();
};
