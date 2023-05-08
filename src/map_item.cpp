/**
@file

@brief Obshuje definice metod třídy Map_item a s ní spojené proměnné

Autoři: Robin Volf (xvolfr00)
*/
#include "map_item.h"

/**
@brief Řetězce cest k souborům s texturamy jednotlivých položek mapy

Jsou v takovém pořadí, aby se daly indexovat pomocí map_item_type
*/
const QString map_item_sprite_files[] = {"sprites/map_sprites/wall.png",
                                         "sprites/map_sprites/key.png",
                                         "sprites/map_sprites/start.png",
                                         "sprites/map_sprites/finish.png",
                                         "sprites/map_sprites/road.png"
};

/**
@brief Konstruktor třídy Map_item

Vytvoří objekt map_item s daným typem a vytvoří grafickou reprezentaci scene_item
@param type - typ položky, jeden z "wall", "key", "start", "finish"
@param parent - Rodič, použito pro Qt parenting systém
*/
Map_item::Map_item(map_item_type type, QGraphicsItem* parent) : QGraphicsPixmapItem(parent) {
    //nastavení správné cesty k textuře a nahrání pixmapy
    QString file_name = map_item_sprite_files[type];
    this->setData(TYPE_DATA_KEY, QVariant(type));
    QPixmap pixmap(file_name);
    //nastevení Pixmapy tohoto objektu
    this->setPixmap(pixmap);
}

/**
@brief Getter pro typ objektu
@note implementováno pomocí atributu data třídy QGraphicsItem, aby to bylo
konzistentě implementováno v Map_item i Entity
*/
map_item_type Map_item::get_type() {
    return static_cast<map_item_type>(this->data(TYPE_DATA_KEY).toInt());
}
 

Map_item::~Map_item(){
}
