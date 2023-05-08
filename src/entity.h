#ifndef ENTITY_H
#define ENTITY_H
#include<QGraphicsPixmapItem>
#include<QGraphicsScene>
#include <QBitmap>
#include <QPainter>
#include "utils.h"
#include "map_item.h"

enum entity_type {pacman = 10, ghost};    //začátek od 10, aby nedošlo k interferenci mezi typem entity a objektem na mapě (map_item)
enum entity_direction {right, left, up, down, stopped};

class QPixmapItem;
/**
@brief Třída reprezentující pohyblivé entity na mapě
*/
class Entity : public QGraphicsPixmapItem{
    public:
    unsigned x;
    unsigned y;
    QString current_pixmap_path;

    std::vector<entity_direction> path;
    entity_direction last_path;
    std::tuple<int, int, int> color;

    private:
    entity_direction direction;
    unsigned next_sprite_index;
    void move(entity_direction dir);
    bool would_turn(entity_direction dir);
    void set_next_sprite(entity_direction dir);
    int id = 0;
    entity_type type;

    void get_color_sprites();
    std::vector<QPixmap *> ghost_sprites;

    public:
    Entity(entity_type type, unsigned x, unsigned y);
    Entity(entity_type type, unsigned x, unsigned y, int id);
    ~Entity();
    bool aligned_with_grid();
    bool movement_handler(entity_direction dir, QGraphicsScene* scene);
    entity_type get_type();
    entity_direction get_direction();
    int get_random_int(int min, int max);
    
};
#endif //ENTITY_H
