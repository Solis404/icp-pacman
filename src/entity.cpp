#include "entity.h"
#include "qgraphicsitem.h"
#include "src/utils.h"

/**
Proměnná ukládající jména souborů se sprajty animovaného pacmana
Uděláno tak, že lze indexovat pomocí enumu entity_direction `pacman_sprite_files[direction][N]`
*/
const QString pacman_sprite_files[5][ANIM_SPRITES] {
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

Entity::Entity(entity_type type, unsigned x, unsigned y) : QGraphicsPixmapItem() {
    this->x = x;
    this->y = y;
    this->setPos(x, y);
    this->direction = entity_direction::stopped;
    this->next_sprite_index = 0;
    this->type = type;
    
    //inicializuje grafickou reprezentaci entity s počátečním sprajtem
    switch (type) {
        case pacman:
            this->setPixmap(QPixmap(pacman_sprite_files[entity_direction::stopped][0]));
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
@brief Wrapper pro pohyb s entitou, kontroluje kolizi
*/
void Entity::movement_handler(entity_direction dir) {
    move(dir);
}


/**
@brief Pohne s entitou o 1px v daném směru

@param entity_direction dir - Směr ve kterém se má pohnout
*/
void Entity::move(entity_direction dir) {
    unsigned dx;
    unsigned dy;
    QString sprite_path;
    switch(dir) {
        case entity_direction::right:
            dx = 1;
            dy = 0;
            break;
        case entity_direction::left:
            dx = -1;
            dy = 0;
            break;
        case entity_direction::up:
            dx = 0;
            dy = -1;
            break;
        case entity_direction::down:
            dx = 0;
            dy = 1;
            break;
    }

    qDebug() << "Entity with coords x:" << x << "y:" << y << " attempting move";
    
    //nastaví další sprajt entity (animace) a zvýší index o 1 (cyklus -> %)
    this->setPixmap(QPixmap(pacman_sprite_files[dir][this->next_sprite_index]));
    this->next_sprite_index = (this->next_sprite_index + 1) % ANIM_SPRITES;
    this->moveBy(dx, dy);

    //nastaví x a y souřadnice
    x = this->scenePos().x();
    y = this->scenePos().y();
}

