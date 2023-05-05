#include "game.h"
#include "qnamespace.h"
#include "src/utils.h"


Map_displayer::Map_displayer() : QGraphicsScene() {
    this->keys_needed = 0;
    this->map_height = 0;
    this->map_width = 0;
}

Map_displayer::~Map_displayer(){}

/**
@brief Metoda pro načtení statických položek mapy (políčka)
@param QString map - Řetězec představující mapu
*/
void Map_displayer::load_static_map_elements(QString map) {
    //načtení výšky a šířky mapy
    QTextStream input = QTextStream(&map);    //Stream pro čtení

    input >> this->map_height;
    input >> this->map_width;
    if(map_height == 0 || map_width == 0) {
        throw std::runtime_error("Invalid map size");
    }
    
    input.readLine();    //dočtení řádku do konce

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
        if(input.atEnd()) {    //předčasně ukončená mapa, nesouhlasí výška
            throw std::runtime_error("Malformed map data");
        }
        QString line = input.readLine();
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
}




/**
@brief Konstruktor, který vytvoří hru, volitelně s logováním
@param QString map_path - Cesta, kde je vstupní soubor s mapou
@param QString log_path - Cesta, kam se má uložit soubor s logováním, pokud není uveden,
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

        //zapsání mapy do xml dokumentu
        xml_writer->writeStartElement("map");
        xml_writer->writeCharacters(map);
        xml_writer->writeEndElement();    //map

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
    if(log_path != "") {
        connect(movement_timer, SIGNAL(timeout()), this, SLOT(logging_handler()));
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
    QTextStream input = QTextStream(&map_string);    //Stream pro čtení
    input.readLine();    //zahodí první řádek (velikost mapy)

    this->map_representation = new Logical_map(map_width, map_height);

    //nastavení mapy podle předlohy ze souboru
    //čtení každého řádku
    for(unsigned i = 1; i < map_height - 1; i++) {
        QString line = input.readLine();
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
                    //TODO: přidat do vektoru duchů
                    break;
                case 'K':
                    this->map_representation->set_tile(j, i, key);
                    break;
                case '.':
                    this->map_representation->set_tile(j, i, road);
                    break;
                case 'S':
                    this->map_representation->set_tile(j, i, map_item_type::start);
                    this->pacman = new Entity(entity_type::pacman, j * SPRITE_SIZE, i * SPRITE_SIZE);
                    this->addItem(this->pacman);
                    break;
                }
        }
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
}

/**
@brief Metoda pro zastavení hry s daným výsledkem

Emituje signál game_over
*/
void Game::stop(game_result result) {
    this->movement_timer->stop();
    this->play_timer->stop();
    this->state = game_state::finished;
    emit game_over(result);
}

// entity_direction Game::get_dir_from_log() {
//     if(this->movement_log.size() == 0) {    //kontrola konce logu
//         qDebug() << "[INFO]: Reached end of log, ending game";
//         this->stop(game_result::log_end);
//     }

//     bool ok;
//     int dir = (QString() + this->movement_log.at(0)).toInt(&ok);
//     if(!ok) {    //znak nebylo možné převést na číslo
//         qDebug() << "[INFO]: Malformed log, ending game";
//         this->stop(game_result::input_file_err);
//     }

//     switch(dir) {
//         case entity_direction::stopped:
//         case entity_direction::right:
//         case entity_direction::left:
//         case entity_direction::up:
//         case entity_direction::down:
//             break;
//         default:    //dir není platný směr
//             qDebug() << "[INFO]: Malformed log, ending game";
//             this->stop(game_result::input_file_err);
//     }
//     this->movement_log.remove(0, 1);
//     return static_cast<entity_direction>(dir);
// }

/**
@brief Pokusí se pohnout s pacmanem směrem, který chce hráč, pokud nelze.
Pohne s ním v původním směru
*/
void Game::pacman_handler() {
    //zapíše právě chtěný pohyb pacmana do řetězce pro logování, funkční pouze v manuálním režimu
    // if(this->log_file != nullptr) {
    //     this->movement_log.append(QString::number(this->desired_pacman_direction));
    // }

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
                qDebug() << "[WARN]: Collision with ghosts not implemented yet";
                break;
        }
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
    

    this->xml_writer->writeEndElement();    //state
}
