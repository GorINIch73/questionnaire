#ifndef FORMEDITQUESTIONS_H
#define FORMEDITQUESTIONS_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QItemSelection>
#include <QDataWidgetMapper>

namespace Ui {
class FormEditQuestions;
}

class FormEditQuestions : public QWidget
{
    Q_OBJECT

public:
    explicit FormEditQuestions(QSqlDatabase db,QWidget *parent = nullptr);
    ~FormEditQuestions();

private slots:
    void on_pushButtonClose_clicked();

    void on_tableView_questions_clicked(const QModelIndex &index);

    void slotSelectionChange(const QItemSelection &current, const QItemSelection &previous);

    void on_pushButton_QAdd_clicked();

    void on_pushButton_QDel_clicked();

    void on_pushButton_AAdd_clicked();

    void on_pushButton_ADel_clicked();

    void on_pushButton_First_clicked();

    void on_pushButton_Previous_clicked();

    void on_pushButton_Next_clicked();

    void on_pushButton_Last_clicked();

    void on_tableView_questions_doubleClicked(const QModelIndex &index);

private:
    Ui::FormEditQuestions *ui;

    QSqlDatabase base;
    QSqlTableModel *modelQuestions;
    QSqlTableModel *modelAnswers;
    QDataWidgetMapper *mapper;

    void SetupTable();
};

#endif // FORMEDITQUESTIONS_H
