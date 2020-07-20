#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "formquestionnaire.h"
#include "formeditquestions.h"
#include "formsettings.h"
#include "formplace.h"
#include "formregion.h"
#include "defining.h"

#include <QSqlDatabase>
#include <QDebug>
#include <QSettings>
#include <QFile>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
//    mdiArea = new QMdiArea(this);
//    setCentralWidget(mdiArea);

    // повторная инициализация сбивает модели, делаем один раз
    database = QSqlDatabase::addDatabase("QSQLITE","main");
    // открываем базу
    if (!OpenBase()) return;
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::OpenBase()
{
    // читаем из настроек имя базы
    QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
    databaseName = settings.value(SETTINGS_BASE_NAME, "").toString();

    //проверяем на наличие файл базы
    if(!QFile(databaseName).exists()){
        qDebug() << "Файла базы нет!";
    //    this->setWindowTitle(tr("Файла базы нет!"));
    }

    // открываем базу
    database.setDatabaseName(databaseName);
    if(!database.open()){
      qDebug() << "Ошибка открытия базы!";
      this->setWindowTitle("Error!");
      return false;
    }
    // титульный окна имя базы
    this->setWindowTitle(databaseName);
    return true;
}

void MainWindow::on_actionQuestionnaire_triggered()
{
    FormQuestionnaire *questionnaire = new FormQuestionnaire(database,this);
//    ui->tabWidget->addTab(questionnaire,tr("Анкеты"));
    ui->tabWidget->insertTab(0,questionnaire,tr("Анкеты"));
    ui->tabWidget->setCurrentIndex(0);

//    ui->tabWidget->insertTab(0,questionnaire,"--");
//    QMdiSubWindow *subWindow = mdiArea->addSubWindow(questionnaire);
//    subWindow->setAttribute(Qt::WA_DeleteOnClose);
//    subWindow->showMaximized();


}


void MainWindow::on_actionEditQuestions_triggered()
{
    FormEditQuestions *editquestions = new FormEditQuestions(database,this);
    //ui->tabWidget->addTab(editquestions,tr("Редактор вопросов"));
    ui->tabWidget->insertTab(0,editquestions,tr("Редактор вопросов"));
    ui->tabWidget->setCurrentIndex(0);
}

void MainWindow::on_actionSettings_triggered()
{
    FormSettings *settings = new FormSettings(this);
//    ui->tabWidget->insertTab(0,settings,tr("Настройки"));
//    ui->tabWidget->setCurrentIndex(0);

    settings->exec();
    database.close();
    OpenBase();

}

void MainWindow::on_actionRegion_triggered()
{
    FormRegion *region = new FormRegion(database,this);
    ui->tabWidget->insertTab(0,region,tr("Районы"));
    ui->tabWidget->setCurrentIndex(0);
}

void MainWindow::on_actionPlace_triggered()
{
    FormPlace *place = new FormPlace(database,this);
    ui->tabWidget->insertTab(0,place,tr("Районы"));
    ui->tabWidget->setCurrentIndex(0);
}


