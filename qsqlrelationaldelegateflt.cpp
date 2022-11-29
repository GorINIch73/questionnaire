<<<<<<< HEAD
#include "qsqlrelationaldelegateflt.h"

#include <QCompleter>
#include <QtDebug>
#include <QSqlRecord>
#include <QSqlField>
#include <QLineEdit>


QSqlRelationalDelegateFlt::QSqlRelationalDelegateFlt(QObject * parent) : QSqlRelationalDelegate(parent)
{

}

QWidget *QSqlRelationalDelegateFlt::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{

    // сделано специализировано для одной колонки
    QString field = "answer"; //имя поля с ответом для комбобокса
    int field_id = 4; // номер поля вопроса для фильта - имя не срабатывает так как поле дополнительное

    // выковыриваем модель
    const QSqlRelationalTableModel *sqlModel = qobject_cast<const QSqlRelationalTableModel *>(index.model());

    // выковыриваем связанную модель
    QSqlTableModel *childModel = sqlModel ? sqlModel->relationModel(index.column()) : nullptr;

    // магия только для колонки с вопросами, остальным null
    if (index.column()!=sqlModel->fieldIndex(field))
        return nullptr;
//        return QSqlRelationalDelegate::createEditor(parent, option, index);

    // настраиваем комбобокс
    QComboBox *combo = new QComboBox(parent);
    combo->setModel(childModel);
    combo->setModelColumn(childModel->fieldIndex(field));
    // получить id вопроса
    //qDebug() <<childModel->fieldIndex("answers_data.question_id");
    QString qss= sqlModel->record(index.row()).value(field_id).toString();
    //установить фильтер
    childModel->setFilter(QString("question_id = \%1 ").arg(qss));

    combo->setEditable(true);

    // настраиваем комплитер
    QCompleter *mycompletear = new QCompleter(parent);
    mycompletear->setCaseSensitivity(Qt::CaseInsensitive);
    //mycompletear->setFilterMode(Qt::MatchContains);
    mycompletear->setCompletionMode(QCompleter::InlineCompletion);
    mycompletear->setModel(childModel);
    mycompletear->setCompletionColumn(childModel->fieldIndex(field)); // номер колонки с данными подстановки
    combo->setCompleter(mycompletear);

    // выделить текст
//    combo->lineEdit()->selectAll();  не вышло!

    return combo;

}


=======
#include "qsqlrelationaldelegateflt.h"

#include <QCompleter>
#include <QtDebug>
#include <QSqlRecord>
#include <QSqlField>
#include <QLineEdit>


QSqlRelationalDelegateFlt::QSqlRelationalDelegateFlt(QObject * parent) : QSqlRelationalDelegate(parent)
{

}

QWidget *QSqlRelationalDelegateFlt::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{

    // сделано специализировано для одной колонки
    QString field = "answer"; //имя поля с ответом для комбобокса
    int field_id = 4; // номер поля вопроса для фильта - имя не срабатывает так как поле дополнительное

    // выковыриваем модель
    const QSqlRelationalTableModel *sqlModel = qobject_cast<const QSqlRelationalTableModel *>(index.model());

    // выковыриваем связанную модель
    QSqlTableModel *childModel = sqlModel ? sqlModel->relationModel(index.column()) : nullptr;

    // магия только для колонки с вопросами, остальным null
    if (index.column()!=sqlModel->fieldIndex(field))
        return nullptr;
//        return QSqlRelationalDelegate::createEditor(parent, option, index);

    // настраиваем комбобокс
    QComboBox *combo = new QComboBox(parent);
    combo->setModel(childModel);
    combo->setModelColumn(childModel->fieldIndex(field));
    // получить id вопроса
    //qDebug() <<childModel->fieldIndex("answers_data.question_id");
    QString qss= sqlModel->record(index.row()).value(field_id).toString();
    //установить фильтер
    childModel->setFilter(QString("question_id = \%1 ").arg(qss));

    combo->setEditable(true);

    // настраиваем комплитер
    QCompleter *mycompletear = new QCompleter(parent);
    mycompletear->setCaseSensitivity(Qt::CaseInsensitive);
    //mycompletear->setFilterMode(Qt::MatchContains);
    mycompletear->setCompletionMode(QCompleter::InlineCompletion);
    mycompletear->setModel(childModel);
    mycompletear->setCompletionColumn(childModel->fieldIndex(field)); // номер колонки с данными подстановки
    combo->setCompleter(mycompletear);

    // выделить текст
//    combo->lineEdit()->selectAll();  не вышло!

    return combo;

}


>>>>>>> 4eb8f4bbf917be0f1294fe3d8855518c914cbe29
