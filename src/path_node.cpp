#include "path_node.h"

Path_node::Path_node(Cords cords, Cords parent_cords, int fcost)
{
    this->parent_cords = parent_cords;
    this->cords = cords;
    this->fcost = fcost;
}

Path_node::~Path_node(){};

void Path_node::set_fcost(int fcost)
{
    this->fcost = fcost;
}

void Path_node::set_parent_cords(Cords parent_cords)
{
    this->parent_cords = parent_cords;
}

int Path_node::get_fcost() const
{
    return this->fcost;
}

Cords Path_node::get_parent_cords() const
{
    return this->parent_cords;
}

Cords Path_node::get_cords() const
{
    return this->cords;
}

bool Path_node::operator==(const Path_node other) const
{
    return fcost == other.fcost && cords == other.cords && parent_cords == other.parent_cords;
}

bool Path_node::operator<(const Path_node other) const
{
    return fcost < other.fcost;
}
