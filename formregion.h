#ifndef FORMREGION_H
#define FORMREGION_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlTableModel>

namespace Ui {
class FormRegion;
}

class FormRegion : public QWidget
{
    Q_OBJECT

public:
    explicit FormRegion(QSqlDatabase db,QWidget *parent = nullptr);
    ~FormRegion();

private slots:
    void on_pushButtonClose_clicked();

    void on_pushButton_Add_clicked();

    void on_pushButton_Del_clicked();

    void on_pushButton_First_clicked();

    void on_pushButton_Prev_clicked();

    void on_pushButton_Next_clicked();

    void on_pushButton_Last_clicked();

    void on_pushButton_Re_clicked();

private:
    Ui::FormRegion *ui;

    QSqlDatabase base;
    QSqlTableModel *model;

    void SetupTable();
};

#endif // FORMREGION_H
