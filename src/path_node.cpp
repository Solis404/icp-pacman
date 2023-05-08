/**
@file

@brief Obshuje definice metod třídy Path_node

Autoři: Patrik Uher (xuherp02)
*/
#include "path_node.h"

/**
@brief Konstruktor pro tridu Path_node

@param cords - Cisla nynejsi uzlu
@param parent_cords - Cisla otcovskeho uzku nynejsiho uzlu
@param fcost - Cena f pro nynejsi uzel
*/
Path_node::Path_node(Cords cords, Cords parent_cords, int fcost)
{
    this->parent_cords = parent_cords;
    this->cords = cords;
    this->fcost = fcost;
}

Path_node::~Path_node(){};

/**
@brief Zmeni privatni attribut fcost

@param fcost - hodnota na ktere se attribut ma zmenit
*/
void Path_node::set_fcost(int fcost)
{
    this->fcost = fcost;
}

/**
@brief Zmeni privatni attribut parent_cords

@param parent_cords - hodnota na ktere se attribut ma zmenit
*/
void Path_node::set_parent_cords(Cords parent_cords)
{
    this->parent_cords = parent_cords;
}

/**
@brief Ziskani hodnoty attributu fcost
@return Hodnota fcost
*/
int Path_node::get_fcost() const
{
    return this->fcost;
}

/**
@brief Ziskani hodnoty attributu parent_cords
@return Hodnota parent_cords
*/
Cords Path_node::get_parent_cords() const
{
    return this->parent_cords;
}

/**
@brief Ziskani hodnoty attributu cords
@return Hodnota cords
*/
Cords Path_node::get_cords() const
{
    return this->cords;
}

/**
@brief Definice operatoru == pro objekt Path_node
@param other - Uzel s kterym se porovnava
@return true/false
*/
bool Path_node::operator==(const Path_node other) const
{
    return fcost == other.fcost && cords == other.cords && parent_cords == other.parent_cords;
}

/**
@brief Definice operatoru < pro objekt Path_node
@param other - Uzel s kterym se porovnava
@return true/false
*/
bool Path_node::operator<(const Path_node other) const
{
    return fcost < other.fcost;
}
