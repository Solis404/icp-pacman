#include "qgraphicsscene.h"
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

    // public slots:
    // void pacman_handler(entity_direction dir);
};
