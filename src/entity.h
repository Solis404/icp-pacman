/**
@file

@brief Definice Třídy Entity a s ní spojovaných výčtových typů

Autoři: Robin Volf (xvolfr00), Patrik Uher (xuherp02)
*/
#ifndef ENTITY_H
#define ENTITY_H
#include<QGraphicsPixmapItem>
#include<QGraphicsScene>
#include <QBitmap>
#include <QPainter>
#include "utils.h"
#include "map_item.h"

extern const int GHOST_SHARED;
extern const QString ghost_body_temp;
extern const QString ghost_sprites_shared[];

enum entity_type {pacman = 10, ghost};    //začátek od 10, aby nedošlo k interferenci mezi typem entity a objektem na mapě (map_item)
enum entity_direction {right, left, up, down, stopped};

/**
@brief Třída reprezentující pohyblivé entity na mapě
*/
class Entity : public QGraphicsPixmapItem{
    public:
    unsigned x;    ///< Horizontální (x) pozice entity
    unsigned y;    ///< Horizontální (y) pozice entity
    QString current_pixmap_path;    ///< Cesta k aktuálně načtenému sprajtu

    std::vector<entity_direction> path;    ///< Naplánovaná cesta k cíli
    entity_direction last_path;    ///< Poslední použitý směr z cesty
    std::tuple<int, int, int> color;    ///< Barva sprajtu (použité u duchů)
    std::vector<QPixmap *> ghost_sprites;    ///< Vygenerované sprajty z barvy a předlohy (použité u duchů)

    private:
    entity_direction direction;    ///< Vygenerované sprajty z barvy a předlohy (použité u duchů)
    unsigned next_sprite_index;    ///< Index na další sprajt, který se má načíst (pro animaci)
    int id = 0;
    entity_type type;

    private:
    void move(entity_direction dir);
    bool would_turn(entity_direction dir);
    void set_next_sprite(entity_direction dir);
    void get_color_sprites();

    public:
    Entity(entity_type type, unsigned x, unsigned y, int id);
    ~Entity();
    bool aligned_with_grid();
    bool movement_handler(entity_direction dir, QGraphicsScene* scene);
    entity_type get_type();
    entity_direction get_direction();
    int get_random_int(int min, int max);
    
};
#endif //ENTITY_H
