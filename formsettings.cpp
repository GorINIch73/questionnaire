#include "formsettings.h"
#include "ui_formsettings.h"
#include "defining.h"

#include <QFileDialog>
#include <QtDebug>
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMessageBox>
#include <QSqlError>


FormSettings::FormSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormSettings)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    // считать из настроек имя базы

    QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
    ui->lineEdit_NameBase->setText(settings.value(SETTINGS_BASE_NAME, "").toString());

}

FormSettings::~FormSettings()
{
    delete ui;
}

void FormSettings::on_pushButtonClose_clicked()
{
    // сохранить в настройказ имя базы
    QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
    //пишем настройки
    settings.setValue(SETTINGS_BASE_NAME, ui->lineEdit_NameBase->text());
    settings.sync();

    close();
}

void FormSettings::on_pushButton_File_clicked()
{
    // выбор файла базы данных

    QString filename = QFileDialog::getOpenFileName(this,tr("Open base"),"./",tr("Data base Fules (*.db)"));
    if (!filename.isEmpty()) {
        ui->lineEdit_NameBase->setText(filename);
        // сохранить в настройказ имя базы
        QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
        //пишем настройки
        settings.setValue(SETTINGS_BASE_NAME, filename);
        settings.sync();
    }

}

void FormSettings::on_pushButton_clicked()
{
    // подтверждение удаление
    if(QMessageBox::Yes != QMessageBox::question(this, tr("Внимание!"),
                                                 tr("Уверены в удалении данных?")))  return;

    QString baseName = ui->lineEdit_NameBase->text();

    //проверяем на наличие файл базы
    if(!QFile(baseName).exists()){
        qDebug() << "Файла базы нет!";
        QMessageBox::information(this,"Error","Выбранной базы не существует!");
        return;
    }

// открываем базу
    QSqlDatabase dbm = QSqlDatabase::addDatabase("QSQLITE","new");
    dbm.setDatabaseName(baseName);
    if(!dbm.open()){
      qDebug() << "Ошибка открытия базы!";
      QMessageBox::critical(this,"Error",dbm.lastError().text());
      return;
    }

    QSqlQuery a_query = QSqlQuery(dbm);

    // запрос на очистку ответов
    if (!a_query.exec("DELETE FROM answers_data;")) {
        qDebug() << "таблица ответов: " << a_query.lastError().text();
        return;
    }
    // сбрасываем счетчик автоинкремента id
    if (!a_query.exec("UPDATE sqlite_sequence set seq=0 WHERE Name='answers_data'")) {
        qDebug() << "счетчик ответов: " << a_query.lastError().text();
    }
    // запрос на очистку анкет
    if (!a_query.exec("DELETE FROM questionnaire;")) {
        qDebug() << "таблица анкет: " << a_query.lastError().text();
        return;
    }

    // сбрасываем счетчик автоинкремента id
    if (!a_query.exec("UPDATE sqlite_sequence set seq=0 WHERE Name='questionnaire'")) {
        qDebug() << "счетчик ответов: " << a_query.lastError().text();
    }
    //
    QMessageBox::information(this,"Info","Операция завершена.");
    dbm.close();

}

void FormSettings::on_pushButton_Create_clicked()
{
    // создаем новую базу
    // используется имя в диалоге имя базы
    if(QMessageBox::Yes != QMessageBox::question(this, tr("Внимание!"),
                                                 tr("Уверены в создании таблиц?")))  return;

    QString baseName = ui->lineEdit_NameBase->text();

    //проверяем на наличие файл базы
    if(QFile(baseName).exists()){
        qDebug() << "Файла базы есть!";
        //QMessageBox::information(this,"Error","Выбранная база уже существует. Выберете другое имя, база создатся автоматически!");
//        return;
    }

// открываем базу
    QSqlDatabase dbm = QSqlDatabase::addDatabase("QSQLITE","new");
    dbm.setDatabaseName(baseName);
    if(!dbm.open()){
      qDebug() << "Ошибка открытия базы!";
      QMessageBox::critical(this,"Error",dbm.lastError().text());
      return;
    }

    QSqlQuery a_query = QSqlQuery(dbm);

    // запрос на создание таблицы районов
    QString str = "CREATE TABLE region ("
                "id   INTEGER       PRIMARY KEY AUTOINCREMENT UNIQUE,"
                "name VARCHAR (255));";
    if (!a_query.exec(str))
        qDebug() << "таблица районов: " << a_query.lastError().text();

    // запрос на создание таблицы мест проведения
    str = "CREATE TABLE place ("
        "id         INTEGER       PRIMARY KEY AUTOINCREMENT UNIQUE,"
        "name       VARCHAR (255),"
        "region_id  INTEGER       REFERENCES region (id),"
        "for_report BOOLEAN);";
    if (!a_query.exec(str))
        qDebug() << "таблица мест проведения: " << a_query.lastError().text();

    // запрос на создание таблицы ответов
    str = "CREATE TABLE answers ("
                "id          INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE,"
                "question_id INTEGER REFERENCES questions (id),"
                "answer      TEXT);";
    if (!a_query.exec(str))
        qDebug() << "таблица ответов: " << a_query.lastError().text();

    // запрос на создание таблицы вопросов
    str = "CREATE TABLE questions ("
            "id           INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE,"
            "question     TEXT,"
            "some_answers BOOLEAN DEFAULT (false),"
            "be_empty     BOOLEAN DEFAULT (false),"
            "satisfaction BOOLEAN DEFAULT (false));";
    if (!a_query.exec(str))
        qDebug() << "таблица вопросов: " << a_query.lastError().text();

    // запрос на создание таблицы анкет
    str = "CREATE TABLE questionnaire ("
            "id       INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE,"
            "place_id INTEGER REFERENCES place (id));";
    if (!a_query.exec(str))
        qDebug() << "таблица аекет: " << a_query.lastError().text();

    // запрос на создание таблицы данных по ответам
    str = "CREATE TABLE answers_data ("
            "id               INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE,"
            "questionnaire_id INTEGER REFERENCES questionnaire (id),"
            "question_id      INTEGER REFERENCES questions (id),"
            "answer_id        INTEGER REFERENCES answers (id));";
    if (!a_query.exec(str))
        qDebug() << "таблица данных по ответам: " << a_query.lastError().text();

    //
    QMessageBox::information(this,"Info","Операция завершена.");
    dbm.close();
}

void FormSettings::on_pushButton_Dbl_clicked()
{
    // создаем копию файла базы

}
