#include "qsqlrelationaldelegateflt.h"

#include <QCompleter>
#include <QtDebug>
#include <QSqlRecord>
#include <QSqlField>

QSqlRelationalDelegateFlt::QSqlRelationalDelegateFlt(QObject * parent) : QSqlRelationalDelegate(parent)
{

}

QWidget *QSqlRelationalDelegateFlt::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{

    // стащил из родителя
    // выковыриваем модель
    const QSqlRelationalTableModel *sqlModel = qobject_cast<const QSqlRelationalTableModel *>(index.model());

    // выковыриваем связанную модель
    QSqlTableModel *childModel = sqlModel ? sqlModel->relationModel(index.column()) : nullptr;

    // связь с вопросом - дает ссылку да полную запись строки - хз почему работает?
    QSqlTableModel *childModelQ = sqlModel ? sqlModel->relationModel(sqlModel->fieldIndex("question")) : nullptr;
    // не понятно почему работает, но используем это что бы получить ID подставляемого значения
    //qDebug() << "main index: " << index.row();
    //qDebug() << "индекс поля вопроса " << sqlModel->fieldIndex("question");
    //запись для вопроса для текущей строки
    //QSqlRecord rrZ = childModelQ->record(index.row());
    //qDebug() << "record:"<< rrZ;
    //qDebug() << "record: 0 : "<< rrZ.value(0).toString();
    // ID вопроса
    // не понятно почему работает, но используем это что бы получить ID подставляемого значения
    //QString qss= childModelQ->record(index.row()).value(0).toString();

    if (!childModel)
        return QSqlRelationalDelegate::createEditor(parent, option, index);

    const QSqlDriver *const driver = childModel->database().driver();

    // настраиваем комбобокс
    QComboBox *combo = new QComboBox(parent);
    combo->setModel(childModel);
    // получаем номер колонки
    QString fn = sqlModel->relation(index.column()).displayColumn();
    QString stripped = driver->isIdentifierEscaped(fn, QSqlDriver::FieldName) // имя связанного поля
            ? driver->stripDelimiters(fn, QSqlDriver::FieldName)
            : fn;

    //qDebug() << fn;
    //qDebug() << stripped;  // имя связанного поля

    combo->setModelColumn(childModel->fieldIndex(stripped));
    // надо настроить фильтр только для колонки с ответами
    if (index.column()==sqlModel->fieldIndex("answer")) {
        // получить id вопроса
        QString qss= childModelQ->record(index.row()).value(0).toString();
        //установить фильтер
        childModel->setFilter(QString("question_id = \%1 ").arg(qss));
        //childModel->select();
    }


    //настраиваем редактирование - врнмненно
    combo->setEnabled(false);
    if (index.column()==sqlModel->fieldIndex("answer")) {
        combo->setEnabled(true);
        combo->setEditable(true);
    }
    // настраиваем комплитер
    QCompleter *mycompletear = new QCompleter(parent);
    mycompletear->setCaseSensitivity(Qt::CaseInsensitive);
    mycompletear->setFilterMode(Qt::MatchContains);
    mycompletear->setModel(childModel);
    mycompletear->setCompletionColumn(childModel->fieldIndex(stripped)); // номер колонки с данными подстановки
    combo->setCompleter(mycompletear);


    return combo;

}


