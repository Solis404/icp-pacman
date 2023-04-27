#include "game.h"
#include "src/utils.h"

/**
@brief Konstruktor třídy Game.
*/
Game::Game() : QGraphicsScene() {}

/**
@brief Načte mapu uloženou v souboru filename
*/
void Game::load_map(QString file_name) {
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

    //nastavení mapy podle předlohy ze souboru
    //čtení každého řádku
    Map_item* new_item;    //pomocný ukazatel pro vytvoření nové položky v mapě
    for(unsigned i = 1; i < map_height - 1; i++) {
        QString line = input.readLine();
        unsigned line_index = 0;
        for(unsigned j = 1; j < map_height - 1; j++) {
            QChar character = line.at(line_index);
            line_index++;
            map_item_type item_type;
            switch(character.toLatin1()) {
                case 'T':
                    item_type = finish;
                    break;
                case 'X':
                    item_type = wall;
                    break;
                case 'G':
                    item_type = road;
                    //TODO dodělat inicializaci duchů
                    break;
                case 'K':
                    item_type = key;
                    break;
                case '.':
                    item_type = road;
                    break;
                case 'S':
                    item_type = start;
                    this->pacman = new Entity(entity_type::pacman, j * SPRITE_SIZE, i * SPRITE_SIZE);
                    break;
                }
            //vytvoření nové položky a její zařazení do scény
            new_item = new Map_item(item_type);
            this->addItem(new_item);    //scéna přebírá vlastnictví, nevolám destruktor
            //pohyb s položkou na příslušné místo
            new_item->moveBy(j * SPRITE_SIZE, i * SPRITE_SIZE);
        }
    }

    /*inicializace okrajů mapy na zdi*/

    for(unsigned i = 0; i < map_height; i++) {
        new_item = new Map_item(map_item_type::wall);
        this->addItem(new_item);
        new_item->moveBy(0, i * SPRITE_SIZE);
    }
    for(unsigned i = 0; i < map_height; i++) {
        new_item = new Map_item(map_item_type::wall);
        this->addItem(new_item);
        new_item->moveBy((map_width - 1) * SPRITE_SIZE, i * SPRITE_SIZE);
    }
    //pozor! nesmí se překrývat, začíná od 1 a končí dřív
    for(unsigned i = 1; i < map_height - 1; i++) {
        new_item = new Map_item(map_item_type::wall);
        this->addItem(new_item);
        new_item->moveBy(i * SPRITE_SIZE, 0);
    }
    for(unsigned i = 1; i < map_height - 1; i++) {
        new_item = new Map_item(map_item_type::wall);
        this->addItem(new_item);
        new_item->moveBy(i * SPRITE_SIZE, (map_height - 1) * SPRITE_SIZE);
    }

    //nastavení černého pozadí
    this->setBackgroundBrush(Qt::black);
    this->addItem(this->pacman);
}

void Game::keyPressEvent(QKeyEvent *keyEvent) {
    switch(keyEvent->key()) {
        case Qt::Key_W:
            this->pacman->movement_handler(entity_direction::up, this);
            break;
        case Qt::Key_A:
            this->pacman->movement_handler(entity_direction::left, this);
            break;
        case Qt::Key_S:
            this->pacman->movement_handler(entity_direction::down, this);
            break;
        case Qt::Key_D:
            this->pacman->movement_handler(entity_direction::right, this);
            break;
    }
}

Game::~Game() {}

// void Game::pacman_handler(entity_direction dir) {
//     pacman->movement_handler(entity_direction dir);
// }
