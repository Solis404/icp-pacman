/**
@file

@brief Obshuje definice metod pro tridu Ui_MainWindow

Autoři: Patrik Uher (xuherp02)
Vygenerovano programem QTDesigner
*/
#include "main_ui.h"
#include "src/game.h"

/**
@brief Konstruktor pro objekt tridy Ui_MainWindow
@param app - QT applikace
*/
Ui_MainWindow::Ui_MainWindow(QApplication *app)
{
    this->app = app;
    this->MainWindow = new QMainWindow();
    this->setupUi();

    connect(this->pushButton_3, SIGNAL(clicked()), this, SLOT(exit_slot()));
    connect(this->pushButton_2, SIGNAL(clicked()), this, SLOT(replay_slot()));
    connect(this->pushButton, SIGNAL(clicked()), this, SLOT(start_slot()));
}

/**
@brief Slot ktery vypina applikaci QT
*/
void Ui_MainWindow::exit_slot()
{
    this->app->exit(0);
}

/**
@brief Slot ktery se zpousti po dokonceni hry
*/
void Ui_MainWindow::return_from_game(game_result result)
{
    qInfo() << "result: " << (result ? "Defeat!" : "Victory!");
    this->view->hide();
    delete this->view;
    delete this->my_game;
    this->MainWindow->show();
}

/**
@brief Slot ktery se zpousti po dokonceni hry spoustene pres replay
*/
void Ui_MainWindow::return_from_replay()
{
    this->view->hide();
    delete this->view;
    delete this->my_replay;
    this->MainWindow->show();
}

/**
@brief Slot ktery se zapina hru. Zpousti se pres clicked eventem
*/
void Ui_MainWindow::start_slot()
{
    QString selected_file = QFileDialog::getOpenFileName(this->MainWindow, "Select Map");
    qInfo() << "file: " << selected_file;
    
    QString curr_time = QDateTime::currentDateTime().toString();
    QString log_file_name = QString("logfile-");
    log_file_name.append(curr_time);
    log_file_name.append(".xml");
    try{
        this->my_game = new Game(selected_file, log_file_name);

    } catch(std::runtime_error& e) {
        qDebug() << e.what();
        this->app->exit(1);
    }

    this->my_game->start();

    QGraphicsView *view = new QGraphicsView(this->my_game);
    this->view = view;
    this->MainWindow->hide();
    connect(this->my_game, SIGNAL(game_over(game_result)), this, SLOT(return_from_game(game_result)));
    view->show();
}

/**
@brief Slot ktery se zapina replay. Zpousti se pres clicked eventem
*/
void Ui_MainWindow::replay_slot()
{
    QString selected_file = QFileDialog::getOpenFileName(this->MainWindow, "Select Replay", "", tr("XML files (*.xml)"));
    qInfo() << "file: " << selected_file;

    try{
        this->my_replay = new Replay(selected_file);

    } catch(std::runtime_error& e) {
        qDebug() << e.what();
        this->app->exit(1);
    }

    this->my_replay->initialize_entities();
    this->my_replay->start();

    QGraphicsView *view = new QGraphicsView(this->my_replay);
    this->view = view;
    this->MainWindow->hide();
    connect(this->my_replay, SIGNAL(log_over()), this, SLOT(return_from_replay()));
    view->show();
}

/**
@brief Generovana funkce ktera pojmenuje elementy v QMainWindow
*/
void Ui_MainWindow::retranslateUi()
{
    MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Pacman-esque Game", nullptr));
    pushButton_2->setText(QCoreApplication::translate("MainWindow", "Replay", nullptr));
    pushButton_3->setText(QCoreApplication::translate("MainWindow", "Exit", nullptr));
    pushButton->setText(QCoreApplication::translate("MainWindow", "Start", nullptr));
    label_2->setText(QCoreApplication::translate("MainWindow", "<html><head/><body><p><span style=\" color:#ffff00;\">Pacman</span></p></body></html>", nullptr));
    label->setText(QString());
}

/**
@brief Generovana funkce ktera vytvori cely main window
*/
void Ui_MainWindow::setupUi()
{
    if (this->MainWindow->objectName().isEmpty())
        this->MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
    this->MainWindow->resize(800, 800);
    this->MainWindow->setMinimumSize(QSize(800, 800));
    this->MainWindow->setMaximumSize(QSize(800, 800));
    this->MainWindow->setCursor(QCursor(Qt::ArrowCursor));
    this->MainWindow->setMouseTracking(true);
    QIcon icon;
    icon.addFile(QString::fromUtf8("sprites/pacman_sprites/right3.png"), QSize(), QIcon::Normal, QIcon::Off);
    MainWindow->setWindowIcon(icon);
    centralwidget = new QWidget(MainWindow);
    centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
    pushButton_2 = new QPushButton(centralwidget);
    pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
    pushButton_2->setGeometry(QRect(30, 440, 311, 81));
    QFont font;
    font.setPointSize(15);
    pushButton_2->setFont(font);
    QIcon icon1;
    icon1.addFile(QString::fromUtf8("sprites/map_sprites/key.png"), QSize(), QIcon::Normal, QIcon::Off);
    pushButton_2->setIcon(icon1);
    pushButton_2->setIconSize(QSize(32, 32));
    pushButton_3 = new QPushButton(centralwidget);
    pushButton_3->setObjectName(QString::fromUtf8("pushButton_3"));
    pushButton_3->setGeometry(QRect(30, 630, 311, 81));
    pushButton_3->setFont(font);
    QIcon icon2;
    icon2.addFile(QString::fromUtf8("sprites/ghost_sprites/ghost_scared.png"), QSize(), QIcon::Normal, QIcon::Off);
    pushButton_3->setIcon(icon2);
    pushButton_3->setIconSize(QSize(32, 32));
    pushButton = new QPushButton(centralwidget);
    pushButton->setObjectName(QString::fromUtf8("pushButton"));
    pushButton->setGeometry(QRect(30, 260, 311, 81));
    pushButton->setFont(font);
    pushButton->setIcon(icon);
    pushButton->setIconSize(QSize(32, 32));
    label_2 = new QLabel(centralwidget);
    label_2->setObjectName(QString::fromUtf8("label_2"));
    label_2->setGeometry(QRect(30, 40, 361, 151));
    QFont font1;
    font1.setPointSize(64);
    label_2->setFont(font1);
    label_2->setFrameShape(QFrame::NoFrame);
    label_2->setFrameShadow(QFrame::Raised);
    label_2->setLineWidth(5);
    label_2->setTextFormat(Qt::RichText);
    label = new QLabel(centralwidget);
    label->setObjectName(QString::fromUtf8("label"));
    label->setGeometry(QRect(420, 70, 301, 641));
    label->setPixmap(QPixmap(QString::fromUtf8("sprites/finished_splash.png")));
    MainWindow->setCentralWidget(centralwidget);
    menubar = new QMenuBar(MainWindow);
    menubar->setObjectName(QString::fromUtf8("menubar"));
    menubar->setGeometry(QRect(0, 0, 800, 30));
    MainWindow->setMenuBar(menubar);
    statusbar = new QStatusBar(MainWindow);
    statusbar->setObjectName(QString::fromUtf8("statusbar"));
    MainWindow->setStatusBar(statusbar);

    retranslateUi();

    QMetaObject::connectSlotsByName(MainWindow);
} // setupUi
