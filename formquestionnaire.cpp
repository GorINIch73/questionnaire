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

    //modelQuestionnaire->setFilter("questionnaire.id='250'");

    modelQuestionnaire->select();
    modelAnswers_data->select();

    //изменение анкеты
    //connect(ui-> myTableView->selectionModel(), &QItemSelectionModel::currentRowChanged, mapper, &QDataWidgetMapper::setCurrentModelIndex);
    //connect(mapper->sele ui->tableView_questions->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), SLOT(slotSelectionChange(const QItemSelection &, const QItemSelection &)));
    connect(ui->tableView->itemDelegate(),SIGNAL(closeEditor(QWidget*,QAbstractItemDelegate::EndEditHint)),SLOT(endEditSlot(QWidget*,QAbstractItemDelegate::EndEditHint)));

    // сигнал создания запроса во вкладках
    connect(this, SIGNAL(signalFromQuery(QString)),parent, SLOT(slot_goQuery(QString)));


    mapper->toLast();
    // обновить ответы
    TunAnswers_data();
    //ui->tableView->selectRow(0);
    ui->tableView->setFocus();

//    QModelIndex index = ui->tableView->model()->index(0, 3);
//    ui->tableView->selectionModel()->select(index, QItemSelectionModel::Select);
//    ui->tableView->setCurrentIndex(index);
//    ui->tableView->setFocus();
//    ui->tableView->edit(index);

//    int count = modelAnswers_data->rowCount();
//    qDebug() << "count: " << count;
//    for(int i=0;i < count; i++)
//    {
//        qDebug() << modelAnswers_data->record(i);
//    }
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
//    qDebug() << modelAnswers_data->fieldIndex("answer_id");
//    modelAnswers_data->setRelation(modelAnswers_data->fieldIndex("question_id"), QSqlRelation("questions", "id", "question, answers_data.question_id")); // дополнительное поле индекса - перенесено в конец
    modelAnswers_data->setRelation(modelAnswers_data->fieldIndex("question_id"), QSqlRelation("questions", "id", "question")); // дополнительное поле индекса
    // путает колонки хз что делать -----------------------------------------------------------------------------
    modelAnswers_data->setRelation(modelAnswers_data->fieldIndex("answer_id"), QSqlRelation("answers", "id", "answer, answers_data.question_id")); // дополнительное поле индекса вопроса в конец, что бы небыло путаницы номеров полей


    // названия колонок
    modelAnswers_data->setHeaderData(modelAnswers_data->fieldIndex("question"),Qt::Horizontal,"Вопрос");
    modelAnswers_data->setHeaderData(modelAnswers_data->fieldIndex("answer"),Qt::Horizontal,"Ответ");
    modelAnswers_data->setHeaderData(2,Qt::Horizontal,"Вопрос");
    modelAnswers_data->setHeaderData(3,Qt::Horizontal,"Ответ");
    ui->tableView->setModel(modelAnswers_data);
    ui->tableView->setItemDelegate(a_delegate);

    ui->tableView->setColumnHidden(0, true);    // Скрываем колонку с id записей
    ui->tableView->setColumnHidden(1, true);    // Скрываем колонку
    ui->tableView->setColumnHidden(99, true);    // Скрываем колонку  - не срабатывает ((
    //ui->tableView>setEditTriggers(QAbstractItemView::NoEditTriggers);  //запрет редактирования
    //ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows); // Разрешаем выделение строк
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection); // Устанавливаем режим выделения лишь одно строки в таблице
    //ui->tableView->horizontalHeader()->setStretchLastSection(true);
    //ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents); // по содержимому
    ui->tableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    //ui->tableView->setSelectionModel();


}

void FormQuestionnaire::TunAnswers_data()
{
    // настройка фильтра ответов по номеру анкеты
    modelAnswers_data->setFilter(QString("questionnaire_id = \%1 ").arg(modelQuestionnaire->data(modelQuestionnaire->index(mapper->currentIndex(), modelQuestionnaire->fieldIndex("id"))).toString()));
    modelAnswers_data->select();
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

    // запрос на получение перечня вопросов сортировка по написанию (номеру в начале) вопроса!
    QSqlQuery q_query = QSqlQuery(base);
    q_query.prepare("SELECT id FROM questions ORDER by questions.question");
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

    // обновить ответы
    TunAnswers_data();

    // устанавливаем курсор на строку редактирования
    ui->tableView->selectRow(0);

}

void FormQuestionnaire::on_pushButton_Del_clicked()
{
// удаление анкеты
    if(QMessageBox::Yes != QMessageBox::question(this, tr("Внимание!"),
                                                 tr("Уверены в удалении анкеты?")))  return;

    // удаляем принадлежащие анкете ответы
    QSqlQuery query = QSqlQuery(base);
    query.prepare("DELETE FROM answers_data WHERE questionnaire_id=:id");
    query.bindValue(":id",modelQuestionnaire->data(modelQuestionnaire->index(mapper->currentIndex(),modelQuestionnaire->fieldIndex("id"))).toString());
    if (!query.exec())
        qDebug() << "Ошибка удаления ответов: " << query.lastError().text();

// удаляем саму анкету
    int row=mapper->currentIndex();
    modelQuestionnaire->removeRow(row);
    modelQuestionnaire->select();

    //прыгаем на предыдущую запись
    mapper->setCurrentIndex(row-1);

    // обновить ответы
    TunAnswers_data();

}

void FormQuestionnaire::on_pushButton_First_clicked()
{
    // прыгаем на первую
    //mapper->setCurrentIndex(0);
    mapper->toFirst();
    // обновить ответы
    TunAnswers_data();

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
    // обновить ответы
    TunAnswers_data();

}

void FormQuestionnaire::on_pushButton_Next_clicked()
{
    // modelQuestionnaire->rowCount();

//    int row=mapper->currentIndex();
//    if (row < modelQuestionnaire->rowCount()-1)
//        mapper->setCurrentIndex(row+1);
    mapper->toNext();

//    QString ff = QString("questionnaire_id = \%1 ").arg(modelQuestionnaire->data(modelQuestionnaire->index(mapper->currentIndex(), 0)).toString());
    // обновить ответы
    TunAnswers_data();

}

void FormQuestionnaire::on_pushButton_Last_clicked()
{
//    int row=modelQuestionnaire->rowCount();
//    mapper->setCurrentIndex(row-1);
       mapper->toLast();
       // обновить ответы
       TunAnswers_data();

}

void FormQuestionnaire::on_pushButton_Refr_clicked()
{
    // сохраняем курсор
    int row = mapper->currentIndex();
    // обновляем предстваления
    SetupTable();
    modelQuestionnaire->select();
//    modelAnswers_data->select();

    // восстанавливаем курсор
    mapper->setCurrentIndex(row);
    // обновить ответы
    TunAnswers_data();


}

void FormQuestionnaire::on_comboBox_Place_currentIndexChanged(int index)
{
    //иначе не сохраняет
    mapper->submit();
    modelQuestionnaire->submit();
}

void FormQuestionnaire::endEditSlot(QWidget* w,QAbstractItemDelegate::EndEditHint hint)
{
    // обработка выхода из режима редактирования делегата таблицы
    int row = ui->tableView->currentIndex().row();
    int rowCount = ui->tableView->model()->rowCount();

//    qDebug() << "hint: " << hint;

    //qDebug() << row << "-" << ui->tableView->model()->rowCount();
    // прыгаем в режим редактирования только при подтверждении изменения значения
    if (row >= rowCount-1 || hint != QAbstractItemDelegate::SubmitModelCache)
        return;

    QModelIndex index = ui->tableView->model()->index(row+1, 3);
    ui->tableView->selectionModel()->select(index, QItemSelectionModel::Select);
    ui->tableView->setCurrentIndex(index);
    ui->tableView->setFocus();
    ui->tableView->edit(index);
//    ui->tableView->setEd

}

void FormQuestionnaire::on_lineEdit_Flt_textChanged(const QString &arg1)
{
    // фильтр по номеру анкеты
    if (!ui->lineEdit_Flt->text().isEmpty()) {

        QString ff = QString("questionnaire.id = \%1 ").arg(arg1);
//        QString ff = QString(" %1 Like '\%%2\%' ").arg("questionnaire.id", arg1);

        modelQuestionnaire->setFilter(ff);
        modelQuestionnaire->select();
        mapper->toFirst();
        // обновить ответы
        TunAnswers_data();
    }
    else {
        modelQuestionnaire->setFilter("");
        modelQuestionnaire->select();
        mapper->toFirst();
        // обновить ответы
        TunAnswers_data();
    }

}

void FormQuestionnaire::on_pushButton_ClrFlt_clicked()
{
    // очистка фильта
    ui->lineEdit_Flt->setText("");
}

void FormQuestionnaire::on_pushButton_erase_clicked()
{

    // ояистка данных ответов для анкеты
//        if(QMessageBox::Yes != QMessageBox::question(this, tr("Внимание!"),
//                                                     tr("Уверены в удалении анкеты?")))  return;
        QString sID = modelQuestionnaire->data(modelQuestionnaire->index(mapper->currentIndex(),modelQuestionnaire->fieldIndex("id"))).toString();

        // удаляем принадлежащие анкете ответы
        QSqlQuery query = QSqlQuery(base);
        QString sQuery = QString("UPDATE answers_data SET answer_id=NULL WHERE questionnaire_id=%1").arg(sID);
        qDebug() << "очистка ответов " << sQuery;

        if (!query.exec(sQuery))
            qDebug() << "Ошибка очистки ответов: " << query.lastError().text();

        // обновить ответы
        TunAnswers_data();

}


void FormQuestionnaire::on_pushButton_queryRep2_clicked()
{
    //запрос на свод по районам
    emit signalFromQuery("SELECT region.name, region.id, answers_data.question_id, questions.question, answer_id, answers.answer, count(answer_id) FROM answers_data  inner join questionnaire on answers_data.questionnaire_id=questionnaire.id inner join place on questionnaire.place_id=place.id inner join region on region.id=place.region_id inner join questions on answers_data.question_id = questions.id inner join answers on answer_id=answers.id group by answer_id, answers_data.question_id, region.id order by region.id, questions.question, answers.answer");

}


void FormQuestionnaire::on_pushButton_queryRep_clicked()
{
    //запрос на свод по местам оказания
//    emit signalFromQuery("SELECT place.name, profil.profil_name, questionnaire.place_id, answers_data.question_id, questions.question, answer_id, answers.answer, count(answer_id) FROM answers_data  inner join questionnaire on answers_data.questionnaire_id=questionnaire.id inner join place on questionnaire.place_id=place.id inner join profil on place.profil_id=profil.id inner join questions on answers_data.question_id = questions.id inner join answers on answer_id=answers.id group by answer_id, answers_data.question_id, questionnaire.place_id order by questionnaire.place_id, answers_data.question_id");
    emit signalFromQuery("SELECT place.name, profil.profil_name, questionnaire.place_id, answers_data.question_id, questions.question, answer_id, answers.answer, count(answer_id) FROM answers_data  inner join questionnaire on answers_data.questionnaire_id=questionnaire.id inner join place on questionnaire.place_id=place.id inner join profil on place.profil_id=profil.id inner join questions on answers_data.question_id = questions.id inner join answers on answer_id=answers.id group by answer_id, answers_data.question_id, questionnaire.place_id order by questionnaire.place_id, questions.question, answers.answer");

}

void FormQuestionnaire::on_pushButton_Rep_U_clicked()
{
    //запрос на свод по удовлетворенности с учетом группы профиля
//    emit signalFromQuery("SELECT region.name, profil.profil_name, count(questions.id) as count_questions, count(answers.satisfaction) FROM answers_data  inner join questionnaire on answers_data.questionnaire_id=questionnaire.id inner join place on questionnaire.place_id=place.id inner join region on place.region_id=region.id inner join profil on place.profil_id=profil.id inner join questions on answers_data.question_id = questions.id inner join answers on answer_id=answers.id WHERE questions.satisfaction = TRUE GROUP BY region.name, profil.profil_name");
    emit signalFromQuery("SELECT region.name, profil.profil_name, st_profile_name, count(questions.id) as count_questions, count(CASE WHEN answers.satisfaction = 1 THEN 1 END) FROM answers_data  inner join questionnaire on answers_data.questionnaire_id=questionnaire.id inner join place on questionnaire.place_id=place.id inner join region on place.region_id=region.id inner join profil on place.profil_id=profil.id inner join questions on answers_data.question_id = questions.id inner join answers on answer_id=answers.id inner join (SELECT answers_data.questionnaire_id as st_questionnaire_id, answers.answer as st_profile_name FROM answers_data  inner join questions on answers_data.question_id = questions.id inner join answers on answer_id=answers.id WHERE questions.profile_st = TRUE) on answers_data.questionnaire_id=st_questionnaire_id WHERE questions.satisfaction = TRUE GROUP BY region.name, profil.profil_name, st_profile_name");

}


void FormQuestionnaire::on_pushButton_Rep_US_clicked()
{
    //запрос на свод по удовлетворенности по профилю
   emit signalFromQuery("SELECT region.name, profil.profil_name, count(questions.id) as count_questions, count(CASE WHEN answers.satisfaction = 1 THEN 1 END) FROM answers_data  inner join questionnaire on answers_data.questionnaire_id=questionnaire.id inner join place on questionnaire.place_id=place.id inner join region on place.region_id=region.id inner join profil on place.profil_id=profil.id inner join questions on answers_data.question_id = questions.id inner join answers on answer_id=answers.id WHERE questions.satisfaction = TRUE GROUP BY region.name, profil.profil_name");

}

void FormQuestionnaire::on_pushButton_ErrS_clicked()
{
    // прочитать ID из настроек
    // 76 14 78
    QSqlQuery t_query = QSqlQuery(base);
    t_query.exec("SELECT option_data FROM setings WHERE option_name=\"ID вопроса СМП\""); // вопрос пользовались ли СМП 13
    t_query.next();
    QString ID_QCMP =t_query.value(0).toString();
    t_query.exec("SELECT option_data FROM setings WHERE option_name=\"ID ответа СМП\""); // ответ пользовались ли СМП  - НЕТ 76
    t_query.next();
    QString ID_ACMP =t_query.value(0).toString();
    t_query.exec("SELECT option_data FROM setings WHERE option_name=\"ID вопроса удовлетворенности СМП\""); // вопрос удовлетворенности СМП 14
    t_query.next();
    QString ID_QSCMP =t_query.value(0).toString();
    t_query.exec("SELECT option_data FROM setings WHERE option_name=\"ID ответа удовлетворенности СМП пусто\""); // - пусто - 78
    t_query.next();
    QString ID_ASCMP =t_query.value(0).toString();

    if (ID_QCMP.isEmpty() || ID_ACMP.isEmpty() || ID_QSCMP.isEmpty() || ID_ASCMP.isEmpty()) {

        QMessageBox::critical(this,"Ошибка","Недостаточно данных по ID вопросов!\nЗадайте ID вопросов и ответов по МСП в настройках!");
        return;
    }

    //запрос на ошибки в вопросе по МСП
//    emit signalFromQuery(QString("SELECT * FROM answers_data WHERE answers_data.questionnaire_id IN (SELECT answers_data.questionnaire_id FROM answers_data WHERE answers_data.answer_id=%1) AND answers_data.question_id=%2 AND NOT answers_data.answer_id=%3").arg(ID_AC).arg(ID_QC).arg(ID_SMP));
//    emit signalFromQuery(QString("SELECT questionnaire.id, qid13, qd13, ad13, qid14, qd14, ad14 FROM questionnaire left join (SELECT answers_data.questionnaire_id as qid13, answers_data.question_id as qd13, answers_data.answer_id as ad13 FROM answers_data WHERE qd13=13) on questionnaire.id=qid13 left join (SELECT answers_data.questionnaire_id as qid14, answers_data.question_id as qd14, answers_data.answer_id as ad14 FROM answers_data WHERE qd14=14) on questionnaire.id=qid14 WHERE (ad13=76 AND not ad14=78) OR (ad14=78 AND not ad13=76)"));
    emit signalFromQuery(QString("SELECT questionnaire.id, qd13, ad13, qd14, ad14 FROM questionnaire left join (SELECT answers_data.questionnaire_id as qid13, answers_data.question_id as qd13, answers_data.answer_id as ad13 FROM answers_data WHERE qd13=%1) on questionnaire.id=qid13 left join (SELECT answers_data.questionnaire_id as qid14, answers_data.question_id as qd14, answers_data.answer_id as ad14 FROM answers_data WHERE qd14=%3) on questionnaire.id=qid14 WHERE (ad13=%2 AND not ad14=%4) OR (ad14=%4 AND not ad13=%2)").arg(ID_QCMP).arg(ID_ACMP).arg(ID_QSCMP).arg(ID_ASCMP));

}

void FormQuestionnaire::on_pushButton_list_clicked()
{
    //запрос на список сортировака по анкетам
    emit signalFromQuery("SELECT questionnaire.id as \"номер анкеты\", place.name, profil.profil_name, questionnaire.place_id, answers_data.question_id, questions.question, answer_id, answers.answer FROM answers_data  inner join questionnaire on answers_data.questionnaire_id=questionnaire.id inner join place on questionnaire.place_id=place.id inner join profil on place.profil_id=profil.id inner join questions on answers_data.question_id = questions.id inner join answers on answer_id=answers.id order by questionnaire.id, questions.question, answers.answer");

}

void FormQuestionnaire::on_pushButton_list2_clicked()
{
    //запрос на список сортировака по вопросам
    emit signalFromQuery("SELECT place.name, profil.profil_name, questionnaire.place_id, questionnaire.id as \"номер анкеты\", answers_data.question_id, questions.question, answer_id, answers.answer FROM answers_data  inner join questionnaire on answers_data.questionnaire_id=questionnaire.id inner join place on questionnaire.place_id=place.id inner join profil on place.profil_id=profil.id inner join questions on answers_data.question_id = questions.id inner join answers on answer_id=answers.id order by place.name, questions.question, answers.answer");

}

void FormQuestionnaire::on_pushButton_null_clicked()
{
    //запрос на список анкет с пустыми ответами
    emit signalFromQuery("SELECT questionnaire_id, question_id  FROM answers_data WHERE answer_id IS NULL");
}




