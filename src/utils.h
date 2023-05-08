/**
@file

@brief Obshuje definice maker pro nastavení různých parametrů hry a using deklarací.

Autoři: Robin Volf (xvolfr00) Patrik Uher (xuherp02)
*/
#ifndef UTILS_H
#define UTILS_H
#include <QDebug>
#include <vector>

#define SPRITE_SIZE 20
#define PACMAN_SPRITES 5
#define PACMAN_MOVEMENT_DELAY 40
#define TYPE_DATA_KEY 0
#define GHOST_SPRITES_DATA_KEY 1
#define REPLAY_MOVEMENT_DELAY 40

using Cords = std::pair<int, int>;
using uint = unsigned int;

#endif //UTILS_H
