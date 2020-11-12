#ifndef FORMPROFIL_H
#define FORMPROFIL_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlTableModel>
//#include <QSqlRelationalTableModel>
//#include <QSqlRelationalDelegate>

namespace Ui {
class FormProfil;
}

class FormProfil : public QWidget
{
    Q_OBJECT

public:
    explicit FormProfil(QSqlDatabase db,QWidget *parent = nullptr);
    ~FormProfil();

private slots:
    void on_pushButton_First_clicked();

    void on_pushButton_Prev_clicked();

    void on_pushButton_Close_clicked();

    void on_pushButton_Next_clicked();

    void on_pushButton_Last_clicked();

    void on_pushButton_Re_clicked();

    void on_pushButton_Add_clicked();

    void on_pushButton_Del_clicked();

private:
    Ui::FormProfil *ui;
    QSqlDatabase base;
    QSqlTableModel *model;
    //QSqlRelationalDelegate *delegate;

    void SetupTable();
};

#endif // FORMPROFIL_H
