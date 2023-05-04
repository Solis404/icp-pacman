#include "logic_map.h"

Logical_map::Logical_map(){
    this->width = 0;
    this->height = 0;
    this->map = nullptr;
}

/**
@brief Kontruktor, inicializuje prázdnou mapu s šířkou width a výškou height
*/
void Logical_map::new_map(size_t width, size_t height) {
    this->width = width;
    this->height = height;

    //výkonově šetrnější způsob vyrobení 2D pole, nutno indexovat pomocí makra INDEX(x,y)
    this->map = new map_item_type[width * height];
}

/**
@brief Destruktor třídy Logical_map
*/
Logical_map::~Logical_map() {
    delete[] this->map;
}

/**
@brief Setter pro nastavování hodnoty políčka [x,y] v mapě
*/
void Logical_map::set_tile(size_t x, size_t y, map_item_type tile_type) {
    this->map[INDEX(x, y)] = tile_type;
}

/**
@brief Getter pro získání hodnoty políčka [x,y] v mapě
*/
map_item_type Logical_map::get_tile(size_t x, size_t y) {
    return this->map[INDEX(x,y)];
}

/**
@brief Metoda vypíše mapu na standartní chybový výstup, pro ladící účely
*/
void Logical_map::print_map() {
    for(size_t i = 0; i < this->height; i++) {
        for(size_t j = 0; j < this->width; j++) {
            switch(this->map[INDEX(j, i)]) {
                case map_item_type::finish:                
                    std::cerr << 'T';
                    break;
                case map_item_type::start:                
                    std::cerr << 'S';
                    break;
                case map_item_type::wall:                
                    std::cerr << 'X';
                    break;
                case map_item_type::key:                
                    std::cerr << 'K';
                    break;
                case map_item_type::road:                
                    std::cerr << '.';
                    break;
            }
        }
        std::cerr << std::endl;
    }
}

