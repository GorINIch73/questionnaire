#ifndef FORMQUESTIONNAIRE_H
#define FORMQUESTIONNAIRE_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QSqlRelationalTableModel>
#include <QSqlRelationalDelegate>
#include <QDataWidgetMapper>

#include "qsqlrelationaldelegateflt.h"

namespace Ui {
class FormQuestionnaire;
}

class FormQuestionnaire : public QWidget
{
    Q_OBJECT

public:
    explicit FormQuestionnaire(QSqlDatabase db,QWidget *parent = nullptr);
    ~FormQuestionnaire();

private slots:
    void on_pushButtonClose_clicked();

    void on_pushButton_Add_clicked();

    void on_pushButton_Del_clicked();

    void on_pushButton_First_clicked();

    void on_pushButton_Prev_clicked();

    void on_pushButton_Next_clicked();

    void on_pushButton_Last_clicked();

    void on_pushButton_Refr_clicked();

    void on_comboBox_Place_currentIndexChanged(int index);

private:
    Ui::FormQuestionnaire *ui;

    QSqlDatabase base;
    QSqlRelationalTableModel *modelQuestionnaire;
    QSqlRelationalTableModel *modelAnswers_data;
    QDataWidgetMapper *mapper;
    QSqlRelationalDelegate *delegate;
    QSqlRelationalDelegateFlt *a_delegate;

    void SetupTable();
};

#endif // FORMQUESTIONNAIRE_H
