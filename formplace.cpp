#include "formplace.h"
#include "ui_formplace.h"
#include <QMessageBox>

FormPlace::FormPlace(QSqlDatabase db,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormPlace)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    base=db;

    model = new QSqlRelationalTableModel(this,base);
    delegate = new QSqlRelationalDelegate(this);

    //Настраиваем модели
    SetupTable();

    model->select();

    ui->tableView->selectRow(0);
}

FormPlace::~FormPlace()
{
    delete ui;
}

void FormPlace::on_pushButton_Close_clicked()
{
    close();
}

void FormPlace::SetupTable()
{
    //Таблица
    model->setTable("place");
    model->setRelation(2, QSqlRelation("region", "id", "name"));
    // названия колонок
    model->setHeaderData(1,Qt::Horizontal,"Место проведения");
    model->setHeaderData(2,Qt::Horizontal,"Район");
    model->setHeaderData(3,Qt::Horizontal,"Для отчета");
    ui->tableView->setModel(model);
    ui->tableView->setItemDelegate(delegate);
   // ui->tableView->setColumnHidden(0, true);    // Скрываем колонку с id записей
//    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);  //запрет редактирования
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows); // Разрешаем выделение строк
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection); // Устанавливаем режим выделения лишь одно строки в таблице
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents); // по содержимому


}

void FormPlace::on_pushButton_Add_clicked()
{
    // добавление запись
    // определяем количество записей
    int row=model->rowCount();
    // вставляем следующую
    model->insertRow(row);
    model->submitAll();
    // устанавливаем курсор на строку редактирования
    ui->tableView->selectRow(row);
}

void FormPlace::on_pushButton_Del_clicked()
{
    // удаление
    if(QMessageBox::Yes != QMessageBox::question(this, tr("Внимание!"),
                                                 tr("Уверены в удалении?")))  return;

    model->removeRow(ui->tableView->currentIndex().row());
}

void FormPlace::on_pushButton_First_clicked()
{
    // прыгаем на первую
    ui->tableView->selectRow(0);

}

void FormPlace::on_pushButton_Prev_clicked()
{
    // прыгаем на предыдущую запись
    ui->tableView->selectRow(ui->tableView->currentIndex().row()-1);
}

void FormPlace::on_pushButton_Next_clicked()
{
    // прыгаем на следующую запись
    ui->tableView->selectRow(ui->tableView->currentIndex().row()+1);
}

void FormPlace::on_pushButton_Last_clicked()
{
    // последняя запись
    ui->tableView->selectRow(model->rowCount()-1);
}

void FormPlace::on_pushButton_Re_clicked()
{
    SetupTable();

    model->select();

    ui->tableView->selectRow(0);
}
