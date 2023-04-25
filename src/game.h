#include "qobjectdefs.h"
#include <QGraphicsScene>

/**
@brief Třída reprezentující samotnou hru

Detailní popis (TODO)
*/
class Game : public QGraphicsScene {
    Q_OBJECT    //tato třída bude mít sloty a signály

    public:
        //konstruktor, destruktor
        Game(QObject* parent = nullptr);    //TODO parametry -> mapa
        ~Game();

    //další věci
};
