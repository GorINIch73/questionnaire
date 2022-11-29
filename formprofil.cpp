<<<<<<< HEAD
#include "formprofil.h"
#include "ui_formprofil.h"
#include <QMessageBox>


FormProfil::FormProfil(QSqlDatabase db,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormProfil)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_DeleteOnClose);

    base=db;

   model = new QSqlTableModel(this,base);
//    delegate = new QSqlRelationalDelegate(this);

    //Настраиваем модели
    SetupTable();

    model->select();

    ui->tableView->selectRow(0);
}

FormProfil::~FormProfil()
{
    delete ui;
}

void FormProfil::SetupTable()
{
    //Таблица
    model->setTable("profil");
    // названия колонок
    model->setHeaderData(1,Qt::Horizontal,"Наименование профиля");
    ui->tableView->setModel(model);
    //ui->tableView->setItemDelegate(delegate);
    ui->tableView->setColumnHidden(0, true);    // Скрываем колонку с id записей
//    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);  //запрет редактирования
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows); // Разрешаем выделение строк
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection); // Устанавливаем режим выделения лишь одно строки в таблице
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents); // по содержимому


}

void FormProfil::on_pushButton_First_clicked()
{
    // прыгаем на первую
    ui->tableView->selectRow(0);
}

void FormProfil::on_pushButton_Prev_clicked()
{
    // прыгаем на предыдущую запись
    ui->tableView->selectRow(ui->tableView->currentIndex().row()-1);
}

void FormProfil::on_pushButton_Close_clicked()
{
       close();
}

void FormProfil::on_pushButton_Next_clicked()
{
    // прыгаем на следующую запись
    ui->tableView->selectRow(ui->tableView->currentIndex().row()+1);
}

void FormProfil::on_pushButton_Last_clicked()
{
    // последняя запись
    ui->tableView->selectRow(model->rowCount()-1);

}

void FormProfil::on_pushButton_Re_clicked()
{
    SetupTable();

    model->select();

    ui->tableView->selectRow(0);
}

void FormProfil::on_pushButton_Add_clicked()
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

void FormProfil::on_pushButton_Del_clicked()
{
    // удаление - нет проверки на свзи!
    if(QMessageBox::Yes != QMessageBox::question(this, tr("Внимание!"),
                                                 tr("Уверены в удалении?")))  return;
    model->removeRow(ui->tableView->currentIndex().row());
}
=======
#include "formprofil.h"
#include "ui_formprofil.h"
#include <QMessageBox>


FormProfil::FormProfil(QSqlDatabase db,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormProfil)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_DeleteOnClose);

    base=db;

   model = new QSqlTableModel(this,base);
//    delegate = new QSqlRelationalDelegate(this);

    //Настраиваем модели
    SetupTable();

    model->select();

    ui->tableView->selectRow(0);
}

FormProfil::~FormProfil()
{
    delete ui;
}

void FormProfil::SetupTable()
{
    //Таблица
    model->setTable("profil");
    // названия колонок
    model->setHeaderData(1,Qt::Horizontal,"Наименование профиля");
    ui->tableView->setModel(model);
    //ui->tableView->setItemDelegate(delegate);
    ui->tableView->setColumnHidden(0, true);    // Скрываем колонку с id записей
//    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);  //запрет редактирования
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows); // Разрешаем выделение строк
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection); // Устанавливаем режим выделения лишь одно строки в таблице
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents); // по содержимому


}

void FormProfil::on_pushButton_First_clicked()
{
    // прыгаем на первую
    ui->tableView->selectRow(0);
}

void FormProfil::on_pushButton_Prev_clicked()
{
    // прыгаем на предыдущую запись
    ui->tableView->selectRow(ui->tableView->currentIndex().row()-1);
}

void FormProfil::on_pushButton_Close_clicked()
{
       close();
}

void FormProfil::on_pushButton_Next_clicked()
{
    // прыгаем на следующую запись
    ui->tableView->selectRow(ui->tableView->currentIndex().row()+1);
}

void FormProfil::on_pushButton_Last_clicked()
{
    // последняя запись
    ui->tableView->selectRow(model->rowCount()-1);

}

void FormProfil::on_pushButton_Re_clicked()
{
    SetupTable();

    model->select();

    ui->tableView->selectRow(0);
}

void FormProfil::on_pushButton_Add_clicked()
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

void FormProfil::on_pushButton_Del_clicked()
{
    // удаление - нет проверки на свзи!
    if(QMessageBox::Yes != QMessageBox::question(this, tr("Внимание!"),
                                                 tr("Уверены в удалении?")))  return;
    model->removeRow(ui->tableView->currentIndex().row());
}
>>>>>>> 4eb8f4bbf917be0f1294fe3d8855518c914cbe29
