#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "formquestionnaire.h"
#include "formeditquestions.h"
#include "formsettings.h"
#include "formplace.h"
#include "formregion.h"
#include "defining.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include <QDebug>
#include <QSettings>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>

#include <QtPrintSupport/QPrinter>
#include <QPrintDialog>
#include <QTextStream>
#include <QTextDocument>

#include <QPdfWriter>
#include <QPainter>
#include <QDesktopServices>
#include <QCommandLineParser>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
//    mdiArea = new QMdiArea(this);
//    setCentralWidget(mdiArea);

    // повторная инициализация сбивает модели, делаем один раз
    database = QSqlDatabase::addDatabase("QSQLITE","main");

    //сбрасываем тригеры меню
    on_actionCloseBase_triggered();


    // если есть параметры в командной строке то открываем
    if (QCoreApplication::arguments().count() > 1) {
        databaseName=QCoreApplication::arguments().at(1);
        on_actionOpenBase_triggered();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::OpenBase()
{
    // читаем из настроек имя базы
//    QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
//    databaseName = settings.value(SETTINGS_BASE_NAME, "").toString();

    //проверяем на наличие файл базы
    if(!QFile(databaseName).exists()){
        qDebug() << "Файла базы нет!";
    //    this->setWindowTitle(tr("Файла базы нет!"));
    }

    // открываем базу
    database.setDatabaseName(databaseName);
    if(!database.open()){
      qDebug() << "Ошибка открытия базы!";
      this->setWindowTitle("Error!");
      return false;
    }
    // титульный окна имя базы
    this->setWindowTitle("Обработка анкет: " + databaseName);
    return true;
}

void MainWindow::on_actionQuestionnaire_triggered()
{
    FormQuestionnaire *questionnaire = new FormQuestionnaire(database,this);
//    ui->tabWidget->addTab(questionnaire,tr("Анкеты"));
    ui->tabWidget->insertTab(0,questionnaire,tr("Анкеты"));
    ui->tabWidget->setCurrentIndex(0);

//    ui->tabWidget->insertTab(0,questionnaire,"--");
//    QMdiSubWindow *subWindow = mdiArea->addSubWindow(questionnaire);
//    subWindow->setAttribute(Qt::WA_DeleteOnClose);
//    subWindow->showMaximized();


}


void MainWindow::on_actionEditQuestions_triggered()
{
    FormEditQuestions *editquestions = new FormEditQuestions(database,this);
    //ui->tabWidget->addTab(editquestions,tr("Редактор вопросов"));
    ui->tabWidget->insertTab(0,editquestions,tr("Редактор вопросов"));
    ui->tabWidget->setCurrentIndex(0);
}

void MainWindow::on_actionSettings_triggered()
{
    FormSettings *settings = new FormSettings(database, this);
//    ui->tabWidget->insertTab(0,settings,tr("Настройки"));
//    ui->tabWidget->setCurrentIndex(0);

    ui->tabWidget->insertTab(0,settings,tr("Настройки"));
    ui->tabWidget->setCurrentIndex(0);

    // обновить
//    settings->exec();
//    database.close();
//    OpenBase();

}

void MainWindow::on_actionRegion_triggered()
{
    FormRegion *region = new FormRegion(database,this);
    ui->tabWidget->insertTab(0,region,tr("Районы"));
    ui->tabWidget->setCurrentIndex(0);
}

void MainWindow::on_actionPlace_triggered()
{
    FormPlace *place = new FormPlace(database,this);
    ui->tabWidget->insertTab(0,place,tr("Места проведения"));
    ui->tabWidget->setCurrentIndex(0);
}



void MainWindow::on_actionReportMain_triggered()
{
    // Отчет по анкетам основной

    // формирование и печать

    QString stringStream;
    QTextStream out(&stringStream);

    // запрос
    // база открыта?
    if(!database.isOpen()){
          qDebug() << "База не открыта!";
          QMessageBox::critical(this,"Error","База не открыта!");
          return;
    }


    QSqlQuery a_query = QSqlQuery(database);
    QString query = "SELECT place.name, questionnaire.place_id, answers_data.question_id, questions.question, answer_id, answers.answer, count(answer_id) FROM answers_data  inner join questionnaire on answers_data.questionnaire_id=questionnaire.id inner join place on questionnaire.place_id=place.id inner join questions on answers_data.question_id = questions.id inner join answers on answer_id=answers.id group by answer_id, answers_data.question_id, questionnaire.place_id order by questionnaire.place_id, answers_data.question_id";
    if (!a_query.exec(query)) {
         qDebug() << "Ошибка запроса отчета: " << a_query.lastError().text();
         return;
    }

    // формирование отчета
    out << "<html>\n" << "<head>\n" << "meta Content=\"Text/html;charsrt=Windows-1251\">\n" <<
           QString("<title>%1</title>\n").arg("Report") <<

           "<style>"
           "table {"
            "width: 100%; /* Ширина таблицы */"
//            "border-collapse: collapse; /* Убираем двойные линии */"
//            "border-bottom: 1px none #333; /* Линия снизу таблицы */"
//            "border-bottom: none; /* Линия снизу таблицы */"
//            "border-top: none; /* Линия сверху таблицы */"
           "}"
           "td { "
            "text-align: left; /* Выравнивание по лево */"
            "border-bottom: none;"
//            "border-top: 1px dashed;"
           "}"
           "td, th {"
            "padding: 1px; /* Поля в ячейках */"
           "}"
            "th {"
            "border-top: 1px dashed;"
            "}"
         "  </style>"

           "</head>\n"
           "<body bgcolor = #ffffff link=#5000A0>\n";


    // маркеры смены группы
    QString val01= ""; // место проведения
    QString val02= ""; // вопрос
    // счетчик для количества ответов в вопросе
    int countA=0;

    // титульный
    QSqlQuery t_query = QSqlQuery(database);
    t_query.exec("SELECT option_data FROM setings WHERE option_name=\"Наименование анкеты\"");
    t_query.next();
    out << QString("<h2>ОТЧЕТ по %1 </h2>").arg(t_query.value(0).toString());

    // данные
    out <<  "<table>\n";
    while (a_query.next()) {
        // печать категорий
        // Место проведения
        if (val01!=a_query.value(0).toString()) {
            // если поменялось, то пропечатываем
            out << "<tr>";
            out <<  QString("<th colspan=\"4\" align=left>%1 </th>").arg(a_query.value(0).toString());
            // запоминаем новое
            val01= a_query.value(0).toString();
            out << "</tr>\n";
        }
        // Вопрос
        if (val02!=a_query.value(3).toString()) {
            // вывод итога прошлой группы
            if(countA!=0) {
                out << "<tr>";
                out <<  QString("<td></td>");
                out <<  QString("<td></td>");
                out <<  QString("<td align=right><i>ИТОГО по вопросу:</i></td>");
                out <<  QString("<td align=right><i>%1</i></td>").arg(countA);
                countA=0;
             }

            // смена группы
            // запоминаем новое
            val02= a_query.value(3).toString();
            out << "</tr>\n";

            // если поменялось, то пропечатываем
            out << "<tr>";
            out <<  QString("<td></td>");
            out <<  QString("<th colspan=\"3\" align=left>%1 </th>").arg((!a_query.value(3).toString().isEmpty())? a_query.value(3).toString():QString("&nbsp;"));
            // запоминаем новое
            val02= a_query.value(3).toString();
            out << "</tr>\n";
        }

        out << "<tr>";
        // печать основных данных
        out <<  QString("<td></td>");
        out <<  QString("<td></td>");
        out <<  QString("<td  width=\"80%\">%1 </td>").arg((!a_query.value(5).toString().isEmpty())? a_query.value(5).toString():QString("&nbsp;"));
        out <<  QString("<td >%1 </td>").arg((!a_query.value(6).toString().isEmpty())? a_query.value(6).toString():QString("&nbsp;"));

        //добавляем текущее значение
        countA=countA+a_query.value(6).toInt();

        out << "</tr>\n";
    }
    out << "</table>\n";



    out << "</body>\n" << "</html>\n";

//    qDebug() << out.readAll();
    // печать
    QTextDocument document;
    document.setHtml(stringStream);

    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPaperSize(QPrinter::A4);
    printer.setOutputFileName("rep_plc.pdf");
    printer.setPageMargins(QMarginsF(15, 15, 15, 15));

    document.print(&printer);

    // откровем созданный отчет
    QDesktopServices::openUrl(QUrl(QUrl::fromLocalFile("rep_plc.pdf")));



}

void MainWindow::on_actionOpenBase_triggered()
{
    // отктыьб базу данных
    // выбор файла базы данных
    if (databaseName.isEmpty())
     databaseName = QFileDialog::getOpenFileName(this,tr("Open base"),"./",tr("Data base Fules (*.qustnr)"));

     if (!databaseName.isEmpty()) {
        // закрывкем старую
            database.close();
            // сохранить в настройказ имя базы
    //        QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
            //пишем настройки
    //        settings.setValue(SETTINGS_BASE_NAME, filename);
    //        settings.sync();
            // открыть меню
            ui->actionPlace->setEnabled(true);
            ui->actionRegion->setEnabled(true);
            ui->actionQuestionnaire->setEnabled(true);
            ui->actionEditQuestions->setEnabled(true);
            ui->actionReportMain->setEnabled(true);
            ui->actionOpenBase->setEnabled(false);
            ui->actionCloseBase->setEnabled(true);

            ui->actionSaveAs->setEnabled(true);
            ui->actionReportS->setEnabled(true);
            ui->actionSettings->setEnabled(true);

            //ui->actionSettings->setEnabled(true);
            // открываем новую базу
            OpenBase();
     }

}

void MainWindow::on_actionCloseBase_triggered()
{
    // закрыть все вкладки
//    for (int i = ui->tabWidget->count() - 1; i >= 0; --i) {
//       ui->tabWidget->removeTab(i);
//    }

    ui->tabWidget->clear();

    //закрыть базу
    databaseName="";
    database.close();
    this->setWindowTitle("Обработка анкет: no base");
    ui->actionPlace->setEnabled(false);
    ui->actionRegion->setEnabled(false);
    ui->actionQuestionnaire->setEnabled(false);
    ui->actionEditQuestions->setEnabled(false);
    ui->actionReportMain->setEnabled(false);
    ui->actionCloseBase->setEnabled(false);
    ui->actionOpenBase->setEnabled(true);

    ui->actionSaveAs->setEnabled(false);
    ui->actionReportS->setEnabled(false);
    ui->actionSettings->setEnabled(false);

}

void MainWindow::on_actionExit_triggered()
{
    QApplication::closeAllWindows();
}

void MainWindow::on_actionSaveAs_triggered()
{
    // дублирование базы
    if (!databaseName.isEmpty()) {
        // запросить новое имя
        QString  newBase = QFileDialog::getSaveFileName(this,tr("Open base"),databaseName,tr("Data base Fules (*.qustnr)"));
        // если дано новое имя
        if (databaseName != newBase) {
            database.close();
            qDebug() << "copy " << databaseName << " to " << newBase;
            // возможно надо проверить не открыта ли база кем то еще
            if (!QFile::copy(databaseName,newBase)) {
                    QMessageBox::critical(this,"ERROR","База не скопирована!");
                    return;
            }
            databaseName = newBase;
        }
        on_actionOpenBase_triggered();
    }
}

void MainWindow::on_actionNewBase_triggered()
{
    //переноcим создание базы сюда!
    // выбор файла базы данных
    QString newBase =  QFileDialog::getSaveFileName(this,tr("Create new base"),"./",tr("Data base Fules (*.qustnr)"));

     if (!newBase.isEmpty()) {
        // создаем

         //проверяем на наличие файл базы
         if(QFile(newBase).exists()){
             qDebug() << "Файла базы есть!";
             QMessageBox::information(this,"Error","Выбранная база уже существует. Выберете другое имя!");
             return;
         }

     // открываем базу
         QSqlDatabase dbm = QSqlDatabase::addDatabase("QSQLITE","new");
         dbm.setDatabaseName(newBase);
         if(!dbm.open()){
           qDebug() << "Ошибка открытия базы!";
           QMessageBox::critical(this,"Error",dbm.lastError().text());
           return;
         }

         QSqlQuery a_query = QSqlQuery(dbm);

         // запрос на создание таблицы районов
         QString str = "CREATE TABLE region ("
                     "id   INTEGER       PRIMARY KEY AUTOINCREMENT UNIQUE,"
                     "name VARCHAR (255));";
         if (!a_query.exec(str))
             qDebug() << "таблица районов: " << a_query.lastError().text();

         // запрос на создание таблицы мест проведения
         str = "CREATE TABLE place ("
             "id         INTEGER       PRIMARY KEY AUTOINCREMENT UNIQUE,"
             "name       VARCHAR (255),"
             "region_id  INTEGER       REFERENCES region (id),"
             "for_report BOOLEAN);";
         if (!a_query.exec(str))
             qDebug() << "таблица мест проведения: " << a_query.lastError().text();

         // запрос на создание таблицы ответов
         str = "CREATE TABLE answers ("
                     "id          INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE,"
                     "question_id INTEGER REFERENCES questions (id),"
                     "answer      TEXT);";
         if (!a_query.exec(str))
             qDebug() << "таблица ответов: " << a_query.lastError().text();

         // запрос на создание таблицы вопросов
         str = "CREATE TABLE questions ("
                 "id           INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE,"
                 "question     TEXT,"
                 "some_answers BOOLEAN DEFAULT (false),"
                 "be_empty     BOOLEAN DEFAULT (false),"
                 "satisfaction BOOLEAN DEFAULT (false));";
         if (!a_query.exec(str))
             qDebug() << "таблица вопросов: " << a_query.lastError().text();

         // запрос на создание таблицы анкет
         str = "CREATE TABLE questionnaire ("
                 "id       INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE,"
                 "place_id INTEGER REFERENCES place (id));";
         if (!a_query.exec(str))
             qDebug() << "таблица аекет: " << a_query.lastError().text();

         // запрос на создание таблицы данных по ответам
         str = "CREATE TABLE answers_data ("
                 "id               INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE,"
                 "questionnaire_id INTEGER REFERENCES questionnaire (id),"
                 "question_id      INTEGER REFERENCES questions (id),"
                 "answer_id        INTEGER REFERENCES answers (id));";
         if (!a_query.exec(str))
             qDebug() << "таблица данных по ответам: " << a_query.lastError().text();

         // запрос на создание таблицы с настройками  // как то не так надо передалеть!
         str = "CREATE TABLE setings ("
                 "id          INTEGER PRIMARY KEY AUTOINCREMENT"
                                     " UNIQUE,"
                 "option_name VARCHAR,"
                 "option_data VARCHAR"
                ");";
         if (!a_query.exec(str))
             qDebug() << "таблица настроек: " << a_query.lastError().text();
         // вставить значения опций
         str = "INSERT INTO setings(option_name) values ('Наименование анкеты'),('Примечание');";
         if (!a_query.exec(str))
             qDebug() << "таблица настроек: " << a_query.lastError().text();


         //
         QMessageBox::information(this,"Info","Операция завершена.");
         dbm.close();

         // откроем созданную базу
        databaseName=newBase;
        on_actionOpenBase_triggered();
     }


}

void MainWindow::on_actionReportS_triggered()
{
    // отчет по удовлетворенности

}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::information(this,"Info","Программа обработки результатов анкетирования. \n\nGorINIch`2020 ver0.01\nggorinich@gmail.com");
}
