#include "entity.h"
#include "qgraphicsitem.h"
#include "src/utils.h"

/**
Proměnná ukládající jména souborů se sprajty animovaného pacmana
Uděláno tak, že lze indexovat pomocí enumu entity_direction `pacman_sprite_files[direction][N]`
*/
const QString pacman_sprite_files[5][PACMAN_SPRITES] {
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

/**
@brief Konstruktor entity, nastaví její typ a úvodní pozici na x, y

Načte také úvodní sprajt entity
@param entity_type type - Typ entity, výčtový typ = {pacman, ghost}
@param unsigned x - úvodní horizontální souřadnice entity
@param unsigned y - úvodní vertikální souřadnice entity (pozor dolů je kladný směr!)
*/
Entity::Entity(entity_type type, unsigned x, unsigned y) : QGraphicsPixmapItem() {
    this->x = x;
    this->y = y;
    this->setPos(x, y);
    this->direction = entity_direction::stopped;
    this->next_sprite_index = 0;
    this->backtrack_log = std::vector<std::tuple<entity_direction, Map_item*>>();
    this->setData(TYPE_DATA_KEY, QVariant(type));
    
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

/**
@brief Destruktor
*/
Entity::~Entity() {}

/**
@brief Getter atributu type
*/
entity_type Entity::get_type() {
    return static_cast<entity_type>(this->data(TYPE_DATA_KEY).toInt());
}

/**
@brief Nastaví další sprajt entity podle atributu next_sprite_index a inkrementuje jej

@param entity_direction dir - Sprajt se volí i na základě směru
*/
void Entity::set_next_sprite(entity_direction dir) {
    QPixmap new_sprite_pixmap;

    //pokud se změnil směr, animace jde od prvního sprajtu
    if(dir != this->direction) {
        this->next_sprite_index = 0;
    }

    switch(this->get_type()) {
        case pacman:
            new_sprite_pixmap.load(pacman_sprite_files[dir][this->next_sprite_index]);
            this->next_sprite_index = (this->next_sprite_index + 1) % PACMAN_SPRITES;
            break;
        case ghost:
            qDebug() << "[WARN]: set_next_sprite not implemented for ghosts";
            return;
            break;
    }

    this->setPixmap(new_sprite_pixmap);
}

/**
@brief Metoda pro zjištění, jestli je entita zarovnaná s mřížkou
@return bool - Vrací true, pokud je zarovnaná, false pokud není
*/
bool Entity::aligned_with_grid() {
    if(this->x % SPRITE_SIZE == 0 && this->y % SPRITE_SIZE == 0) {
        return true;
    } else {
        return false;
    }
}

/**
@brief Metoda pro zjištění, jestli entita chce zatočit

Za zatočení se považuje změna směru, která není couvání
@return bool - Vrací true, pokud by došlo k zatočení, false pokud ne
*/
bool Entity::would_turn(entity_direction dir) {
    switch(this->direction) {
        case entity_direction::right:
        case entity_direction::left:
            if(dir == up || dir == down) {
                return true;
            } else {
                return false;
            }
        case entity_direction::up:
        case entity_direction::down:
            if(dir == entity_direction::right || dir == entity_direction::left) {
                return true;
            } else {
                return false;
            }
    }

    return false;
}

/**
@brief Wrapper pro pohyb s entitou, kontroluje kolizi, zajišťuje animaci

Pohyb je povolen pouze po mřížce (není možné změnit směr v půlce bloku)

@param entity_direction dir - Směr ve kterém se má entita pohnout
@param QGraphicsScene* scene - Scéna, ve které se entita pohybuje, nutné pro kontrolu kolize
@return bool - Vrací true, pokud se entita pohnula, false pokud ne (nešlo to)
*/
bool Entity::movement_handler(entity_direction dir, QGraphicsScene* scene, bool log_backtrack) {

    //změnu směru lze provést jen, pokud je entita zarovnaná s mřížkou
    if(this->would_turn(dir) && !this->aligned_with_grid()) {
        return false;
    }

    /*detekce kolize, pokud je ve směru pohybu zeď, nic se neprovede*/

    //vyrobení bodu pro vyzkoušení kolize
    QPointF probe(this->x, this->y);
    switch(dir) {
        case entity_direction::right:
            probe.rx() += SPRITE_SIZE;
        break;
        case entity_direction::left:
            probe.rx() -= 1;
        break;
        case entity_direction::down:
            probe.ry() += SPRITE_SIZE;
        break;
        case entity_direction::up:
            probe.ry() -= 1;
        break;
        case entity_direction::stopped:
        return true;
    }

    //kontrola, že v daném směru není zeď
    Map_item* probe_target = static_cast<Map_item*>(scene->itemAt(probe, QTransform()));
    Map_item* backtrack_ptr = nullptr;
    if(probe_target != nullptr) {
        if(probe_target->get_type() == map_item_type::wall) {
            return false;
        } else if(probe_target->get_type() == map_item_type::key) {
            backtrack_ptr = probe_target;
        }
    }

    set_next_sprite(dir);
    move(dir);
    if(log_backtrack) {
        this->backtrack_log.push_back(std::tuple<entity_direction, Map_item*>(dir, backtrack_ptr));
    }
    return true;
}

/**
@brief Pohne s entitou o 1px v daném směru

Nekontroluje kolizi, očekává platný pohyb

@param entity_direction dir - Směr ve kterém se má pohnout
*/
void Entity::move(entity_direction dir) {

    this->direction = dir;    

    int dx;
    int dy;
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
    
    //pohyb
    this->moveBy(dx, dy);
    //nastaví interní x a y souřadnice
    x = this->scenePos().x();
    y = this->scenePos().y();
}

/**
@brief Getter atributu direction
*/
entity_direction Entity::get_direction() {
    return this->direction;
}

