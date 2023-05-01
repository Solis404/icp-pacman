#include "game.h"
#include "src/utils.h"

/**
@brief Konstruktor třídy Game.
*/
Game::Game(QString file_name) : QGraphicsScene() {
    this->keys_needed = 0;
    load_map(file_name);
    this->desired_pacman_direction = entity_direction::stopped;

    this->pacman_timer = new QTimer(this);
    connect(pacman_timer, SIGNAL(timeout()), this, SLOT(pacman_handler()));
}

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
                    continue;
                    //TODO dodělat inicializaci duchů
                    break;
                case 'K':
                    item_type = key;
                    this->keys_needed++;
                    break;
                case '.':
                    continue;
                    break;
                case 'S':
                    item_type = map_item_type::start;
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

/**
@brief Handler pro stisknutí klávesy

Nastaví požadovaný směr pacmana od uživatele
*/
void Game::keyPressEvent(QKeyEvent *keyEvent) {
    switch(keyEvent->key()) {
        case Qt::Key_W:
            this->desired_pacman_direction = entity_direction::up;
            break;
        case Qt::Key_A:
            this->desired_pacman_direction = entity_direction::left;
            break;
        case Qt::Key_S:
            this->desired_pacman_direction = entity_direction::down;
            break;
        case Qt::Key_D:
            this->desired_pacman_direction = entity_direction::right;
            break;
    }
}

/**
@brief Destruktor
*/
Game::~Game() {}

/**
@brief Začne hru

Spustí časovače, aby hra reagovala na vstup
*/
void Game::start() {
    this->pacman_timer->start(PACMAN_MOVEMENT_DELAY);
}

void Game::stop() {
    this->pacman_timer->stop();
}

/**
@brief Pokusí se pohnout s pacmanem směrem, který chce hráč, pokud nelze.
Pohne s ním v původním směru
*/
void Game::pacman_handler() {
    //zpracování pohybu
    if(pacman->movement_handler(this->desired_pacman_direction, this)) {
        qDebug() << "[INFO]: Succesful desired direction movement";
    } else {
        pacman->movement_handler(this->pacman->get_direction(), this);
        qDebug() << "[INFO]: Desired direction movement failed continuing";
    }
    //zpracování interakce s okolím
    pacman_interaction_handler();
}

/**
Metoda zajišťující interakci pacnmana s okolím (duchy, klíči, cílem)
*/
void Game::pacman_interaction_handler() {
    QList<QGraphicsItem *> collides_with = this->pacman->collidingItems();

    //nalezení objektů, s nimiž by měl pacman interreagovat
    for (int i = 0; i < collides_with.size(); ++i) {
        QGraphicsItem* item = collides_with.at(i);
        int type = item->data(TYPE_DATA_KEY).toInt();
        switch(type) {
            case map_item_type::finish:
                //v případě cíle musí celý pacman být v cíli
                if(this->keys_needed == 0 && this->pacman->collidesWithItem(item, Qt::ContainsItemShape) == true) {
                    qDebug() << "[INFO]: Pacman colliding with finish, ending game";
                    this->stop();
                }
                break;
            case map_item_type::key:
                qDebug() << "[INFO]: Pacman colliding with key";
                this->removeItem(item);    //scéna již objekt klíč nevlastní
                delete item;
                this->keys_needed--;
                break;
            case entity_type::ghost:
                qDebug() << "[WARN]: Collision with ghosts not implemented yet";
                break;
        }
    }
}
