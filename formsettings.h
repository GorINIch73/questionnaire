#ifndef FORMSETTINGS_H
#define FORMSETTINGS_H

#include <QDialog>

namespace Ui {
class FormSettings;
}

class FormSettings : public QDialog
{
    Q_OBJECT

public:
    explicit FormSettings(QWidget *parent = nullptr);
    ~FormSettings();

private slots:
    void on_pushButtonClose_clicked();

    void on_pushButton_File_clicked();

    void on_pushButton_clicked();

    void on_pushButton_Create_clicked();

    void on_pushButton_Dbl_clicked();

private:
    Ui::FormSettings *ui;
};

#endif // FORMSETTINGS_H
