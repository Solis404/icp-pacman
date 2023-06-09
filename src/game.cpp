/**
@file game.cpp

@brief Definice funkcí a atributů pro třídy reprezentující herní logiku, přehrávání

Autoři: Robin Volf (xvolfr00), Patrik Uher (xuherp02)
*/

#include "game.h"
#include "qnamespace.h"
#include "qobject.h"
#include "src/entity.h"
#include "src/utils.h"
#include "src/path_node.h"
#include <algorithm>
#include <cmath>
#include <ostream>
#include <utility>
#include <vector>

/**
@brief Konstruktor, vyrobí prázdnou mapu
*/
Map_displayer::Map_displayer() : QGraphicsScene() {
    this->keys_needed = 0;
    this->map_height = 0;
    this->map_width = 0;
}

/**
@brief Destruktor
*/
Map_displayer::~Map_displayer(){}

/**
@brief Metoda pro načtení statických položek mapy (políčka)

Načte z řetězce představující mapu všechny políčka kromě duchů a pacmana

@param map - Řetězec představující mapu
*/
void Map_displayer::load_static_map_elements(QString map) {
    //načtení výšky a šířky mapy
    QTextStream* input = new QTextStream(&map);    //Stream pro čtení

    *(input) >> this->map_height;
    *(input) >> this->map_width;
    if(map_height == 0 || map_width == 0) {
        throw std::runtime_error("Invalid map size");
    }
    
    input->readLine();    //dočtení řádku do konce

    //ve skutečnosti ještě mapu obklopuje neuvedená zeď
    map_height += 2;
    map_width += 2;
    qDebug() << "[INFO]: Map dimensions:" << map_height << 'x' << map_width;

    //nastavení mapy podle předlohy ze souboru
    //čtení každého řádku
    Map_item* new_item;    //pomocný ukazatel pro vytvoření nové položky v mapě
    size_t finish_num = 0;
    size_t start_num = 0;
    for(unsigned i = 1; i < map_height - 1; i++) {
        if(input->atEnd()) {    //předčasně ukončená mapa, nesouhlasí výška
            throw std::runtime_error("Malformed map data");
        }
        QString line = input->readLine();
        if(static_cast<unsigned>(line.size()) != map_width - 2) {
            throw std::runtime_error("Garbage data after line in map data");
        }
        unsigned line_index = 0;
        for(unsigned j = 1; j < map_width - 1; j++) {
            if(line_index >= static_cast<unsigned>(line.size())) {    //nesouhlasí šířka
                throw std::runtime_error("Malformed map data");
            }
            QChar character = line.at(line_index);
            line_index++;
            map_item_type item_type;

            switch(character.toLatin1()) {
                case 'T':
                    item_type = finish;
                    finish_num++;
                    break;
                case 'X':
                    item_type = wall;
                    break;
                case 'K':
                    item_type = key;
                    this->keys_needed++;
                    break;
                case 'S':
                    item_type = start;
                    start_num++;
                    break;
                case 'G':
                case '.':
                    continue;
                default:
                    throw std::runtime_error("Unknown tile character in map data");
                }
            //vytvoření nové položky a její zařazení do scény
            new_item = new Map_item(item_type);
            this->addItem(new_item);    //scéna přebírá vlastnictví, nevolám destruktor
            //pohyb s položkou na příslušné místo
            new_item->moveBy(j * SPRITE_SIZE, i * SPRITE_SIZE);

            if(item_type == key) {    //přidání do vektoru klíčů
                this->keys.push_back(new_item);
            }

        }
    }

    //sémantická kontrola počtu políček
    if(start_num != 1 || finish_num != 1) {
        throw std::runtime_error("There can only be precisely one start and one finish tile");
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
    for(unsigned i = 1; i < map_width - 1; i++) {
        new_item = new Map_item(map_item_type::wall);
        this->addItem(new_item);
        new_item->moveBy(i * SPRITE_SIZE, 0);
    }
    for(unsigned i = 1; i < map_width - 1; i++) {
        new_item = new Map_item(map_item_type::wall);
        this->addItem(new_item);
        new_item->moveBy(i * SPRITE_SIZE, (map_height - 1) * SPRITE_SIZE);
    }

    //nastavení černého pozadí
    this->setBackgroundBrush(Qt::black);

    delete input;
}




/**
@brief Konstruktor, který vytvoří hru, volitelně s logováním
@param map_path - Cesta, kde je vstupní soubor s mapou
@param log_path - Cesta, kam se má uložit soubor s logováním, pokud není uveden,
k logování nedojde
@note Může vyhodit vyjímku std::runtime_error v případě neplatných vstupních dat
*/
Game::Game(QString map_path, QString log_path) : Map_displayer() {
    this->state = game_state::init;
    
    this->keys_acquired = 0;

    //čtení a inicializace mapy
    qDebug() << "[INFO]: Loading map from file:" << map_path;
    QFile map_file(map_path);
    map_file.open(QIODevice::ReadOnly | QIODevice::Text);    //otevření souboru pro čtení
    QString map = QString(map_file.readAll());

    this->load_static_map_elements(map);
    this->load_dynamic_map_elements(map);

    if(log_path != "") {    //nastavení logování do souboru
        //otevření logovacího souboru a inicializace streamWriteru
        this->log_file = new QFile(log_path, this);    //nový soubor pro logování, tento objekt je parent, netřeba delete
        qDebug() << "[INFO]: Saving log file to " << log_path;
        this->log_file->open(QIODevice::WriteOnly);    //vytvoří/přepíše soubor s logem
        this->xml_writer = new QXmlStreamWriter(this->log_file);
        xml_writer->setAutoFormatting(true);    //human-readable xml
        xml_writer->writeStartDocument();
        xml_writer->writeStartElement("pacman_log");

        //zapsání mapy do xml dokumentu
        xml_writer->writeStartElement("map");
        xml_writer->writeCharacters(map);
        xml_writer->writeEndElement();    //map

        //zapsání barvy všech duchů do logu
        xml_writer->writeStartElement("ghost_colors");
        for(Entity* ghost : this->ghosts) {
            xml_writer->writeStartElement("ghost");
            xml_writer->writeAttribute("r", QString::number(std::get<0>(ghost->color)));
            xml_writer->writeAttribute("g", QString::number(std::get<1>(ghost->color)));
            xml_writer->writeAttribute("b", QString::number(std::get<2>(ghost->color)));
            xml_writer->writeEndElement();    //ghost
        }
        xml_writer->writeEndElement();    //ghost_colors

        //začátek zapisování stavů
        xml_writer->writeStartElement("states");
    } else {
        qDebug() << "[INFO]: No log path specified, logging disabled";
        this->log_file = nullptr;
        this->xml_writer = nullptr;
    }

    setup_counters();

    this->play_timer = new QTimer(this);
    connect(this->play_timer, SIGNAL(timeout()), this, SLOT(elapsed_time_handler()));

    this->desired_pacman_direction = entity_direction::stopped;
    this->movement_timer = new QTimer(this);
    connect(movement_timer, SIGNAL(timeout()), this, SLOT(pacman_handler()));
    connect(movement_timer, SIGNAL(timeout()), this, SLOT(ghost_movement_handler()));

    if(log_path != "") {
        connect(movement_timer, SIGNAL(timeout()), this, SLOT(logging_handler()));
    }

    for(uint i = 0; i < this->ghosts.size(); i++)
    {
        QTimer *g_timer = new QTimer(this);
        connect(g_timer, SIGNAL(timeout()), this, SLOT(ghost_pathfind_handler()));
        this->ghost_timers.push_back(g_timer);
    }
}

/**
@brief Helper metoda inicializující počítadla času a klíčů

Inicilizuje příslušné proměnné pro čítače a jejich grafické reprezentace
*/
void Game::setup_counters() {
    //vytvoření infrastruktury pro časovač hry a vypisování uplynulého času
    this->elapsed_time = QTime(0, 0);

    //vytvoření grafických reprezentací
    this->elapsed_time_item = new QGraphicsSimpleTextItem("Time:" + this->elapsed_time.toString("mm:ss"));
    this->key_counter= new QGraphicsSimpleTextItem();

    //nastavení vlastností a posunutí na příslušnou pozici
    this->elapsed_time_item->setBrush(QBrush(Qt::white));
    this->addItem(this->elapsed_time_item);
    this->elapsed_time_item->moveBy(0, -SPRITE_SIZE);    //posunutí nad mapu

    this->key_counter->setBrush(QBrush(Qt::white));
    this->addItem(this->key_counter);
    this->key_counter->moveBy(SPRITE_SIZE * 4, -SPRITE_SIZE);    //posunutí nad mapu
    
    //update čítače klíčů
    this->update_key_counter();
}

/**
@brief Helper metoda pro updatování počítadla klíčů

Jsou updatovány hodnotami atributů keys_needed a keys_acquired
*/
void Game::update_key_counter() {
    this->key_counter->setText("Keys:" + QString::number(this->keys_acquired) + '/' + QString::number(this->keys_needed));
}

/**
@brief Načte dynamické položky v mapě - pacmana a duchy(TODO) a log. reprezentaci mapy
@param QString file_name - cesta k souboru s uloženou mapou

Nekontroluje správnost mapy, určen k volání po načtení statických položek
*/
void Game::load_dynamic_map_elements(QString map_string) {

    //načtení výšky a šířky mapy
    QTextStream* input = new QTextStream(&map_string);    //Stream pro čtení
    input->readLine();    //zahodí první řádek (velikost mapy)

    this->map_representation = new Logic_map(map_width, map_height);
    Entity *ghost;

    //nastavení mapy podle předlohy ze souboru
    //čtení každého řádku
    for(unsigned i = 1; i < map_height - 1; i++) {
        QString line = input->readLine();
        unsigned line_index = 0;
        for(unsigned j = 1; j < map_width - 1; j++) {
            QChar character = line.at(line_index);
            line_index++;
            switch(character.toLatin1()) {
                case 'T':
                    this->map_representation->set_tile(j, i, finish);
                    break;
                case 'X':
                    this->map_representation->set_tile(j, i, wall);
                    break;
                case 'G':
                    this->map_representation->set_tile(j, i, road);
                    ghost = new Entity(entity_type::ghost, j * SPRITE_SIZE, i * SPRITE_SIZE, ghost_id);
                    ghost->last_path = entity_direction::stopped;
                    this->ghosts.push_back(ghost);
                    this->ghost_id++;
                    break;
                case 'K':
                    this->map_representation->set_tile(j, i, key);
                    break;
                case '.':
                    this->map_representation->set_tile(j, i, road);
                    break;
                case 'S':
                    this->map_representation->set_tile(j, i, map_item_type::start);
                    this->pacman = new Entity(entity_type::pacman, j * SPRITE_SIZE, i * SPRITE_SIZE, 0);
                    this->addItem(this->pacman);
                    break;
            }
        }
    }

    for(uint i = 0; i < this->ghosts.size(); i++) {
        this->addItem(this->ghosts[i]);
    }

    delete input;
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
            this->desired_pacman_direction = entity_direction::left_dir;
            break;
        case Qt::Key_S:
            this->desired_pacman_direction = entity_direction::down;
            break;
        case Qt::Key_D:
            this->desired_pacman_direction = entity_direction::right_dir;
            break;
    }
}

/**
@brief Destruktor
*/
Game::~Game() {
    delete this->map_representation;

    //pokud se logovalo, zapíše se konec xml dokumentu a smaže se streamWriter
    if(xml_writer != nullptr) {
        xml_writer->writeEndDocument();
        delete this->xml_writer;
    }

}

/**
@brief Začne hru

Spustí časovače, aby hra reagovala na vstup
*/
void Game::start() {
    this->movement_timer->start(PACMAN_MOVEMENT_DELAY);
    this->play_timer->start(1000);    //interval 1s
    this->state = game_state::playing;

    int GHOST_MOVEMENT_DELAY = PACMAN_MOVEMENT_DELAY * 25;
    for(uint i = 0; i < this->ghost_timers.size(); i++)
    {
        this->ghost_timers[i]->start(GHOST_MOVEMENT_DELAY);
        GHOST_MOVEMENT_DELAY *= 2;
    }
}

/**
@brief Metoda pro zastavení hry s daným výsledkem

Emituje signál game_over
*/
void Game::stop(game_result result) {
    this->movement_timer->stop();
    this->play_timer->stop();
    this->state = game_state::finished;

    for(QTimer* timer : this->ghost_timers) {
        timer->stop();
    }
    
    emit game_over(result);
}

/**
@brief Pokusí se pohnout s pacmanem směrem, který chce hráč, pokud nelze.
Pohne s ním v původním směru
*/
void Game::pacman_handler() {
    if(this->state != game_state::playing) {
        return;
    }
    
    //pokusí se pohnout ve vyžadovaném směru, pokud nelze, pokusí se pohnout
    //stejným směrem jako minule
    if(!pacman->movement_handler(this->desired_pacman_direction, this)) {
        pacman->movement_handler(this->pacman->get_direction(), this);
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
                if(this->keys_needed == this->keys_acquired && this->pacman->collidesWithItem(item, Qt::ContainsItemShape) == true) {
                    qDebug() << "[INFO]: Pacman reached finish with all keys, ending game";
                    this->stop(game_result::victory);
                }
                break;
            case map_item_type::key:
                qDebug() << "[INFO]: Pacman acquired key";
                item->hide();
                this->keys_acquired++;
                this->update_key_counter();
                break;
            case entity_type::ghost:
                qDebug() << "[INFO]: Colliding with ghost, game over";
                this->stop(game_result::defeat);
                break;
        }
    }
}

/**
@brief Pohne se všemi duchy o 1 krok, který mají uložený v path
*/
void Game::ghost_movement_handler() {
    for(Entity* ghost : this->ghosts) {
        if(ghost->path.empty()) {
            ghost->movement_handler(ghost->last_path, this);
            continue;
        }

        ghost->movement_handler(ghost->path.front(), this);
        ghost->last_path = ghost->path.front();

        ghost->path.erase(ghost->path.begin());
    }
}

/**
@brief Handler pro počítadlo uplynulého času ve hře

Zvýší interní hodnotu času o 1s a updatuje item, který ukazuje čas
*/
void Game::elapsed_time_handler() {
    this->elapsed_time = this->elapsed_time.addSecs(1);
    this->elapsed_time_item->setText("Time:" + this->elapsed_time.toString("mm:ss"));
}

/**
@brief Handler logování informací

Loguje se:
    - pozice a sprite pacmana
    - pozice klíčů
    - pozice duchů
*/
void Game::logging_handler() {
    this->xml_writer->writeStartElement("state");

    //výpis informací o pacmanovi
    this->xml_writer->writeStartElement("pacman");
    this->xml_writer->writeAttribute("x", QString::number(this->pacman->x));
    this->xml_writer->writeAttribute("y", QString::number(this->pacman->y));
    this->xml_writer->writeAttribute("sprite", this->pacman->current_pixmap_path);
    this->xml_writer->writeEndElement();

    //výpis informací o klíčích
    this->xml_writer->writeStartElement("keys");
    //vypíše informace o každém viditelném klíči do logu
    for(size_t i = 0; i < this->keys.size(); i++) {
        if(this->keys.at(i)->isVisible()) {
            this->xml_writer->writeStartElement("key");

            this->xml_writer->writeAttribute("x", QString::number(this->keys.at(i)->scenePos().x()));
            this->xml_writer->writeAttribute("y", QString::number(this->keys.at(i)->scenePos().y()));

            this->xml_writer->writeEndElement();    //key
        }
    }
    this->xml_writer->writeEndElement();    //keys

    //výpis informacích o duších
    this->xml_writer->writeStartElement("ghosts");
    for(Entity* ghost : this->ghosts) {
        this->xml_writer->writeStartElement("ghost");
        this->xml_writer->writeAttribute("x", QString::number(ghost->x));
        this->xml_writer->writeAttribute("y", QString::number(ghost->y));
        this->xml_writer->writeAttribute("dir", QString::number(ghost->last_path));
        this->xml_writer->writeEndElement();
    }
    this->xml_writer->writeEndElement();

    this->xml_writer->writeEndElement();    //state
}

/**
@brief Konstruktor třídy Replay, vytvoří hru z logu
@param log_path - Cesta k souboru s logem

Načte statické elementy z logu

@note Při neúspěchu (soubor neexistuje, nevalidní xml) vyhazuje vyjímku std::runtime_error
*/
Replay::Replay(QString log_path) : Map_displayer() {

    qDebug() << "[INFO]: Loading log from file:" << log_path;
    QFile map_file(log_path);
    map_file.open(QIODevice::ReadOnly);    //otevření souboru pro čtení

    //načtení xml logu a vytvoření dokumentu
    this->xml_doc = QDomDocument();
    QString err_msg; int err_line; int err_col;
    if(this->xml_doc.setContent(&map_file, &err_msg, &err_line, &err_col) == false) {    //selhalo parsování dokumentu
        qDebug() << "[ERR]: Parser:" << err_msg << " at line:" << err_line << " at col:" << err_col;
        throw std::runtime_error("[ERR]: Parsing of log failed");
    }

    //načtení mapy z logu
    QDomElement map_element = xml_doc.firstChildElement();
    if(map_element.isNull()) {
        throw std::runtime_error("[ERR]: Failed to find map in log");
    }

    this->load_static_map_elements(map_element.text());

    //načtení barviček duchů
    QDomElement ghost_colors = this->xml_doc.elementsByTagName("ghost_colors").at(0).toElement();
    QDomElement ghost = ghost_colors.firstChildElement();
    while(ghost.isNull() == false) {
        int r = ghost.attribute("r").toInt();
        int g = ghost.attribute("g").toInt();
        int b = ghost.attribute("b").toInt();
        this->ghost_colors.push_back({r, g, b});
        ghost = ghost.nextSiblingElement();
    }

    this->pacman = nullptr;
    this->backtracking = false;
    this->step_timer = new QTimer(this);
}

/**
@brief Inicializuje entity (pacmana a duchy)

Načte z prvního kroku pozici pacmana a duchů a zařadí je do scény. Inicializuje také
atribut states.
*/
void Replay::initialize_entities() {
    QDomElement states = this->xml_doc.elementsByTagName("states").at(0).toElement();
    if(states.isNull()) {
        throw std::runtime_error("[ERR]: States not found in log");
    }

    QDomElement first_step = states.firstChildElement();
    QDomNodeList step_items = first_step.childNodes(); 

    //načtení elementů prvního kroku
    QDomElement pacman = step_items.at(0).toElement();
    QDomElement keys = step_items.at(1).toElement();
    QDomElement ghosts = step_items.at(2).toElement();
    if(pacman.isNull() || keys.isNull() || ghosts.isNull()) {    //kontrola existence elementů
        throw std::runtime_error("[ERR]: Missing element in state");
    }

    //nastavení pacmana
    this->pacman = this->addPixmap(QPixmap(pacman.attribute("sprite")));
    QPointF pacman_pos(pacman.attribute("x").toInt(), pacman.attribute("y").toInt());
    this->pacman->setPos(pacman_pos);

    this->current_state = first_step;

    //nastavení duchů
    QDomElement ghost = ghosts.firstChildElement();
    while(ghost.isNull() == false) {
        Entity* ghost_item = new Entity(entity_type::ghost, ghost.attribute("x").toInt(), ghost.attribute("y").toInt(), 0);
        QPointF ghost_pos(ghost.attribute("x").toInt(), ghost.attribute("y").toInt());

        //inicializace sprajtů ducha
        std::vector<QPixmap *> sprites = initialize_ghost_sprites(std::get<0>(ghost_colors.front()),
                                                                  std::get<1>(ghost_colors.front()),
                                                                  std::get<2>(ghost_colors.front()));
        ghost_colors.erase(ghost_colors.begin());    //smazání barvy
        ghost_item->ghost_sprites = sprites;

        ghost_item->setPos(ghost_pos);
        this->ghosts.push_back(ghost_item);
        this->addItem(ghost_item);

        ghost = ghost.nextSiblingElement();
    }

}

/**
@brief Metoda, která zajistí změnu klíčů (schování/odkrytí), pokud je třeba
@param keys - Parent element klíčů z xml logu
*/
void Replay::handle_key_change(QDomElement& keys) {
    std::vector<Map_item*> visible_keys;
    for(Map_item* key : this->keys) {
        if(key->isVisible()) {
            visible_keys.push_back(key);
        }
    }

    size_t num_of_keys_in_log = static_cast<size_t>(keys.childNodes().size()); 
    if(visible_keys.size()  == num_of_keys_in_log) {
        return;
    } else if(visible_keys.size() > num_of_keys_in_log) {    //musíme zakrýt klíč
        find_surplus_key(visible_keys, keys)->hide();
    } else if(visible_keys.size() < num_of_keys_in_log) {    //musíme odkrýt klíč
        find_missing_key(visible_keys, keys)->show();
    }
}

/**
@brief Funkce pro nalezení klíče, který se vyskytuje ve visible keys a zároveň se nevyskytuje v log_keys
@param visible_keys - vektor viditelných klíčů
@param log_keys - Element obsahující klíče z logu
@return Vrací ukazatel na přebytečný klíč
*/
Map_item* Replay::find_surplus_key(std::vector<Map_item *>& visible_keys, QDomElement& log_keys) {
    //iterace přes všechny viditelné klíče
    for(Map_item* key_on_screen : visible_keys) {
        QDomElement key_from_log = log_keys.firstChildElement();
        bool found_match = false;

        //iterace přes všechny klíče v logu
        while(!key_from_log.isNull() && !found_match) {
            QPointF log_key_pos(key_from_log.attribute("x").toInt(), key_from_log.attribute("y").toInt());
            if(log_key_pos == key_on_screen->pos()) {
                found_match = true;
            }
            key_from_log = key_from_log.nextSiblingElement();
        }

        if(!found_match) {    //našli jsme přebývající klíč
            return key_on_screen;
        }
    }

    return nullptr;
}

/**
@brief Funkce pro nalezení klíče, který se vyskytuje v log keys a zároveň se nevyskytuje ve visible_keys
@param visible_keys - vektor viditelných klíčů
@param log_keys - Element obsahující klíče z logu
@return Vrací ukazatel na klíč, který chybí ve visible_keys
*/
Map_item* Replay::find_missing_key(std::vector<Map_item *>& visible_keys, QDomElement& log_keys) {
    //iterace přes všechny klíče z logu
    QPointF missing_key_pos;
    QDomElement key_from_log = log_keys.firstChildElement();
    while(!key_from_log.isNull()) {
        QPointF log_key_pos(key_from_log.attribute("x").toInt(), key_from_log.attribute("y").toInt());
        bool found_match = false;

        //iterace přes všechny viditelné klíče
        for(Map_item* key_on_screen : visible_keys) {
            if(log_key_pos == key_on_screen->pos()) {
                found_match = true;
            }
        }

        if(!found_match) {    //našli jsme klíč v logu, který není mezi viditelnými klíči
            missing_key_pos = log_key_pos;
        }

        key_from_log = key_from_log.nextSiblingElement();
    }

    //ještě musíme najít, který klíč je skrytý
    for(Map_item* key : this->keys) {
        if(key->pos() == missing_key_pos) {
            return key;
        }
    }

    return nullptr;
}

/**
@brief Metoda pro zobrazení kroku na mapě
@param step - Krok z xml logu, který se má zobrazit
*/
void Replay::display_step(QDomElement& step) {
    QDomNodeList step_items = step.childNodes(); 

    //načtení elementů kroku
    QDomElement pacman = step_items.at(0).toElement();
    QDomElement keys = step_items.at(1).toElement();
    QDomElement ghosts = step_items.at(2).toElement();

    if(pacman.isNull() || keys.isNull() || ghosts.isNull()) {    //kontrola existence elementů
        throw std::runtime_error("[ERR]: Missing element in state");
    }

    //nastavení pozice a sprajtu pacmana
    QPointF new_pacman_pos(pacman.attribute("x").toInt(), pacman.attribute("y").toInt());
    QString new_pacman_sprite_path = pacman.attribute("sprite");
    this->pacman->setPos(new_pacman_pos);
    this->pacman->setPixmap(QPixmap(new_pacman_sprite_path));

    //zpracuj změnu klíčů
    handle_key_change(keys);

    //nastavení pozice duchů
    QDomElement ghost = ghosts.firstChildElement();
    for(size_t i = 0; i < this->ghosts.size(); i++) {
        QPointF new_ghost_pos(ghost.attribute("x").toInt(), ghost.attribute("y").toInt());
        this->ghosts.at(i)->setPos(new_ghost_pos);    //nastevení nové pozice
        entity_direction dir = static_cast<entity_direction>(ghost.attribute("dir").toInt());
        this->ghosts.at(i)->setPixmap(*(this->ghosts.at(i)->ghost_sprites.at(dir)));    //nastevení nového sprajtu
        ghost = ghost.nextSiblingElement();
    }

}

/**
@brief Metoda pro spuštění přehrávání hry z logu
*/
void Replay::start() {
    connect(this->step_timer, SIGNAL(timeout()), this, SLOT(step_handler()));
    this->step_timer->start(REPLAY_MOVEMENT_DELAY);
}

/**
@brief Metoda pro nastavení dalšího kroku a jeho zobrazení

Pokud se dostane na konec logu, zastaví se. Pokud by se při backtrackingu dostal na začátek,
vypíše o tom informaci a nedělá nic

Provádí detekci konce (začátku) hry - TODO
*/
void Replay::step_handler() {
    QDomElement next_state;
    if(this->backtracking == false) {
        next_state = this->current_state.nextSiblingElement();
    } else {    //backtracking
        next_state = this->current_state.previousSiblingElement();
    }

    if(next_state.isNull()) {
        if(this->backtracking == false) {
            qDebug() << "[INFO]: Log is at end";
            this->step_timer->stop();
            emit log_over();
            return;
        } else {
            qDebug() << "[INFO]: Log can't continue, hit start";
            return;
        }
    } else {
        this->current_state = next_state;
    }

    display_step(this->current_state);
}

/**
@brief Přepsaný handler pro stisk kláves

Stisknutím klávesy R se přepne režim pohybu - pokud byl aktivní backtracking, bude se pohybovat vpřed,
a naopak.
*/
void Replay::keyPressEvent(QKeyEvent *keyEvent) {
    switch(keyEvent->key()) {
        case Qt::Key_R:
            this->backtracking = !this->backtracking;
            break;
    }
}

/**
@brief Destruktor třídy Replay
*/
Replay::~Replay() {}

/**
@brief Funkce na vypocitani ceny f pro uzel 'a' kdyz cil je uzel 'b'

@param a - Uzel a jako par cisel
@param b - Uzel b jako par cisel
@return F cena uzlu
*/
int calc_fcost(Cords a, Cords b, int gcost)
{
    return 10*(std::abs(a.first - b.first) + std::abs(a.second - b.second)) + gcost;
}

/**
@brief Funkce ktera expanduje dany uzel

@param path_nodes - Vector vsech neexpandovanych uzlu
@param goal_cords - Cilovy uzel
@param parent_cords - Otcovsky uzel
@param gcost - G cena nynejsiho uzlu
*/
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

/**
@brief Funkce ktera se zavola na konec a sestroji cestu z vyuzitych uzlu

@param checked_nodes - Vektor pouzitych uzlu
@param last_cords - Konecny uzel
@return Vector smeru ktery duch musi udelat aby se dostal k pacmanovi
*/
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
        qWarning() << "[ERROR] node not found in checked node";
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
        final_direction.push_back(entity_direction::right_dir);
    }
    else if (((parent_cords.first - last_cords.first) == 1) && ((parent_cords.second-last_cords.second) == 0))
    {
        //left
        final_direction.push_back(entity_direction::left_dir);
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
        qWarning() << "[ERROR] unknown cord direction";
    }
    
    return get_final_path(checked_nodes, parent_cords);
}

/**
@brief Funkce ktera zjisti jestli uzel je uz pouzity

@param checked_nodes - Vektor pouzitych uzlu
@param node_cords - Uzel ktery resime
@return Jestli je uzel v checked_nodes
*/
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

/**
@brief Funkce ktera zjisti jestli uzel se da expandovat

@param c - Uzel ktery resime
@param checked_nodes - Vektor pouzitych uzlu
@return Jestli je uzel expandovatelny
*/
bool Game::expandable(Cords c, std::vector<Path_node> checked_nodes)
{
    bool in_ch_nodes = in_checked_nodes(checked_nodes, Cords(c.first, c.second));
    if((this->map_representation->get_tile(c.first, c.second) != map_item_type::wall) && (not in_ch_nodes))
    {
        return true;
    }
    return false;
}

/**
@brief Hlavni funkce pro pathfinding pomoci algoritmu A*

@param path_nodes - Vsechny uzly co jeste nebyli expandovane
@param end_cords - Posledni uzel v ceste
@return Vector smeru ktery duch musi udelat aby se dostal k pacmanovi
*/
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
        qWarning() << "Ghost unable to find a path to pacman";
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

/**
@brief Funkce ktera se stara o pathfinding vsech existujicich duchu

*/
void Game::ghost_pathfind_handler()
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
        qWarning() << "[ERROR] index of ghost not found";
        return;
    }

    //pokud není duch zarovnán s mřížkou, nebude mu změněna cesta
    if(this->ghosts[index]->aligned_with_grid() == false) {
        return;
    }
    
    uint pacman_pos_x = this->pacman->x/SPRITE_SIZE;
    uint pacman_pos_y = this->pacman->y/SPRITE_SIZE;
    uint ghost_pos_x = this->ghosts[index]->x/SPRITE_SIZE;
    uint ghost_pos_y = this->ghosts[index]->y/SPRITE_SIZE;

    auto path_nodes = new std::vector<Path_node>;
    Cords a = {pacman_pos_x, pacman_pos_y};
    Cords b = {ghost_pos_x, ghost_pos_y};

    path_nodes->push_back(Path_node(b, std::pair<int, int>(-1, -1), calc_fcost(a, b, 0)));
    auto final_path = Game::ghost_pathfind(path_nodes, a);

    delete path_nodes;

    //nahrazení cesty v duchovi nově vypočítanou cestou
    Entity* ghost = this->ghosts[index];
    ghost->path.clear();
    for(size_t i = 0; i < final_path.size(); i++) {
        for(size_t j = 0; j < SPRITE_SIZE; j++) {
            ghost->path.push_back(final_path.at(i));

        }
    }
}

/**
@brief Metoda pro získání vektoru sprajtů s příslušnou barvou (RGB)
@param r - Číslo reprezentující červenou intenzitu barvy
@param g - Číslo reprezentující zelenou intenzitu barvy
@param b - Číslo reprezentující modrou intenzitu barvy
@return Vrací vektor se sprajty
*/
std::vector<QPixmap *> Replay::initialize_ghost_sprites(int r, int g, int b) {
    QPixmap sprite_body, sprite_eyes, *new_sprite = nullptr;
    QBitmap mask;
    
    sprite_body.load(ghost_body_temp);
    
    mask = sprite_body.createMaskFromColor(QColor("#464646"), Qt::MaskOutColor);
    
    QPixmap *colored_body = new QPixmap(sprite_body.size());

    colored_body->fill(Qt::transparent);

    QPainter cb_painter(colored_body);
    cb_painter.drawPixmap(0, 0, sprite_body);
    cb_painter.setCompositionMode(QPainter::CompositionMode_SourceIn);

    cb_painter.fillRect(colored_body->rect(), QColor(r, g, b));
    cb_painter.end();

    
    std::vector<QPixmap *> ghost_sprites;     
    for(int i = 0; i < GHOST_SHARED; i++)
    {
        if(i == entity_direction::stopped)
        {
            // create a copy of right sprite to have a sprite for the stopped direction
            ghost_sprites.push_back(ghost_sprites[0]);
            continue;
        }

        new_sprite = new QPixmap(colored_body->size());
        new_sprite->fill(Qt::transparent);

        QPainter ns_painter(new_sprite);
        
        ns_painter.drawPixmap(0, 0, *colored_body);
        ns_painter.drawPixmap(0, 0, ghost_sprites_shared[i]);
        ns_painter.end();
        
        ghost_sprites.push_back(new_sprite);
    }

    delete colored_body;
    return ghost_sprites;
}
