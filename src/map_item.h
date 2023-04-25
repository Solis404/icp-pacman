#include "qgraphicsitem.h"
#include <QGraphicsPixmapItem>

/**
@brief Třída reprezentující jednotlivé položky na mapě
*/
class Map_item : public QGraphicsPixmapItem {
    public:
        Map_item(QGraphicsItem *parent = nullptr);
        ~Map_item();
};
