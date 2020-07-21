#include "formquestionnaire.h"
#include "ui_formquestionnaire.h"

#include <QMessageBox>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <QSqlRecord>


FormQuestionnaire::FormQuestionnaire(QSqlDatabase db,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormQuestionnaire)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    base=db;

    modelQuestionnaire = new QSqlRelationalTableModel(this,base);
    modelAnswers_data = new QSqlRelationalTableModel(this,base);
    delegate = new QSqlRelationalDelegate(this);
    a_delegate = new QSqlRelationalDelegateFlt(this);
    mapper = new QDataWidgetMapper(this);

    //Настраиваем модели
    SetupTable();

    modelQuestionnaire->select();
    modelAnswers_data->select();

    //изменение анкеты
    //connect(ui-> myTableView->selectionModel(), &QItemSelectionModel::currentRowChanged, mapper, &QDataWidgetMapper::setCurrentModelIndex);
    //connect(mapper->sele ui->tableView_questions->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), SLOT(slotSelectionChange(const QItemSelection &, const QItemSelection &)));


    mapper->toLast();
    modelAnswers_data->setFilter(QString("questionnaire_id = \%1 ").arg(modelQuestionnaire->data(modelQuestionnaire->index(mapper->currentIndex(), 0)).toString()));
    modelAnswers_data->select();

    int count = modelAnswers_data->rowCount();
    qDebug() << "count: " << count;
    for(int i=0;i < count; i++)
    {
        qDebug() << modelAnswers_data->record(i);
    }
}

FormQuestionnaire::~FormQuestionnaire()
{
    delete ui;
}

void FormQuestionnaire::on_pushButtonClose_clicked()
{
    //this->parentWidget()->close();
    close();
}

void FormQuestionnaire::SetupTable()
{
    //Таблица анкет
    modelQuestionnaire->setTable("questionnaire");
    modelQuestionnaire->setJoinMode(QSqlRelationalTableModel::LeftJoin); // что бы были видны пустые
    modelQuestionnaire->setRelation(modelQuestionnaire->fieldIndex("place_id"), QSqlRelation("place", "id", "name, place_id")); // дополнительное поле индекса

    // названия колонок
    //modelQuestionnaire->setHeaderData(1,Qt::Horizontal,"Вопрос");
    ui->comboBox_Place->setModel(modelQuestionnaire->relationModel(modelQuestionnaire->fieldIndex("place_id")));
    ui->comboBox_Place->setModelColumn(modelQuestionnaire->fieldIndex("place_id"));
    //ui->comboBox_Place->

    // настриаваем маписн на поля редактирования вопроса
    mapper->setModel(modelQuestionnaire);
    mapper->setItemDelegate(delegate);
    mapper->addMapping(ui->lineEdit_ID, modelQuestionnaire->fieldIndex("id"));
    mapper->addMapping(ui->comboBox_Place, modelQuestionnaire->fieldIndex("place_id"));
    mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);

    //ui->tableView->setItemDelegate(delegate);

    //Таблица данных ответов
    modelAnswers_data->setTable("answers_data");
    modelAnswers_data->setJoinMode(QSqlRelationalTableModel::LeftJoin); // что бы были видны пустые
//    modelAnswers_data->setRelation(2, QSqlRelation("questions", "id", "question"));
//    qDebug() << modelAnswers_data->fieldIndex("answer_id");
//    modelAnswers_data->setRelation(modelAnswers_data->fieldIndex("question_id"), QSqlRelation("questions", "id", "question, answers_data.question_id")); // дополнительное поле индекса
    modelAnswers_data->setRelation(modelAnswers_data->fieldIndex("question_id"), QSqlRelation("questions", "id", "question")); // дополнительное поле индекса
    // путает колонки хз что делать -----------------------------------------------------------------------------
//    modelAnswers_data->setRelation(modelAnswers_data->fieldIndex("answer_id"), QSqlRelation("answers", "id", "answer")); // не сдвигается номер поля хз почему 3=4
    modelAnswers_data->setRelation(3, QSqlRelation("answers", "id", "answer, answers_data.question_id")); // дополнительное поле индекса вопроса в конец, что бы небыло путаницы номеров полей
//    modelAnswers_data->setRelation(4, QSqlRelation("answers", "id", "answer")); // не сдвигается номер поля хз почему 3=4


    // названия колонок
//    modelAnswers_data->setHeaderData(modelAnswers_data->fieldIndex("questions.question"),Qt::Horizontal,"Вопрос");
//    modelAnswers_data->setHeaderData(modelAnswers_data->fieldIndex("answers.answer"),Qt::Horizontal,"Ответ");
    modelAnswers_data->setHeaderData(2,Qt::Horizontal,"Вопрос");
    modelAnswers_data->setHeaderData(3,Qt::Horizontal,"Ответ");
    ui->tableView->setModel(modelAnswers_data);
    ui->tableView->setItemDelegate(a_delegate);

   // ui->tableView_questions->setColumnHidden(0, true);    // Скрываем колонку с id записей
    //ui->tableView_answers->setEditTriggers(QAbstractItemView::NoEditTriggers);  //запрет редактирования
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows); // Разрешаем выделение строк
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection); // Устанавливаем режим выделения лишь одно строки в таблице
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents); // по содержимому

}

void FormQuestionnaire::on_pushButton_Add_clicked()
{

    modelQuestionnaire->submitAll(); // очень медленно!
    // запомнить прошлое место проведения ID
    // переделать на попроще
    QSqlQuery i_query = QSqlQuery(base);
    // проверка на первую пусое место проведения
//    qDebug() << modelQuestionnaire->fieldIndex("place_id");
//    qDebug() << modelQuestionnaire->fieldIndex("name");

    QString iid= modelQuestionnaire->data(modelQuestionnaire->index(mapper->currentIndex(),modelQuestionnaire->fieldIndex("name"))).toString();

    //---------------------------------------------------------------------------------
// по старому
//    if (!iid.isEmpty()) {
//        i_query.prepare("SELECT id FROM place WHERE name=:n");
//        i_query.bindValue(":n",iid);
//    }
//    else
//        i_query.prepare("SELECT id FROM place"); // иначе принудительно любое место

//    if (!i_query.exec())
//        qDebug() << "Ошибка получения перечня места: " << i_query.lastError().text();

//    i_query.first();
//    plc = i_query.value(0).toString();

    QString plc = modelQuestionnaire->record(mapper->currentIndex()).value(modelQuestionnaire->fieldIndex("place_id")).toString();
    if (iid.isEmpty()) {
        i_query.prepare("SELECT id FROM place"); // иначе принудительно любое место
        if (!i_query.exec())
            qDebug() << "Ошибка получения перечня места: " << i_query.lastError().text();
        i_query.first();
        plc = i_query.value(0).toString();
    }

    //добавляем анкету
    // определяем количество записей
    int row=modelQuestionnaire->rowCount();
    // вставляем следующую
    modelQuestionnaire->insertRow(row);

    //записывем предыдущее место проведения
    modelQuestionnaire->setData(modelQuestionnaire->index(row,modelQuestionnaire->fieldIndex("name")),plc);

//    mapper->submit();
    modelQuestionnaire->submit();

    mapper->setCurrentIndex(row);

    //mapper->toLast();
//   return;
    //добавляем в данные по ответам все вопросы

    // запрос на получение перечня вопросов
    QSqlQuery q_query = QSqlQuery(base);
    q_query.prepare("SELECT id FROM questions");
    if (!q_query.exec())
        qDebug() << "Ошибка получения перечня вопросов: " << q_query.lastError().text();

    //вставка перечня вопросов в ответы
    QSqlQuery query = QSqlQuery(base);
    QString sq = "INSERT INTO answers_data(questionnaire_id, question_id) values ";
    while (q_query.next()) {
        sq.append("(");
        sq.append(modelQuestionnaire->data(modelQuestionnaire->index(mapper->currentIndex(),modelQuestionnaire->fieldIndex("id"))).toString());
        sq.append(",");
        sq.append(q_query.value(0).toString());
        sq.append("),");
    }
    sq.chop(1);
    sq.append(";");
    //qDebug() << sq;
    if (!query.exec(sq))
        qDebug() << "Ошибка формирования перечня вопросов: " << query.lastError().text();

    // обновить
    modelAnswers_data->setFilter(QString("questionnaire_id = \%1 ").arg(modelQuestionnaire->data(modelQuestionnaire->index(mapper->currentIndex(), modelQuestionnaire->fieldIndex("id"))).toString()));
    modelAnswers_data->select();

    // устанавливаем курсор на строку редактирования
    ui->tableView->selectRow(0);

}

void FormQuestionnaire::on_pushButton_Del_clicked()
{

}

void FormQuestionnaire::on_pushButton_First_clicked()
{
    // прыгаем на первую
    //mapper->setCurrentIndex(0);
    mapper->toFirst();
    modelAnswers_data->setFilter(QString("questionnaire_id = \%1 ").arg(modelQuestionnaire->data(modelQuestionnaire->index(mapper->currentIndex(), 0)).toString()));
    modelAnswers_data->select();


}

void FormQuestionnaire::on_pushButton_Prev_clicked()
{
    // прыгаем на предыдущую запись
//    int row=mapper->currentIndex();
//    if (row > 0) {
//        mapper->setCurrentIndex(row-1);
//        qDebug() << row;
//     }
    mapper->toPrevious();
    modelAnswers_data->setFilter(QString("questionnaire_id = \%1 ").arg(modelQuestionnaire->data(modelQuestionnaire->index(mapper->currentIndex(), 0)).toString()));
    modelAnswers_data->select();

}

void FormQuestionnaire::on_pushButton_Next_clicked()
{
    // modelQuestionnaire->rowCount();

//    int row=mapper->currentIndex();
//    if (row < modelQuestionnaire->rowCount()-1)
//        mapper->setCurrentIndex(row+1);
    mapper->toNext();

//    QString ff = QString("questionnaire_id = \%1 ").arg(modelQuestionnaire->data(modelQuestionnaire->index(mapper->currentIndex(), 0)).toString());
    modelAnswers_data->setFilter(QString("questionnaire_id = \%1 ").arg(modelQuestionnaire->data(modelQuestionnaire->index(mapper->currentIndex(), 0)).toString()));
    modelAnswers_data->select();

}

void FormQuestionnaire::on_pushButton_Last_clicked()
{
//    int row=modelQuestionnaire->rowCount();
//    mapper->setCurrentIndex(row-1);
       mapper->toLast();
       modelAnswers_data->setFilter(QString("questionnaire_id = \%1 ").arg(modelQuestionnaire->data(modelQuestionnaire->index(mapper->currentIndex(), 0)).toString()));
       modelAnswers_data->select();

}

void FormQuestionnaire::on_pushButton_Refr_clicked()
{
    // сохраняем курсор
    int row = mapper->currentIndex();
    // обновляем предстваления
    SetupTable();
    modelQuestionnaire->select();
    modelAnswers_data->select();

    // восстанавливаем курсор
    mapper->setCurrentIndex(row);
    modelAnswers_data->setFilter(QString("questionnaire_id = \%1 ").arg(modelQuestionnaire->data(modelQuestionnaire->index(mapper->currentIndex(), 0)).toString()));
    modelAnswers_data->select();


}

void FormQuestionnaire::on_comboBox_Place_currentIndexChanged(int index)
{
    //иначе не сохраняет
    mapper->submit();
    modelQuestionnaire->submit();
}
