/**
@file

@brief Obshuje definic třídy Path_node

Autoři: Patrik Uher (xuherp02)
*/
#include "utils.h"

#ifndef PATH_NODE_H
#define PATH_NODE_H

class Path_node
{
    public:
        Path_node(Cords cords, Cords parent_cords, int fcost);
        ~Path_node();
        void set_fcost(int fcost);
        void set_parent_cords(Cords parent_cords);
        int get_fcost() const;
        Cords get_parent_cords() const;
        Cords get_cords() const;
        bool operator==(const Path_node other) const;
        bool operator<(const Path_node other) const;

    private:
        Cords cords;
        Cords parent_cords;
        int fcost;
};

#endif //PATH_NODE_H
