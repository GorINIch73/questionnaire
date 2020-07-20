#ifndef FORMPLACE_H
#define FORMPLACE_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlRelationalTableModel>
#include <QSqlRelationalDelegate>

namespace Ui {
class FormPlace;
}

class FormPlace : public QWidget
{
    Q_OBJECT

public:
    explicit FormPlace(QSqlDatabase db,QWidget *parent = nullptr);
    ~FormPlace();

private slots:

    void on_pushButton_Close_clicked();

    void on_pushButton_Add_clicked();

    void on_pushButton_Del_clicked();

    void on_pushButton_First_clicked();

    void on_pushButton_Prev_clicked();

    void on_pushButton_Next_clicked();

    void on_pushButton_Last_clicked();

    void on_pushButton_Re_clicked();

private:
    Ui::FormPlace *ui;
    QSqlDatabase base;
    QSqlRelationalTableModel *model;
    QSqlRelationalDelegate *delegate;

    void SetupTable();

};

#endif // FORMPLACE_H
