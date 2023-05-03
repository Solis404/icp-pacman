#include "game.h"
#include "src/utils.h"

/**
@brief Konstruktor, který vytvoří hru buď v manuáním nebo replay režimu
@param QString input_path - Cesta, kde je vstupní soubor (mapa u manuálního režimu,
log u replay režimu)
@param QString log_path - Cesta, kam se má uložit soubor s logováním, pokud není uveden,
k logování nedojde
*/
Game::Game(game_mode mode, QString input_path, QString log_path) : QGraphicsScene() {
    this->mode = mode;
    
    this->keys_needed = 0; //nutné inicializovat nyní, v metodě load_map() se k tomu přičítá
    this->keys_acquired = 0;

    //čtení a inicializace mapy
    qDebug() << "[INFO]: Loading map from file:" << input_path;
    load_map(input_path);

    if(mode == manual && log_path != "") {    //nastavení logování do souboru
        qDebug() << "[INFO]: Saving log file to " << log_path;
        QFile::copy(input_path, log_path);    //mapa se uloží do logovacího souboru
        this->log_file = new QFile(log_path);
        this->log_file->open(QIODevice::Text | QIODevice::Append);
    } else {
        qDebug() << "[INFO]: No log path specified, logging disabled";
        this->log_file = nullptr;
    }

    setup_counters();

    this->play_timer = new QTimer(this);
    connect(this->play_timer, SIGNAL(timeout()), this, SLOT(elapsed_time_handler()));

    this->desired_pacman_direction = entity_direction::stopped;
    this->pacman_timer = new QTimer(this);
    connect(pacman_timer, SIGNAL(timeout()), this, SLOT(pacman_handler()));
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
@brief Načte mapu uloženou v souboru filename
@param QString file_name - cesta k souboru s uloženou mapou

Inicializuje proměnné spojené s mapu v třídě Game (map_height, map_width, map_representation)
*/
void Game::load_map(QString input_path) {

    //načtení výšky a šířky mapy
    QFile input_file = QFile(input_path);
    input_file.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream input = QTextStream(&input_file);
    input >> map_height;
    input >> map_width;
    input.readLine();    //dočtení řádku do konce 
    //ve skutečnosti ještě mapu obklopuje neuvedená zeď
    map_height += 2;
    map_width += 2;

    this->map_representation = new Logical_map(map_width, map_height);

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
                    this->map_representation->set_tile(j, i, finish);
                    break;
                case 'X':
                    item_type = wall;
                    this->map_representation->set_tile(j, i, wall);
                    break;
                case 'G':
                    this->map_representation->set_tile(j, i, road);
                    continue;
                    //TODO dodělat inicializaci duchů
                    break;
                case 'K':
                    item_type = key;
                    this->map_representation->set_tile(j, i, key);
                    this->keys_needed++;
                    break;
                case '.':
                    this->map_representation->set_tile(j, i, road);
                    continue;
                    break;
                case 'S':
                    this->map_representation->set_tile(j, i, map_item_type::start);
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
        this->map_representation->set_tile(0, i, wall);
    }
    for(unsigned i = 0; i < map_height; i++) {
        new_item = new Map_item(map_item_type::wall);
        this->addItem(new_item);
        new_item->moveBy((map_width - 1) * SPRITE_SIZE, i * SPRITE_SIZE);
        this->map_representation->set_tile((map_width - 1) , i, wall);
    }
    //pozor! nesmí se překrývat, začíná od 1 a končí dřív
    for(unsigned i = 1; i < map_width - 1; i++) {
        new_item = new Map_item(map_item_type::wall);
        this->addItem(new_item);
        new_item->moveBy(i * SPRITE_SIZE, 0);
        this->map_representation->set_tile(i , 0, wall);
    }
    for(unsigned i = 1; i < map_width - 1; i++) {
        new_item = new Map_item(map_item_type::wall);
        this->addItem(new_item);
        new_item->moveBy(i * SPRITE_SIZE, (map_width - 1) * SPRITE_SIZE);
        this->map_representation->set_tile(i , (map_width - 1), wall);
    }

    //nastavení černého pozadí
    this->setBackgroundBrush(Qt::black);
    this->addItem(this->pacman);

    this->movement_log = input.readLine();
}

/**
@brief Handler pro stisknutí klávesy

Nastaví požadovaný směr pacmana od uživatele
*/
void Game::keyPressEvent(QKeyEvent *keyEvent) {
    if(this->mode == game_mode::manual) {
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
}

/**
@brief Destruktor
*/
Game::~Game() {
    delete this->map_representation;

    if(this->log_file != nullptr) {    //zapíše do logovacího souboru log pohybu
        QTextStream log(this->log_file);
        log << this->movement_log << Qt::endl;
    }
}

/**
@brief Začne hru

Spustí časovače, aby hra reagovala na vstup
*/
void Game::start() {
    this->pacman_timer->start(PACMAN_MOVEMENT_DELAY);
    this->play_timer->start(1000);    //interval 1s
}

void Game::stop() {
    this->pacman_timer->stop();
    this->play_timer->stop();
}

/**
@brief Pokusí se pohnout s pacmanem směrem, který chce hráč, pokud nelze.
Pohne s ním v původním směru
*/
void Game::pacman_handler() {
    //zapíše právě chtěný pohyb pacmana do řetězce pro logování, funkční pouze v manuálním režimu
    if(this->log_file != nullptr) {
        this->movement_log.append(QString::number(this->desired_pacman_direction));
    }

    //nastaví směr pacmana podle logu
    if(this->mode == game_mode::replay) {
        int dir = (QString() + this->movement_log.at(0)).toInt();
        this->movement_log.remove(0, 1);
        qDebug() << "[INFO]: Dir from log:" << dir;
        this->desired_pacman_direction = static_cast<entity_direction>(dir);
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
                    this->stop();
                }
                break;
            case map_item_type::key:
                qDebug() << "[INFO]: Pacman acquired key";
                this->removeItem(item);    //scéna již objekt klíč nevlastní
                delete item;
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
