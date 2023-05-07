#include <cstdlib>
#include "map_item.h"
#include <iostream>

#ifndef LOGIC_MAP_H
#define LOGIC_MAP_H

//makro pro indexování do mapy
#define INDEX(x, y) x + this->width * y

/**
Třída pro zachycení logické reprezentace mapy, užitečné pro hledání cest (pathfinding)
*/
class Logic_map {
    private:
    size_t width;
    size_t height;
    size_t index(size_t x, size_t y);
    map_item_type* map;

    public:    
    Logic_map();
    ~Logic_map();
    void new_map(size_t width, size_t height);
    void set_tile(size_t x, size_t y, map_item_type tile_type);
    map_item_type get_tile(size_t x, size_t y);
    void pathfind_bfs();
    void get_random_path();
    void print_map();
};
#endif //LOGIC_MAP_H
