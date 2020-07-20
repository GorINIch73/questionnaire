#include "formsettings.h"
#include "ui_formsettings.h"
#include "defining.h"

#include <QSettings>
#include <QFileDialog>

FormSettings::FormSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormSettings)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    // считать из настроек имя базы

    QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
    ui->lineEdit_NameBase->setText(settings.value(SETTINGS_BASE_NAME, "").toString());

}

FormSettings::~FormSettings()
{
    delete ui;
}

void FormSettings::on_pushButtonClose_clicked()
{
    // сохранить в настройказ имя базы
    QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
    //пишем настройки
    settings.setValue(SETTINGS_BASE_NAME, ui->lineEdit_NameBase->text());
    settings.sync();

    close();
}

void FormSettings::on_pushButton_File_clicked()
{
    // выбор файла базы данных

    QString filename = QFileDialog::getOpenFileName(this,tr("Open base"),"./",tr("Data base Fules (*.db)"));
    if (!filename.isEmpty()) {
        ui->lineEdit_NameBase->setText(filename);
        // сохранить в настройказ имя базы
        QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
        //пишем настройки
        settings.setValue(SETTINGS_BASE_NAME, filename);
        settings.sync();
    }

}
