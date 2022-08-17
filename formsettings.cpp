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


FormSettings::FormSettings(QSqlDatabase db,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormSettings)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    // считать из настроек имя базы

//    QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
//    ui->lineEdit_NameBase->setText(settings.value(SETTINGS_BASE_NAME, "").toString());

    base=db;

    model = new QSqlTableModel(this,base);

    //Настраиваем модели
    SetupTable();

    model->select();

    ui->tableView->selectRow(0);


}

FormSettings::~FormSettings()
{
    delete ui;
}

void FormSettings::SetupTable()
{
    //Таблица
    model->setTable("setings");
    // названия колонок
    model->setHeaderData(1,Qt::Horizontal,"Опция");
    model->setHeaderData(2,Qt::Horizontal,"Значение");

    ui->tableView->setModel(model);
    ui->tableView->setColumnHidden(0, true);    // Скрываем колонку с id записей
//    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);  //запрет редактирования
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows); // Разрешаем выделение строк
//    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection); // Устанавливаем режим выделения лишь одно строки в таблице
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents); // по содержимому

}


void FormSettings::on_pushButtonClose_clicked()
{
//    // сохранить в настройказ имя базы
//    QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
//    //пишем настройки
//    settings.setValue(SETTINGS_BASE_NAME, ui->lineEdit_NameBase->text());
//    settings.sync();

    close();
}

void FormSettings::on_pushButton_clicked()
{
    // подтверждение удаление
    if(QMessageBox::Yes != QMessageBox::question(this, tr("Внимание!"),
                                                 tr("Уверены в удалении данных?")))  return;

    QSqlQuery a_query = QSqlQuery(base);
 // очистка данных по анкетам
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

  // очистка списка вопросов
    if(ui->checkBox_clr_q->isChecked()) {
        // запрос на очистку справоцника ответов
        if (!a_query.exec("DELETE FROM answers;")) {
            qDebug() << "справочника ответов: " << a_query.lastError().text();
            return;
        }
        // сбрасываем счетчик автоинкремента id
        if (!a_query.exec("UPDATE sqlite_sequence set seq=0 WHERE Name='answers'")) {
            qDebug() << "счетчик справочника ответов: " << a_query.lastError().text();
        }
        // запрос на очистку справочника вопросов
        if (!a_query.exec("DELETE FROM questions;")) {
            qDebug() << "справочника вопросов: " << a_query.lastError().text();
            return;
        }
        // сбрасываем счетчик автоинкремента id
        if (!a_query.exec("UPDATE sqlite_sequence set seq=0 WHERE Name='questions'")) {
            qDebug() << "счетчик справочника вопросов: " << a_query.lastError().text();
        }
    }
  // очистка списка мест проведения профилей районов
    if(ui->checkBox_clr_pl->isChecked()) {
        // запрос на очистку справоцника мест
        if (!a_query.exec("DELETE FROM place;")) {
            qDebug() << "справочника мест: " << a_query.lastError().text();
            return;
        }
        // сбрасываем счетчик автоинкремента id
        if (!a_query.exec("UPDATE sqlite_sequence set seq=0 WHERE Name='place'")) {
            qDebug() << "счетчик справочника мест: " << a_query.lastError().text();
        }
        // запрос на очистку справоцника районов
        if (!a_query.exec("DELETE FROM region;")) {
            qDebug() << "справочника районов: " << a_query.lastError().text();
            return;
        }
        // сбрасываем счетчик автоинкремента id
        if (!a_query.exec("UPDATE sqlite_sequence set seq=0 WHERE Name='region'")) {
            qDebug() << "счетчик справочника районов: " << a_query.lastError().text();
        }
        // запрос на очистку справоцника профилей
        if (!a_query.exec("DELETE FROM profil;")) {
            qDebug() << "справочника профилей: " << a_query.lastError().text();
            return;
        }
        // сбрасываем счетчик автоинкремента id
        if (!a_query.exec("UPDATE sqlite_sequence set seq=0 WHERE Name='profil'")) {
            qDebug() << "счетчик справочника профилей: " << a_query.lastError().text();
        }






    }

    //
    QMessageBox::information(this,"Info","Операция завершена.");

}


