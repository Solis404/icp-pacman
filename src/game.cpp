#include "game.h"

/**
@brief Konstruktor třídy Game.
@param file_name - Soubor s uloženou mapou
*/
Game::Game(QString file_name){
    scene = new QGraphicsScene();

    qDebug() << "[INFO]: Loading map from file:" << file_name;

    //otevření souboru s mapou
    QFile mapfile(file_name);
    mapfile.open(QIODevice::ReadOnly|QIODevice::Text);
    //otevření textového streamu pro čtení souboru
    QTextStream input(&mapfile);
    //načtení výšky a šířky mapy
    input >> map_height;
    input >> map_width;
    input.readLine();    //dočtení řádku do konce 
    //ve skutečnosti ještě mapu obklopuje neuvedená zeď
    map_height += 2;
    map_width += 2;

    qDebug() << "[INFO]: Map dimensions:" << map_height << 'x' << map_width;

    //inicializace logické reprezentace herní mapy
    matrix_repr = new Map_item**[map_height];
    for(unsigned i = 0; i < map_height; i++) {
        matrix_repr[i] = new Map_item*[map_width];
        for(unsigned j = 0; j < map_width; ++j) {
            matrix_repr[i][j] = nullptr;
        }
    }


    //nastavení mapy podle předlohy ze souboru
    //čtení každého řádku
    for(unsigned i = 1; i < map_height - 1; i++) {
        QString line = input.readLine();
        unsigned line_index = 0;
        for(unsigned j = 1; j < map_height - 1; j++) {
            QChar character = line.at(line_index);
            line_index++;
            switch(character.toLatin1()) {
                case 'T':
                    matrix_repr[i][j] = new Map_item(map_item_type::finish);
                    break;
                case 'X':
                    matrix_repr[i][j] = new Map_item(map_item_type::wall);
                    break;
                case 'G':
                    //TODO
                    matrix_repr[i][j] = new Map_item(map_item_type::road);
                    break;
                case 'K':
                    matrix_repr[i][j] = new Map_item(map_item_type::key);
                    break;
                case '.':
                    matrix_repr[i][j] = new Map_item(map_item_type::road);
                    break;
                case 'S':
                    matrix_repr[i][j] = new Map_item(map_item_type::start);
                    break;
            }
        }
    }

    //inicializace okrajů mapy na zdi
    for(unsigned i = 0; i < map_height; i++) {
        matrix_repr[i][0] = new Map_item(map_item_type::wall);
    }
    for(unsigned i = 0; i < map_height; i++) {
        matrix_repr[i][map_width - 1] = new Map_item(map_item_type::wall);
    }
    //pozor! nesmí se překrývat
    for(unsigned i = 1; i < map_height - 1; i++) {
        matrix_repr[0][i] = new Map_item(map_item_type::wall);
    }
    for(unsigned i = 1; i < map_height - 1; i++) {
        matrix_repr[map_height - 1][i] = new Map_item(map_item_type::wall);
    }

}

/**
@brief Metoda inicializující grafickou reprezentaci mapy
*/
void Game::init_scene() {
    //projde celou logickou reprezentaci a přidá všechny objekty na scénu
    for(unsigned i = 0; i < map_height; ++i) {
        for(unsigned j = 0; j < map_width; ++j) {
            matrix_repr[i][j]->scene_item->setPos(j * SPRITE_SIZE, i * SPRITE_SIZE);
            scene->addItem(matrix_repr[i][j]->scene_item);
        }
    }
    //nastaví pozadí scény na černou
    scene->setBackgroundBrush(Qt::black);
}

Game::~Game() {
    delete scene;

    //destrukce logické reprezentace mapy
    for(unsigned i = 0; i < map_height; ++i) {
        for(unsigned j = 0; j < map_width; ++j) {
            delete matrix_repr[i][j];
        }
        delete [] matrix_repr[i];
    }
    delete [] matrix_repr;
}
