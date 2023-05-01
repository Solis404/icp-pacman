#ifndef MAP_ITEM_H
#define MAP_ITEM_H
#include "qgraphicsitem.h"
#include <QGraphicsPixmapItem>
#include "utils.h"

/**
@brief Výčet reprezentující typ nehybné položky na mapě
*/
enum map_item_type{wall, key, start, finish};


/**
@class Map_item
@brief Třída zajišťující logickou reprezentaci nepohyblivých položek na mapě

*/
class Map_item : public QGraphicsPixmapItem {
    public:
        map_item_type get_type();
        Map_item(map_item_type type, QGraphicsItem* parent = nullptr);
        ~Map_item();
};
#endif //MAP_ITEM_H
