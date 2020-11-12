#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QSqlDatabase>



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

//signals:

//    void signalFromQuery(QString sq); // сигнал для запроса


private slots:
    void on_actionQuestionnaire_triggered();


    void on_actionEditQuestions_triggered();

    void on_actionSettings_triggered();

    void on_actionRegion_triggered();

    void on_actionPlace_triggered();

    void on_actionReportMain_triggered();

    void on_actionOpenBase_triggered();

    void on_actionCloseBase_triggered();

    void on_actionExit_triggered();

    void on_actionSaveAs_triggered();

    void on_actionNewBase_triggered();

    void on_actionReportS_triggered();

    void on_actionAbout_triggered();

    void on_actionFile01_triggered();

    void on_actionFile02_triggered();

    void on_actionFile03_triggered();

    void on_actionProfil_triggered();

    void slot_goQuery(QString sq); // запуск запроса


private:
    Ui::MainWindow *ui;
     //QMdiArea *mdiArea;

     QString databaseName;
     QSqlDatabase database;

     bool OpenBase();
     void SetHistory();
};
#endif // MAINWINDOW_H
