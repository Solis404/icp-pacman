#include "qgraphicsitem.h"
#include <QGraphicsPixmapItem>
#include "utils.h"

/**
@brief Výčet reprezentující typ nehybné položky na mapě
*/
using map_item_type = enum mi_type {road, wall, key, start, finish};


/**
@class Map_item
@brief Třída zajišťující logickou reprezentaci nepohyblivých položek na mapě

*/
class Map_item {
    public:
        map_item_type type;
        QGraphicsPixmapItem* scene_item;    //grafická reprezentace uložená na heapu
    public:
        Map_item(map_item_type type);
        ~Map_item();
};
