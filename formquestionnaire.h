<<<<<<< HEAD
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

signals:

    void signalFromQuery(QString sq); // сигнал для запроса


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

    void endEditSlot(QWidget*,QAbstractItemDelegate::EndEditHint);

    void on_lineEdit_Flt_textChanged(const QString &arg1);

    void on_pushButton_ClrFlt_clicked();

    void on_pushButton_erase_clicked();

    void on_pushButton_queryRep_clicked();

    void on_pushButton_Rep_U_clicked();

    void on_pushButton_ErrS_clicked();

    void on_pushButton_list_clicked();

    void on_pushButton_list2_clicked();

    void on_pushButton_null_clicked();

    void on_pushButton_Rep_US_clicked();

    void on_pushButton_queryRep2_clicked();

private:
    Ui::FormQuestionnaire *ui;

    QSqlDatabase base;
    QSqlRelationalTableModel *modelQuestionnaire;
    QSqlRelationalTableModel *modelAnswers_data;
    QDataWidgetMapper *mapper;
    QSqlRelationalDelegate *delegate;
    QSqlRelationalDelegateFlt *a_delegate;

    void SetupTable();
    void TunAnswers_data();
};

#endif // FORMQUESTIONNAIRE_H
=======
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

signals:

    void signalFromQuery(QString sq); // сигнал для запроса


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

    void endEditSlot(QWidget*,QAbstractItemDelegate::EndEditHint);

    void on_lineEdit_Flt_textChanged(const QString &arg1);

    void on_pushButton_ClrFlt_clicked();

    void on_pushButton_erase_clicked();

    void on_pushButton_queryRep_clicked();

    void on_pushButton_Rep_U_clicked();

    void on_pushButton_ErrS_clicked();

    void on_pushButton_list_clicked();

    void on_pushButton_list2_clicked();

    void on_pushButton_null_clicked();

    void on_pushButton_Rep_US_clicked();

    void on_pushButton_queryRep2_clicked();

private:
    Ui::FormQuestionnaire *ui;

    QSqlDatabase base;
    QSqlRelationalTableModel *modelQuestionnaire;
    QSqlRelationalTableModel *modelAnswers_data;
    QDataWidgetMapper *mapper;
    QSqlRelationalDelegate *delegate;
    QSqlRelationalDelegateFlt *a_delegate;

    void SetupTable();
    void TunAnswers_data();
};

#endif // FORMQUESTIONNAIRE_H
>>>>>>> 4eb8f4bbf917be0f1294fe3d8855518c914cbe29
