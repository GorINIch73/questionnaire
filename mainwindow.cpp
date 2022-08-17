#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "formquestionnaire.h"
#include "formeditquestions.h"
#include "formsettings.h"
#include "formplace.h"
#include "formregion.h"
#include "formprofil.h"
#include "defining.h"
#include "formquery.h"

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

    SetHistory();


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

    // добавляем имя базы в историю
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, ORGANIZATION_NAME, APPLICATION_NAME);
//     settings.setValue(SETTINGS_BASE_FILE1, "");
    QString file1=settings.value(SETTINGS_BASE_FILE1, "").toString();
    QString file2=settings.value(SETTINGS_BASE_FILE2, "").toString();
    QString file3=settings.value(SETTINGS_BASE_FILE3, "").toString();

    if(file1!=databaseName) {
        // сохраняем
        settings.setValue(SETTINGS_BASE_FILE3, file2);
        settings.setValue(SETTINGS_BASE_FILE2, file1);
        settings.setValue(SETTINGS_BASE_FILE1, databaseName);
        // сбвигаем меню
        ui->actionFile01->setText(databaseName);
        ui->actionFile02->setText(file1);
        ui->actionFile03->setText(file2);
    }


    return true;
}

void MainWindow::SetHistory()
{
    // чтот динамически не вышло -  пока сделаю по простому
    // gпрочитать и добавить историю открытий баз

    // считать из настроек имя базы

    QSettings settings(QSettings::IniFormat, QSettings::UserScope, ORGANIZATION_NAME, APPLICATION_NAME);
//     settings.setValue(SETTINGS_BASE_FILE1, "");
    QString file1=settings.value(SETTINGS_BASE_FILE1, "").toString();
    QString file2=settings.value(SETTINGS_BASE_FILE2, "").toString();
    QString file3=settings.value(SETTINGS_BASE_FILE3, "").toString();

//    if(!file1.isEmpty())
//        ui->menu->addAction(file1, this, SLOT(quit()));
//    if(!file2.isEmpty())
//        ui->menu->addAction(file2, this, SLOT(slot_OpenBase(file2)));
//    if(!file3.isEmpty())
//        ui->menu->addAction(file3, this, SLOT(slot_OpenBase(file3)));

    if(!file1.isEmpty())
        ui->actionFile01->setText(file1);
    if(!file2.isEmpty())
        ui->actionFile02->setText(file2);
    if(!file3.isEmpty())
        ui->actionFile03->setText(file3);
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

void MainWindow::on_actionProfil_triggered()
{
    FormProfil *profil = new FormProfil(database,this);
    ui->tabWidget->insertTab(0,profil,tr("Профили мест проведения"));
    ui->tabWidget->setCurrentIndex(0);

}

void MainWindow::slot_goQuery(QString sq)
{
    // вызов окна запроса
    FormQuery *query = new FormQuery(database, sq, this);
    ui->tabWidget->insertTab(0,query,tr("Запрос"));
    ui->tabWidget->setCurrentIndex(0);

}


void MainWindow::on_actionReportMain_triggered()
{
    // Отчет по анкетам по местам проведения и профилям

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
//    QString query = "SELECT place.name, questionnaire.place_id, answers_data.question_id, questions.question, answer_id, answers.answer, count(answer_id) FROM answers_data  inner join questionnaire on answers_data.questionnaire_id=questionnaire.id inner join place on questionnaire.place_id=place.id inner join questions on answers_data.question_id = questions.id inner join answers on answer_id=answers.id group by answer_id, answers_data.question_id, questionnaire.place_id order by questionnaire.place_id, answers_data.question_id";
//    QString query = "SELECT place.name, profil.profil_name, questionnaire.place_id, answers_data.question_id, questions.question, answer_id, answers.answer, count(answer_id) FROM answers_data  inner join questionnaire on answers_data.questionnaire_id=questionnaire.id inner join place on questionnaire.place_id=place.id inner join profil on place.profil_id=profil.id inner join questions on answers_data.question_id = questions.id inner join answers on answer_id=answers.id group by answer_id, answers_data.question_id, questionnaire.place_id order by questionnaire.place_id, answers_data.question_id";
    QString query = "SELECT place.name, profil.profil_name, questionnaire.place_id, answers_data.question_id, questions.question, answer_id, answers.answer, count(answer_id) FROM answers_data  inner join questionnaire on answers_data.questionnaire_id=questionnaire.id inner join place on questionnaire.place_id=place.id inner join profil on place.profil_id=profil.id inner join questions on answers_data.question_id = questions.id inner join answers on answer_id=answers.id group by answer_id, answers_data.question_id, questionnaire.place_id order by questionnaire.place_id, questions.question, answers.answer";
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

        // вывод итога прошлой группы
        if (val02!=a_query.value(4).toString()) {
            if(countA!=0) {
                out << "<tr>";
                out <<  QString("<td></td>");
                out <<  QString("<td></td>");
                out <<  QString("<td align=right><i>ИТОГО по вопросу:</i></td>");
                out <<  QString("<td align=right><i>%1</i></td>").arg(countA);
                countA=0;
             }
        }

        // Место проведения - верхняя категория
        if (val01!=a_query.value(0).toString()) {
            // если поменялось место проведения, то пропечатываем название запоминаем новое название
            out << "<tr>";
            out <<  QString("<th colspan=\"4\" align=left> <h2> %1 \"профиль: %2 \"</h2></th>").arg(a_query.value(0).toString()).arg(a_query.value(1).toString());
            // запоминаем новое
            val01= a_query.value(0).toString();
            out << "</tr>\n";

        }

        // Вопрос
        if (val02!=a_query.value(4).toString()) {
            // если поменялся вопрос, то пропечатываем название, сбрасываем счетчик, запоминаем новое название
            out << "<tr>";
            out <<  QString("<td></td>");
            out <<  QString("<th colspan=\"3\" align=left>%1 </th>").arg((!a_query.value(4).toString().isEmpty())? a_query.value(4).toString():QString("&nbsp;"));
            // запоминаем новое
            val02= a_query.value(4).toString();
            out << "</tr>\n";
        }



        out << "<tr>";
        // печать основных данных
        out <<  QString("<td></td>");
        out <<  QString("<td></td>");
        out <<  QString("<td  width=\"80%\">%1 </td>").arg((!a_query.value(6).toString().isEmpty())? a_query.value(6).toString():QString("&nbsp;"));
        out <<  QString("<td >%1 </td>").arg((!a_query.value(7).toString().isEmpty())? a_query.value(7).toString():QString("&nbsp;"));

        //добавляем текущее значение
        countA=countA+a_query.value(7).toInt();

        out << "</tr>\n";
    }

    // вывод итога ппоследней группы
    if(countA!=0) {
        out << "<tr>";
        out <<  QString("<td></td>");
        out <<  QString("<td></td>");
        out <<  QString("<td align=right><i>ИТОГО по вопросу:</i></td>");
        out <<  QString("<td align=right><i>%1</i></td>").arg(countA);
        countA=0;
     }

    out << "</table>\n";



    out << "</body>\n" << "</html>\n";

//    qDebug() << out.readAll();
    // печать
    QTextDocument document;
    document.setHtml(stringStream);

    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    //printer.setPaperSize(QPrinter::A4);
    printer.setOutputFileName("rep_plc.pdf");
    printer.setPageMargins(QMarginsF(1, 1, 1, 1));

    document.print(&printer);

    // откровем созданный отчет
    QDesktopServices::openUrl(QUrl(QUrl::fromLocalFile("rep_plc.pdf")));



}


void MainWindow::on_actionReportMain_r_triggered()
{
    // Отчет по анкетам по районам

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
    QString query = "SELECT region.name, region.id, answers_data.question_id, questions.question, answer_id, answers.answer, count(answer_id) FROM answers_data  inner join questionnaire on answers_data.questionnaire_id=questionnaire.id inner join place on questionnaire.place_id=place.id inner join region on region.id=place.region_id inner join questions on answers_data.question_id = questions.id inner join answers on answer_id=answers.id group by answer_id, answers_data.question_id, region.id order by region.id, questions.question, answers.answer";
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
           "}"
           "td { "
            "text-align: left; /* Выравнивание по лево */"
            "border-bottom: none;"
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
    QString val01= ""; // район
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

        // вывод итога прошлой группы
        if (val02!=a_query.value(3).toString()) {
            if(countA!=0) {
                out << "<tr>";
                out <<  QString("<td></td>");
                out <<  QString("<td></td>");
                out <<  QString("<td align=right><i>ИТОГО по вопросу:</i></td>");
                out <<  QString("<td align=right><i>%1</i></td>").arg(countA);
                countA=0;
             }
        }

        // Район - верхняя категория
        if (val01!=a_query.value(0).toString()) {
            // если поменялось место проведения, то пропечатываем название запоминаем новое название
            out << "<tr>";
            out <<  QString("<th colspan=\"4\" align=left> <h2> %1 </h2></th>").arg(a_query.value(0).toString());
            // запоминаем новое
            val01= a_query.value(0).toString();
            out << "</tr>\n";

        }

        // Вопрос
        if (val02!=a_query.value(3).toString()) {
            // если поменялся вопрос, то пропечатываем название, сбрасываем счетчик, запоминаем новое название
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

    // вывод итога ппоследней группы
    if(countA!=0) {
        out << "<tr>";
        out <<  QString("<td></td>");
        out <<  QString("<td></td>");
        out <<  QString("<td align=right><i>ИТОГО по вопросу:</i></td>");
        out <<  QString("<td align=right><i>%1</i></td>").arg(countA);
        countA=0;
     }

    out << "</table>\n";



    out << "</body>\n" << "</html>\n";

//    qDebug() << out.readAll();
    // печать
    QTextDocument document;
    document.setHtml(stringStream);

    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    //printer.setPaperSize(QPrinter::A4);
    printer.setOutputFileName("rep_ry.pdf");
    printer.setPageMargins(QMarginsF(1, 1, 1, 1));

    document.print(&printer);

    // откровем созданный отчет
    QDesktopServices::openUrl(QUrl(QUrl::fromLocalFile("rep_ry.pdf")));

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
            ui->actionProfil->setEnabled(true);
            ui->actionReportMain->setEnabled(true);
            ui->actionReportMain_r->setEnabled(true);
            ui->actionOpenBase->setEnabled(false);
            ui->actionCloseBase->setEnabled(true);

            ui->actionSaveAs->setEnabled(true);
            ui->actionReportS->setEnabled(true);
            ui->actionReportS_US->setEnabled(true);
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
    ui->actionProfil->setEnabled(false);
    ui->actionEditQuestions->setEnabled(false);
    ui->actionReportMain->setEnabled(false);
    ui->actionReportMain_r->setEnabled(false);
    ui->actionCloseBase->setEnabled(false);
    ui->actionOpenBase->setEnabled(true);

    ui->actionSaveAs->setEnabled(false);
    ui->actionReportS->setEnabled(false);
    ui->actionReportS_US->setEnabled(false);
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

         // запрос на создание таблицы профилей
         str = "CREATE TABLE profil ("
             "id          INTEGER PRIMARY KEY AUTOINCREMENT,"
             "profil_name VARCHAR );";
         if (!a_query.exec(str))
             qDebug() << "таблица профилей: " << a_query.lastError().text();


         // запрос на создание таблицы мест проведения
         str = "CREATE TABLE place ("
             "id         INTEGER       PRIMARY KEY AUTOINCREMENT UNIQUE,"
             "name       VARCHAR (255),"
             "region_id  INTEGER       REFERENCES region (id),"
             "profil_id  INTEGER       REFERENCES profil (id),"
             "for_report BOOLEAN);";
         if (!a_query.exec(str))
             qDebug() << "таблица мест проведения: " << a_query.lastError().text();

         // запрос на создание таблицы ответов
         str = "CREATE TABLE answers ("
                     "id          INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE,"
                     "question_id INTEGER REFERENCES questions (id),"
                     "answer      TEXT),"
                     "satisfaction BOOLEAN DEFAULT (FALSE);";
         if (!a_query.exec(str))
             qDebug() << "таблица ответов: " << a_query.lastError().text();

         // запрос на создание таблицы вопросов
         str = "CREATE TABLE questions ("
                 "id           INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE,"
                 "question     TEXT,"
                 "some_answers BOOLEAN DEFAULT (false),"
                 "be_empty     BOOLEAN DEFAULT (false),"
                 "satisfaction BOOLEAN DEFAULT (false),";
                 "profile_st   BOOLEAN DEFAULT (false));";
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
         str = "INSERT INTO setings(option_name) values ('Наименование анкеты'),('Примечание'),('ID ответа СМП'),('ID вопроса контроля'),('ID ответа контроля');";
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
    // отчет по удовлетворенности с подгруппами (учитывае вопрос с подпрофилями)
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
//    QString query = "SELECT region.name, profil.profil_name, count(questions.id) as count_questions, count(answers.satisfaction) FROM answers_data  inner join questionnaire on answers_data.questionnaire_id=questionnaire.id inner join place on questionnaire.place_id=place.id inner join region on place.region_id=region.id inner join profil on place.profil_id=profil.id inner join questions on answers_data.question_id = questions.id inner join answers on answer_id=answers.id WHERE questions.satisfaction = TRUE GROUP BY region.name, profil.profil_name";
    QString query = "SELECT region.name, profil.profil_name, st_profile_name, count(questions.id) as count_questions, count(answers.satisfaction) FROM answers_data  inner join questionnaire on answers_data.questionnaire_id=questionnaire.id inner join place on questionnaire.place_id=place.id inner join region on place.region_id=region.id inner join profil on place.profil_id=profil.id inner join questions on answers_data.question_id = questions.id inner join answers on answer_id=answers.id inner join (SELECT answers_data.questionnaire_id as st_questionnaire_id, answers.answer as st_profile_name FROM answers_data  inner join questions on answers_data.question_id = questions.id inner join answers on answer_id=answers.id WHERE questions.profile_st = TRUE) on answers_data.questionnaire_id=st_questionnaire_id WHERE questions.satisfaction = TRUE GROUP BY region.name, profil.profil_name, st_profile_name";
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
    // титульный
    QSqlQuery t_query = QSqlQuery(database);
    t_query.exec("SELECT option_data FROM setings WHERE option_name=\"Наименование анкеты\"");
    t_query.next();
    out << QString("<h2>ОТЧЕТ по удовлетвореннсти по подгруппам по %1 </h2>").arg(t_query.value(0).toString());

    // данные
    out <<  "<table>\n";
    // титульный
    out << "<tr>";
    out <<  QString("<th> Район </th> <th> Профиль </th> <th> Категория</th> <th> Количество опрошенных</th>  <th> Количество удовлетворенных </th>");
    out << "</tr>";

    QString kat01="";
    QString kat02="";
    int countA=0;
    int countB=0;

    while (a_query.next()) {
        // печать категорий

        if (kat01!=a_query.value(0).toString() || kat02!=a_query.value(1).toString()) {
            if(countA!=0) {
                out << "<tr>";
                out <<  QString("<td></td>");
                out <<  QString("<td></td>");
                out <<  QString("<td align=right><b>ИТОГО:</b></td>");
                out <<  QString("<td align='center'><b>%1</b></td>").arg(countA);
                out <<  QString("<td align='center'><b>%1</b></td>").arg(countB);
                countA=0;
                countB=0;
             }
        }

         out << "<tr>";
        // печать данных
        out <<  QString("<td  width=\"30%\">%1 </td>").arg((!a_query.value(0).toString().isEmpty())? a_query.value(0).toString():QString("&nbsp;"));
        out <<  QString("<td >%1 </td>").arg((!a_query.value(1).toString().isEmpty())? a_query.value(1).toString():QString("&nbsp;"));
        out <<  QString("<td >%1 </td>").arg((!a_query.value(2).toString().isEmpty())? a_query.value(2).toString():QString("&nbsp;"));
        out <<  QString("<td align='center'>%1 </td>").arg((!a_query.value(3).toString().isEmpty())? a_query.value(3).toString():QString("&nbsp;"));
        out <<  QString("<td align='center'>%1 </td>").arg((!a_query.value(4).toString().isEmpty())? a_query.value(4).toString():QString("&nbsp;"));

        kat01=a_query.value(0).toString();
        kat02=a_query.value(1).toString();
        countA += a_query.value(3).toInt();
        countB += a_query.value(4).toInt();

        out << "</tr>";
    }
// последние итоги
    out << "<tr>";
    out <<  QString("<td></td>");
    out <<  QString("<td></td>");
    out <<  QString("<td align=right><b>ИТОГО:</b></td>");
    out <<  QString("<td align='center'><b>%1</b></td>").arg(countA);
    out <<  QString("<td align='center'><b>%1</b></td>").arg(countB);

    out << "</table>";



    out << "</body>\n" << "</html>\n";

//    qDebug() << out.readAll();
    // печать
    QTextDocument document;
    document.setHtml(stringStream);

    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    //printer.setPaperSize(QPrinter::A4);
    printer.setOutputFileName("rep_stf.pdf");
    printer.setPageMargins(QMarginsF(1, 1, 1, 1));

    document.print(&printer);

    // откровем созданный отчет
    QDesktopServices::openUrl(QUrl(QUrl::fromLocalFile("rep_stf.pdf")));


}

void MainWindow::on_actionReportS_US_triggered()
{
    // отчет по удовлетворенности простой
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
    QString query = "SELECT region.name, profil.profil_name, count(questions.id) as count_questions, count(answers.satisfaction) FROM answers_data  inner join questionnaire on answers_data.questionnaire_id=questionnaire.id inner join place on questionnaire.place_id=place.id inner join region on place.region_id=region.id inner join profil on place.profil_id=profil.id inner join questions on answers_data.question_id = questions.id inner join answers on answer_id=answers.id WHERE questions.satisfaction = TRUE GROUP BY region.name, profil.profil_name";
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
           "}"
           "td { "
            "text-align: left; /* Выравнивание по лево */"
            "border-bottom: none;"
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

    // титульный
    QSqlQuery t_query = QSqlQuery(database);
    t_query.exec("SELECT option_data FROM setings WHERE option_name=\"Наименование анкеты\"");
    t_query.next();
    out << QString("<h2>ОТЧЕТ по удовлетвореннсти по %1 </h2>").arg(t_query.value(0).toString());

    // данные
    out <<  "<table>\n";
    // титульный
    out << "<tr>";
    out <<  QString("<th> Район </th> <th> Профиль </th> <th> Количество опрошенных</th>  <th> Количество удовлетворенных </th>");
    out << "</tr>";

    QString kat01="";
    int countA=0;
    int countB=0;

    while (a_query.next()) {
        // печать групп

        if (kat01!=a_query.value(0).toString()) {
            if(countA!=0) {
                out << "<tr>";
                out <<  QString("<td></td>");
                out <<  QString("<td align=right><b>ИТОГО:</b></td>");
                out <<  QString("<td align='center'><b>%1</b></td>").arg(countA);
                out <<  QString("<td align='center'><b>%1</b></td>").arg(countB);
                countA=0;
                countB=0;
             }
        }

         out << "<tr>";
        // печать данных
        out <<  QString("<td  width=\"30%\">%1 </td>").arg((!a_query.value(0).toString().isEmpty())? a_query.value(0).toString():QString("&nbsp;"));
        out <<  QString("<td >%1 </td>").arg((!a_query.value(1).toString().isEmpty())? a_query.value(1).toString():QString("&nbsp;"));
        out <<  QString("<td align='center'>%1 </td>").arg((!a_query.value(2).toString().isEmpty())? a_query.value(2).toString():QString("&nbsp;"));
        out <<  QString("<td align='center'>%1 </td>").arg((!a_query.value(3).toString().isEmpty())? a_query.value(3).toString():QString("&nbsp;"));

        kat01=a_query.value(0).toString();
        countA += a_query.value(2).toInt();
        countB += a_query.value(3).toInt();

        out << "</tr>";
    }
// последние итоги
    out << "<tr>";
    out <<  QString("<td></td>");
    out <<  QString("<td align=right><b>ИТОГО:</b></td>");
    out <<  QString("<td align='center'><b>%1</b></td>").arg(countA);
    out <<  QString("<td align='center'><b>%1</b></td>").arg(countB);

    out << "</table>";



    out << "</body>\n" << "</html>\n";

//    qDebug() << out.readAll();
    // печать
    QTextDocument document;
    document.setHtml(stringStream);

    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName("rep_stfus.pdf");
    printer.setPageMargins(QMarginsF(1, 1, 1, 1));

    document.print(&printer);

    // откровем созданный отчет
    QDesktopServices::openUrl(QUrl(QUrl::fromLocalFile("rep_stfus.pdf")));
}


void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::information(this,"Info","Программа обработки результатов анкетирования. \n\nGorINIch`2022 ver0.07\ngGorINIch@gmail.com");
}

void MainWindow::on_actionFile01_triggered()
{
    //файл истории 01
    if(!ui->actionFile01->text().isEmpty()) {
        databaseName = ui->actionFile01->text();
        on_actionOpenBase_triggered();
    }

}

void MainWindow::on_actionFile02_triggered()
{
    //файл истории 02
    if(!ui->actionFile02->text().isEmpty()) {
        databaseName = ui->actionFile02->text();
        on_actionOpenBase_triggered();
    }


}

void MainWindow::on_actionFile03_triggered()
{
    //файл истории 03
    if(!ui->actionFile03->text().isEmpty()) {
        databaseName = ui->actionFile03->text();
        on_actionOpenBase_triggered();
    }


}




