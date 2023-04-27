#include "map_item.h"

/**
@brief Řetězce cest k souborům s texturamy jednotlivých položek mapy

Jsou v takovém pořadí, aby se daly indexovat pomocí map_item_type
*/
const QString map_item_sprite_files[] = {"sprites/map_sprites/road.png",
                                         "sprites/map_sprites/wall.png",
                                         "sprites/map_sprites/key.png",
                                         "sprites/map_sprites/start.png",
                                         "sprites/map_sprites/finish.png",
};

/**
@brief Konstruktor třídy Map_item

Vytvoří objekt map_item s daným typem a vytvoří grafickou reprezentaci scene_item
@param type - typ položky, jeden z "road", "wall", "key", "start", "finish"
*/
Map_item::Map_item(map_item_type type, QGraphicsItem* parent) : QGraphicsPixmapItem(parent) {
    //nastavení správné cesty k textuře a nahrání pixmapy
    QString file_name = map_item_sprite_files[type];
    QPixmap pixmap(file_name);
    //nastevení Pixmapy tohoto objektu
    this->setPixmap(pixmap);
}

Map_item::~Map_item(){
}
