#include "game.h"
#include "qobject.h"
#include "src/entity.h"
#include "src/utils.h"
#include <algorithm>
#include <cmath>
#include <ostream>
#include <utility>
#include <vector>

/**
@brief Konstruktor třídy Game.
*/
Game::Game(QString file_name) : QGraphicsScene() {
    this->keys_needed = 0;
    this->map_representation = Logic_map();
    load_map(file_name);
    this->desired_pacman_direction = entity_direction::stopped;

    this->pacman_timer = new QTimer(this);
    connect(pacman_timer, SIGNAL(timeout()), this, SLOT(pacman_handler()));

    for(uint i = 0; i < this->ghosts.size(); i++)
    {
        QTimer *g_timer = new QTimer(this);
        connect(g_timer, SIGNAL(timeout()), this, SLOT(ghost_handler()));
        this->ghost_timers.push_back(g_timer);
    }
}

/**
@brief Načte mapu uloženou v souboru filename
*/
void Game::load_map(QString file_name) {
    /* qDebug() << "[INFO]: Loading map from file:" << file_name; */
    this->ghost_id = 0;

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

    this->map_representation.new_map(map_width, map_height);

    /* qDebug() << "[INFO]: Map dimensions:" << map_height << 'x' << map_width; */

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
            Entity *ghost;

            switch(character.toLatin1()) {
                case 'T':
                    item_type = finish;
                    this->map_representation.set_tile(j, i, finish);
                    break;
                case 'X':
                    item_type = wall;
                    this->map_representation.set_tile(j, i, wall);
                    break;
                case 'G':
                    this->map_representation.set_tile(j, i, road);
                    item_type = road;
                    ghost = new Entity(entity_type::ghost, j * SPRITE_SIZE, i * SPRITE_SIZE, ghost_id);
                    this->ghosts.push_back(ghost);
                    this->ghost_id++;
                    break;
                case 'K':
                    item_type = key;
                    this->map_representation.set_tile(j, i, key);
                    this->keys_needed++;
                    break;
                case '.':
                    this->map_representation.set_tile(j, i, road);
                    continue;
                    break;
                case 'S':
                    this->map_representation.set_tile(j, i, map_item_type::start);
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
        this->map_representation.set_tile(0, i, wall);
    }
    for(unsigned i = 0; i < map_height; i++) {
        new_item = new Map_item(map_item_type::wall);
        this->addItem(new_item);
        new_item->moveBy((map_width - 1) * SPRITE_SIZE, i * SPRITE_SIZE);
        this->map_representation.set_tile((map_width - 1) , i, wall);
    }
    //pozor! nesmí se překrývat, začíná od 1 a končí dřív
    for(unsigned i = 1; i < map_width - 1; i++) {
        new_item = new Map_item(map_item_type::wall);
        this->addItem(new_item);
        new_item->moveBy(i * SPRITE_SIZE, 0);
        this->map_representation.set_tile(i , 0, wall);
    }
    for(unsigned i = 1; i < map_width - 1; i++) {
        new_item = new Map_item(map_item_type::wall);
        this->addItem(new_item);
        new_item->moveBy(i * SPRITE_SIZE, (map_width - 1) * SPRITE_SIZE);
        this->map_representation.set_tile(i , (map_width - 1), wall);
    }

    //nastavení černého pozadí
    this->setBackgroundBrush(Qt::black);
    this->addItem(this->pacman);

    for(uint i = 0; i < this->ghosts.size(); i++)
    {
        this->addItem(this->ghosts[i]);
    }
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
    int GHOST_MOVEMENT_DELAY = PACMAN_MOVEMENT_DELAY*100;

    for(uint i = 0; i < this->ghost_timers.size(); i++)
    {
        this->ghost_timers[i]->start(GHOST_MOVEMENT_DELAY);
        GHOST_MOVEMENT_DELAY *= 2;
    }
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
        /* qDebug() << "[INFO]: Succesful desired direction movement"; */
    } else {
        pacman->movement_handler(this->pacman->get_direction(), this);
        /* qDebug() << "[INFO]: Desired direction movement failed continuing"; */
    }
    //zpracování interakce s okolím
    pacman_interaction_handler();
}

/**
@brief Metoda zajišťující interakci pacnmana s okolím (duchy, klíči, cílem)

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
                    /* qDebug() << "[INFO]: Pacman colliding with finish, ending game"; */
                    this->stop();
                }
                break;
            case map_item_type::key:
                /* qDebug() << "[INFO]: Pacman colliding with key"; */
                this->removeItem(item);    //scéna již objekt klíč nevlastní
                delete item;
                this->keys_needed--;
                break;
            case entity_type::ghost:
                /* qDebug() << "[WARN]: Collision with ghosts not implemented yet"; */
                break;
        }
    }
}

int calc_fcost(Cords a, Cords b, int gcost)
{
    return 10*(std::abs(a.first - b.first) + std::abs(a.second - b.second)) + gcost;
}

void expand_node(std::vector<Path_node> *path_nodes, Cords goal_cords,Cords expanded_cords, Cords parent_cords, int gcost)
{
        int new_fcost = calc_fcost(expanded_cords, goal_cords, gcost);

        for (auto i : (*path_nodes))
        {
            if(i.get_cords() == expanded_cords)
            {
                if(new_fcost > i.get_fcost())
                {
                    return;
                }

                auto i_it = std::find(path_nodes->begin(), path_nodes->end(), i);
                path_nodes->erase(i_it);

                break;
            }
        }
        Path_node new_node(expanded_cords, parent_cords, new_fcost);
        path_nodes->push_back(new_node);
}

std::vector<entity_direction> get_final_path(std::vector<Path_node> checked_nodes, Cords last_cords)
{
    static std::vector<entity_direction> final_direction;
    bool found = false;
    Cords parent_cords;

    for(auto i:checked_nodes)
    {
        if(last_cords == i.get_cords())
        {
            found = true;
            parent_cords = i.get_parent_cords();
        }
    }

    if(not found)
    {
        qWarning() << "[ERROR] node not found in checked node" << Qt::endl;
        return std::vector<entity_direction>(); //return empty vector
    }

    if(parent_cords == Cords(-1, -1))
    {
        auto ret = final_direction;
        final_direction.clear();

        return ret;
    }

    if(((parent_cords.first - last_cords.first) == -1) && ((parent_cords.second-last_cords.second) == 0))
    {
        //right
        final_direction.push_back(entity_direction::right);
    }
    else if (((parent_cords.first - last_cords.first) == 1) && ((parent_cords.second-last_cords.second) == 0))
    {
        //left
        final_direction.push_back(entity_direction::left);
    }
    else if (((parent_cords.first - last_cords.first) == 0) && ((parent_cords.second-last_cords.second) == -1))
    {
        //down
        final_direction.push_back(entity_direction::down);
    }
    else if (((parent_cords.first - last_cords.first) == 0) && ((parent_cords.second-last_cords.second) == 1))
    {
        //up
        final_direction.push_back(entity_direction::up);
    }
    else
    {
        qWarning() << "[ERROR] unknown cord direction" << Qt::endl;
    }
    
    return get_final_path(checked_nodes, parent_cords);
}

bool in_checked_nodes(std::vector<Path_node> checked_nodes, Cords node_cords)
{
    for(auto i: checked_nodes)
    {
        if(i.get_cords() == node_cords)
        {
            return true;
        }
    }
    return false;
}

bool Game::expandable(Cords c, std::vector<Path_node> checked_nodes)
{
    bool in_ch_nodes = in_checked_nodes(checked_nodes, Cords(c.first, c.second));
    if((this->map_representation.get_tile(c.first, c.second) != map_item_type::wall) && (not in_ch_nodes))
    {
        return true;
    }
    return false;
}

std::vector<entity_direction> Game::ghost_pathfind(std::vector<Path_node> *path_nodes, Cords end_cords)
{
    std::sort(path_nodes->begin(), path_nodes->end());
    std::vector<Path_node>::iterator node_it = path_nodes->begin();
    static std::vector<Path_node> checked_nodes;
    static int gcost = 1;
    Cords c;

    if(node_it == path_nodes->end())
    {
        // empty set
        gcost = 1;
        checked_nodes.clear();
        qWarning() << "Ghost unable to find a path to pacman" << Qt::endl;
        return std::vector<entity_direction> {entity_direction::stopped};
    }

    auto cords = (*node_it).get_cords();
    checked_nodes.push_back(*node_it);
    path_nodes->erase(node_it);

    if(cords == end_cords)
    {
        gcost = 1;
        auto ret = get_final_path(checked_nodes, end_cords); 
        checked_nodes.clear();

        std::reverse(ret.begin(), ret.end());

        return ret;
    }

    c = Cords(cords.first, cords.second+1); 
    if(expandable(c, checked_nodes))
    {
        expand_node(path_nodes, end_cords, c, cords, gcost);
    }

    c = Cords(cords.first+1, cords.second); 
    if(expandable(c, checked_nodes))
    {
        expand_node(path_nodes, end_cords, c, cords, gcost);
    }

    c = Cords(cords.first-1, cords.second); 
    if(expandable(c, checked_nodes))
    {
        expand_node(path_nodes, end_cords, c, cords, gcost);
    }

    c = Cords(cords.first, cords.second-1); 
    if(expandable(c, checked_nodes))
    {
        expand_node(path_nodes, end_cords, c, cords, gcost);
    }

    gcost++;
    return Game::ghost_pathfind(path_nodes, end_cords);
}

void Game::ghost_handler()
{
    // find ghost index
    QObject *sender = QObject::sender();
    int index = -1;
    for(uint i = 0; i < ghost_timers.size(); i++)
    {
        if(sender == ghost_timers[i])
        {
            index = i;
            break;
        }
    }

    if(index == -1)
    {
        qWarning() << "[ERROR] index of ghost not found" << Qt::endl;
        return;
    }
    
    uint pacman_pos_x = this->pacman->x/SPRITE_SIZE;
    uint pacman_pos_y = this->pacman->y/SPRITE_SIZE;
    uint ghost_pos_x = this->ghosts[index]->x/SPRITE_SIZE;
    uint ghost_pos_y = this->ghosts[index]->y/SPRITE_SIZE;

    auto path_nodes = new std::vector<Path_node>;
    Cords a = {pacman_pos_x, pacman_pos_y};
    Cords b = {ghost_pos_x, ghost_pos_y};

    path_nodes->push_back(Path_node(b, std::pair(-1, -1), calc_fcost(a, b, 0)));
    auto final_path = Game::ghost_pathfind(path_nodes, a);

    delete path_nodes;

    if(final_path.empty())
    {
        qWarning() << "[ERROR] final path is empty" << Qt::endl;
        return;
    }
    else
    {
        std::cout << "Final path for ghost[" << index << "]: ";
        for (auto& i : final_path)
        {
            std::cout << i << ", ";
        }
        std::cout << std::endl;
    }
}
