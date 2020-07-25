#include "formeditquestions.h"
#include "ui_formeditquestions.h"

#include <QMessageBox>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>

FormEditQuestions::FormEditQuestions(QSqlDatabase db,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormEditQuestions)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    //ui->pushButtonClose->setStyleSheet("background-color: red");


    base=db;

    modelQuestions = new QSqlTableModel(this,base);
    modelAnswers = new QSqlTableModel(this,base);
    mapper = new QDataWidgetMapper(this);

    //Настраиваем модели
    SetupTable();

    modelQuestions->select();
    modelAnswers->select();


    // сигнал изменения строки выделения в tableVewQuestion
    connect(ui->tableView_questions->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
                 SLOT(slotSelectionChange(const QItemSelection &, const QItemSelection &)));

    // Enter по умолчанию на кнопку редактирования
    //ui->pushButton_ ->setDefault(true);
    //устанавливаем таблицу на первую запись
    ui->tableView_questions->selectRow(0);

}

FormEditQuestions::~FormEditQuestions()
{
    delete ui;
}

void FormEditQuestions::on_pushButtonClose_clicked()
{
    close();
}

void FormEditQuestions::SetupTable()
{

    //Таблица вопросов
    modelQuestions->setTable("questions");
    // названия колонок
    modelQuestions->setHeaderData(1,Qt::Horizontal,"Вопрос");
    ui->tableView_questions->setModel(modelQuestions);
    ui->tableView_questions->setColumnHidden(0, true);    // Скрываем колонку с id записей
    ui->tableView_questions->setEditTriggers(QAbstractItemView::NoEditTriggers);  //запрет редактирования
    ui->tableView_questions->setSelectionBehavior(QAbstractItemView::SelectRows); // Разрешаем выделение строк
    ui->tableView_questions->setSelectionMode(QAbstractItemView::SingleSelection); // Устанавливаем режим выделения лишь одно строки в таблице
    ui->tableView_questions->horizontalHeader()->setStretchLastSection(true);
    ui->tableView_questions->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents); // по содержимому

    // настриаваем маписн на поля редактирования вопроса
    mapper->setModel(modelQuestions);
    mapper->addMapping(ui->lineEdit_ID, 0);
    mapper->addMapping(ui->lineEdit_Question, 1);
    mapper->addMapping(ui->checkBox_some_answers , 2);
    mapper->addMapping(ui->checkBox_be_empty, 3);
    mapper->addMapping(ui->checkBox_satisfaction, 4);
    mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);



    //Таблица ответов
    modelAnswers->setTable("answers");
    // названия колонок
    modelAnswers->setHeaderData(1,Qt::Horizontal,"Вопрос");
    modelAnswers->setHeaderData(2,Qt::Horizontal,"Ответ");
    ui->tableView_answers->setModel(modelAnswers);
    ui->tableView_answers->setColumnHidden(0, true);    // Скрываем колонку с id записей
    ui->tableView_answers->setColumnHidden(1, true);
    //ui->tableView_answers->setEditTriggers(QAbstractItemView::NoEditTriggers);  //запрет редактирования
    ui->tableView_answers->setSelectionBehavior(QAbstractItemView::SelectRows); // Разрешаем выделение строк
    ui->tableView_answers->setSelectionMode(QAbstractItemView::SingleSelection); // Устанавливаем режим выделения лишь одно строки в таблице
    ui->tableView_answers->horizontalHeader()->setStretchLastSection(true);
    ui->tableView_answers->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents); // по содержимому

}

void FormEditQuestions::on_tableView_questions_clicked(const QModelIndex &index)
{
    // настраиваем фильтр ответов в зависимости от выбранного вопроса
//    QString ff = QString(" question_id = \%1 ").arg(modelQuestions->data(modelQuestions->index(index.row(), 0)).toString());

//    modelAnswers->setFilter(ff);
//    modelAnswers->select();
}

void FormEditQuestions::slotSelectionChange(const QItemSelection &current, const QItemSelection &previous)
{
    // настраиваем фильтр ответов в зависимости от выбранного вопроса
    QString ff = QString(" question_id = \%1 ").arg(modelQuestions->data(modelQuestions->index(ui->tableView_questions->currentIndex().row(), 0)).toString());

    modelAnswers->setFilter(ff);
    modelAnswers->select();

    // сбрасываем чекбоксы - сам не сбрасывает - хз почему?
//    ui->checkBox_some_answers->setChecked(false);
//    ui->checkBox_be_empty->setChecked(false);
//    ui->checkBox_satisfaction->setChecked(false);

    // при изменение строки в таблвьюве устанавливаем маппер на соответствующую запись
    mapper->setCurrentIndex(ui->tableView_questions->currentIndex().row());

}

void FormEditQuestions::on_pushButton_QAdd_clicked()
{
    // добавление запись
    // определяем количество записей
    int row=modelQuestions->rowCount();
    // вставляем следующую
    modelQuestions->insertRow(row);
    modelQuestions->submitAll();
    // устанавливаем курсор на строку редактирования
    ui->tableView_questions->selectRow(row);
    // устанавливаем маппер на ту же запись
    //mapper->toLast();
    // устанавливаем курсор на редактирование имени
    ui->lineEdit_Question->setFocus();
}

void FormEditQuestions::on_pushButton_QDel_clicked()
{
    // удаление вопроса

    if(QMessageBox::Yes != QMessageBox::question(this, tr("Внимание!"),
                                                 tr("Уверены в удалении вопроса с ответами?")))  return;

    // запрос на очистку ответов
    QSqlQuery query = QSqlQuery(base);
    query.prepare(" DELETE FROM answers WHERE question_id = :id");
    query.bindValue(":id",modelQuestions->data(modelQuestions->index(ui->tableView_questions->currentIndex().row(), 0)).toString());
    if (!query.exec())
        qDebug() << "Ошибка удаления ответов: " << query.lastError().text();

    // удаляем сам вопрос
    modelQuestions->removeRow(ui->tableView_questions->currentIndex().row());
    // прыгаем на предыдущую запись
    if (ui->tableView_questions->currentIndex().row() == 0 )
        modelAnswers->select();

    ui->tableView_questions->selectRow(ui->tableView_questions->currentIndex().row()-1);
}

void FormEditQuestions::on_pushButton_AAdd_clicked()
{


    // добапвление ответа

    // обновляем вопросы
    modelQuestions->submitAll();

    // определяем количество записей
    int row=modelAnswers->rowCount();
    // вставляем следующую
    modelAnswers->insertRow(row);
    modelAnswers->submitAll();
    // устанавливаем курсор на строку редактирования
    ui->tableView_answers->selectRow(row);

    // вставдяем номер вопроса
    int tt= modelQuestions->data(modelQuestions->index(ui->tableView_questions->currentIndex().row(), 0)).toInt();
    modelAnswers->setData(modelAnswers->index(ui->tableView_answers->currentIndex().row(), 1) ,tt);


}

void FormEditQuestions::on_pushButton_ADel_clicked()
{
    // удаление ответа

    if(QMessageBox::Yes != QMessageBox::question(this, tr("Внимание!"),
                                                 tr("Уверены в удалении ответа?")))  return;

    modelAnswers->removeRow(ui->tableView_answers->currentIndex().row());
    // прыгаем на предыдущую запись
    ui->tableView_questions->selectRow(ui->tableView_answers->currentIndex().row()-1);
}

void FormEditQuestions::on_pushButton_First_clicked()
{
    // прыгаем на первую
    ui->tableView_questions->selectRow(0);

}

void FormEditQuestions::on_pushButton_Previous_clicked()
{
    // прыгаем на предыдущую запись
    ui->tableView_questions->selectRow(ui->tableView_questions->currentIndex().row()-1);

}

void FormEditQuestions::on_pushButton_Next_clicked()
{
    // прыгаем на следующую запись
    ui->tableView_questions->selectRow(ui->tableView_questions->currentIndex().row()+1);

}

void FormEditQuestions::on_pushButton_Last_clicked()
{
    // последняя запись
    ui->tableView_questions->selectRow(modelQuestions->rowCount()-1);

}

void FormEditQuestions::on_tableView_questions_doubleClicked(const QModelIndex &index)
{
    // устанавливаем курсор на редактирование имени
    ui->lineEdit_Question->setFocus();

}
