#include "qgraphicsitem.h"
#include<QGraphicsPixmapItem>
#include<QGraphicsScene>
#include "utils.h"

enum entity_type {pacman, ghost};
enum entity_direction {stopped, right, left, up, down};

/**
@brief Třída reprezentující pohyblivé entity na mapě
*/
class Entity {
    public:
    QGraphicsPixmapItem* scene_item;
    unsigned x;
    unsigned y;

    private:
    entity_direction direction;
    unsigned next_sprite_index;
    entity_type type;

    public:
    Entity(entity_type type, unsigned x, unsigned y);
    ~Entity();
    void movement_handler(entity_direction dir);
    bool move(entity_direction dir);
    void load_on_scene(QGraphicsScene* scene);
    entity_type get_type();
    
};
