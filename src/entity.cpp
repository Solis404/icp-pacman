#include "entity.h"
#include "qgraphicsitem.h"

/**
Proměnná ukládající jména souborů se sprajty animovaného pacmana
Uděláno tak, že lze indexovat pomocí enumu entity_direction `pacman_sprite_files[direction][N]`
*/
const QString pacman_sprite_files[5][5] {
    {"sprites/pacman_sprites/stopped.png"},
    {"sprites/pacman_sprites/right0.png",
     "sprites/pacman_sprites/right1.png",
     "sprites/pacman_sprites/right2.png",
     "sprites/pacman_sprites/right3.png",
     "sprites/pacman_sprites/right4.png"},
    {"sprites/pacman_sprites/left0.png",
     "sprites/pacman_sprites/left1.png",
     "sprites/pacman_sprites/left2.png",
     "sprites/pacman_sprites/left3.png",
     "sprites/pacman_sprites/left4.png"},
    {"sprites/pacman_sprites/up0.png",
     "sprites/pacman_sprites/up1.png",
     "sprites/pacman_sprites/up2.png",
     "sprites/pacman_sprites/up3.png",
     "sprites/pacman_sprites/up4.png"},
    {"sprites/pacman_sprites/down0.png",
     "sprites/pacman_sprites/down1.png",
     "sprites/pacman_sprites/down2.png",
     "sprites/pacman_sprites/down3.png",
     "sprites/pacman_sprites/down4.png"},
};

Entity::Entity(entity_type type, unsigned x, unsigned y) {
    //inicializuje grafickou reprezentaci entity s počátečním sprajtem
    this->x = x;
    this->y = y;
    switch (type) {
        case pacman:
            scene_item = new QGraphicsPixmapItem(QPixmap(pacman_sprite_files[entity_direction::stopped][0]));
            break;
        case ghost:
            qWarning() << "[WARN]: Ghost not implemented yet!";
            break;
    }
}

Entity::~Entity() {}

entity_type Entity::get_type() {
    return this->type;
}

/**
@brief Vykreslí pacmana na scéně na pozici x,y 
*/
void Entity::load_on_scene(QGraphicsScene* scene) {
    this->scene_item->setPos(this->x, this->y);
    scene->addItem(this->scene_item);
    qDebug() << "[INFO]: Loaded pacman in location: x:" << this->x << " y:"<< this->y;
}
