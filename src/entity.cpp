#include "entity.h"
#include "qgraphicsitem.h"
#include "src/utils.h"

/**
Proměnná ukládající jména souborů se sprajty animovaného pacmana
Uděláno tak, že lze indexovat pomocí enumu entity_direction `pacman_sprite_files[direction][N]`
*/
const int GHOST_SHARED = 5;

const QString pacman_sprite_files[5][PACMAN_SPRITES] {
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
    {"sprites/pacman_sprites/stopped.png"},
};

const QString ghost_body_temp = "sprites/ghost_sprites/ghost_right.png";
const QString ghost_scared = "sprites/ghost_sprites/ghost_scared.png";

const QString ghost_sprites_shared[GHOST_SHARED]
{
    "sprites/ghost_sprites/eyes_right.png",
    "sprites/ghost_sprites/eyes_left.png",
    "sprites/ghost_sprites/eyes_up.png",
    "sprites/ghost_sprites/eyes_down.png"
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
    this->setData(TYPE_DATA_KEY, QVariant(type));
    
    //inicializuje grafickou reprezentaci entity s počátečním sprajtem
    this->setPixmap(QPixmap(pacman_sprite_files[entity_direction::stopped][0]));
    this->current_pixmap_path = pacman_sprite_files[entity_direction::stopped][0];
}

Entity::Entity(entity_type type, unsigned x, unsigned y, int id) : QGraphicsPixmapItem() {
    this->x = x;
    this->y = y;
    this->setPos(x, y);
    this->direction = entity_direction::stopped;
    this->next_sprite_index = 0;
    this->type = type;
    this->id = id;

    //inicializuje grafickou reprezentaci entity s počátečním sprajtem
    get_color_sprites();
    this->setPixmap(*ghost_sprites[entity_direction::right]);
}

int Entity::get_random_int(int min, int max)
{
    max += 1;
    max -= min;
    int rand_num = rand();

    return min + (rand_num % max);
}

void Entity::get_color_sprites()
{
    QPixmap sprite_body, sprite_eyes;
    QBitmap mask;
    int r = 0, g = 0, b = 0;
    
    sprite_body.load(ghost_body_temp);
    sprite_eyes.load(ghost_sprites_shared[entity_direction::right]);
    
    mask = sprite_body.createMaskFromColor(QColor("#464646"), Qt::MaskOutColor);
    
    QPixmap *colored_body = new QPixmap(sprite_body.size());

    colored_body->fill(Qt::transparent);

    QPainter cb_painter(colored_body);
    cb_painter.drawPixmap(0, 0, sprite_body);
    cb_painter.setCompositionMode(QPainter::CompositionMode_SourceIn);

    r = get_random_int(0, 255);
    g = get_random_int(0, 255);
    b = get_random_int(0, 255);


    //TODO implement color saving
    
    cb_painter.fillRect(colored_body->rect(), QColor(r, g, b));
    cb_painter.end();
    
    QPixmap *new_sprite = new QPixmap(colored_body->size());
    new_sprite->fill(Qt::transparent);

    QPainter ns_painter(new_sprite);
    
    ns_painter.drawPixmap(0, 0, *colored_body);
    ns_painter.drawPixmap(0, 0, sprite_eyes);
    ns_painter.end();

    delete colored_body;

    this->ghost_sprites.push_back(new_sprite);
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
            this->current_pixmap_path = pacman_sprite_files[dir][this->next_sprite_index];
            this->next_sprite_index = (this->next_sprite_index + 1) % PACMAN_SPRITES;
            break;
        case ghost:
            this->setPixmap(*ghost_sprites[dir]);
            return;
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
        case entity_direction::stopped:
            break;
        default:
            qWarning() << "[ERROR] Neocekavany typ smeru";
            exit(1);
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
bool Entity::movement_handler(entity_direction dir, QGraphicsScene* scene) {

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
    if(probe_target != nullptr && probe_target->get_type() == map_item_type::wall) {
        return false;
    }

    set_next_sprite(dir);
    move(dir);
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
        case entity_direction::stopped:
            return;
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

