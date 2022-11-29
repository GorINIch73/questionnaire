#ifndef FORMSETTINGS_H
#define FORMSETTINGS_H

#include <QDialog>
#include <QSqlDatabase>
#include <QSqlTableModel>

namespace Ui {
class FormSettings;
}

class FormSettings : public QWidget
{
    Q_OBJECT

public:
    explicit FormSettings(QSqlDatabase db, QWidget *parent = nullptr);
    ~FormSettings();

private slots:
    void on_pushButtonClose_clicked();


    void on_pushButton_clicked();


private:
    Ui::FormSettings *ui;

    QSqlDatabase base;
    QSqlTableModel *model;

    void SetupTable();

};

#endif // FORMSETTINGS_H
