/**
@file

@brief Obshuje definice třídy Ui_MainWindow

Autoři: Patrik Uher (xuherp02)
Vygenerovano programem QTDesigner
*/

#ifndef TITLE_MENUOMELTH_H
#define TITLE_MENUOMELTH_H

#include <QVariant>
#include <QIcon>
#include <QtWidgets>
#include <QDebug>
#include <QFileDialog>
#include <QGraphicsView>
#include "game.h"

class Ui_MainWindow: QObject
{
    Q_OBJECT

    public:
    QMainWindow *MainWindow;
    QWidget *centralwidget;
    QPushButton *pushButton_2;
    QPushButton *pushButton_3;
    QPushButton *pushButton;
    QLabel *label_2;
    QLabel *label;
    QMenuBar *menubar;
    QStatusBar *statusbar;
    QApplication *app;
    QGraphicsView *view;
    Game *my_game;
    Replay *my_replay;
    
    public:
    void setupUi();
    void retranslateUi();
    Ui_MainWindow(QApplication *app);

    private slots:
    void exit_slot();
    void start_slot();
    void replay_slot();
    void return_from_game(game_result result);
    void return_from_replay();
};

#endif // TITLE_MENUOMELTH_H
