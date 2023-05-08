/**
@file

@brief Obshuje definic třídy Logic_map

Autoři: Robin Volf (xvolfr00)
*/
#include <cstdlib>
#include "map_item.h"
#include <iostream>

#ifndef LOGIC_MAP_H
#define LOGIC_MAP_H

//makro pro indexování do mapy
#define INDEX(x, y) x + this->width * y

/**
@brief Třída pro zachycení logické reprezentace mapy, užitečné pro hledání cest (pathfinding)
*/
class Logic_map {
    private:
    size_t width;    ///< Šířka mapy
    size_t height;    ///< Výška mapy
    map_item_type* map;    ///< Ukazatel na vlastní data o mapě

    size_t index(size_t x, size_t y);

    public:    
    Logic_map(size_t width, size_t height);
    ~Logic_map();
    void set_tile(size_t x, size_t y, map_item_type tile_type);
    map_item_type get_tile(size_t x, size_t y);
    void print_map();
};
#endif //LOGIC_MAP_H
