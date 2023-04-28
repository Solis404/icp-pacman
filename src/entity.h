#ifndef ENTITY_H
#define ENTITY_H
#include "qgraphicsitem.h"
#include<QGraphicsPixmapItem>
#include<QGraphicsScene>
#include "utils.h"
#include "map_item.h"

enum entity_type {pacman, ghost};
enum entity_direction {stopped, right, left, up, down};

/**
@brief Třída reprezentující pohyblivé entity na mapě
*/
class Entity : public QGraphicsPixmapItem{
    public:
    unsigned x;
    unsigned y;

    private:
    entity_direction direction;
    unsigned next_sprite_index;
    entity_type type;
    void move(entity_direction dir);
    bool aligned_with_grid();
    bool would_turn(entity_direction dir);
    void set_next_sprite(entity_direction dir);

    public:
    Entity(entity_type type, unsigned x, unsigned y);
    ~Entity();
    bool movement_handler(entity_direction dir, QGraphicsScene* scene);
    entity_type get_type();
    entity_direction get_direction();
    
};
#endif //ENTITY_H
