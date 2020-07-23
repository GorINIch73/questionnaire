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
#include <QMessageBox>

#include <QtPrintSupport/QPrinter>
#include <QPrintDialog>
#include <QTextStream>
#include <QTextDocument>

#include <QPdfWriter>
#include <QPainter>
#include <QDesktopServices>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
//    mdiArea = new QMdiArea(this);
//    setCentralWidget(mdiArea);

    // повторная инициализация сбивает модели, делаем один раз
    database = QSqlDatabase::addDatabase("QSQLITE","main");
    // открываем базу
    if (!OpenBase()) return;
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::OpenBase()
{
    // читаем из настроек имя базы
    QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
    databaseName = settings.value(SETTINGS_BASE_NAME, "").toString();

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
    this->setWindowTitle(databaseName);
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
    FormSettings *settings = new FormSettings(this);
//    ui->tabWidget->insertTab(0,settings,tr("Настройки"));
//    ui->tabWidget->setCurrentIndex(0);

    settings->exec();
    database.close();
    OpenBase();

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
    ui->tabWidget->insertTab(0,place,tr("Районы"));
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
//    QString query = "SELECT answer_data orkers.ID, workers.name, age, sex.name, department.name from workers inner join sex on workers.sex=sex.ID inner join department on workers.department=department.ID order by department.name";
//    QString query = "SELECT questionnaire_id, question_id, answer_id FROM answers_data ORDER by questionnaire_id";
    QString query = "SELECT place.name, questionnaire.place_id, answers_data.question_id, questions.question, answer_id, answers.answer, count(answer_id) FROM answers_data  inner join questionnaire on answers_data.questionnaire_id=questionnaire.id inner join place on questionnaire.place_id=place.id inner join questions on answers_data.question_id = questions.id inner join answers on answer_id=answers.id group by answer_id, answers_data.question_id, questionnaire.place_id order by questionnaire.place_id, answers_data.question_id";
    // SELECT questionnaire.place_id, questionnaire_id, question_id, count(answer_id) FROM answers_data  INNER join questionnaire on answers_data.questionnaire_id=questionnaire.id GROUP BY answer_id, questionnaire.place_id ORDER BY questionnaire.place_id;
    //SELECT questionnaire.place_id, question_id, answer_id, count(answer_id) FROM answers_data  inner join questionnaire on answers_data.questionnaire_id=questionnaire.id group by answer_id order by questionnaire.place_id, question_id
    //SELECT place.name, questionnaire.place_id, answers_data.question_id, questions.question, answer_id, answers.answer, count(answer_id) FROM answers_data  inner join questionnaire on answers_data.questionnaire_id=questionnaire.id inner join place on questionnaire.place_id=place.id inner join questions on answers_data.question_id = questions.id inner join answers on answer_id=answers.id group by answer_id order by questionnaire.place_id, answers_data.question_id
    //SELECT place.name, questionnaire.place_id, answers_data.question_id, questions.question, answer_id, answers.answer, count(answer_id) FROM answers_data  inner join questionnaire on answers_data.questionnaire_id=questionnaire.id inner join place on questionnaire.place_id=place.id inner join questions on answers_data.question_id = questions.id inner join answers on answer_id=answers.id group by answer_id, answers_data.question_id, questionnaire.place_id order by questionnaire.place_id, answers_data.question_id
    if (!a_query.exec(query)) {
         qDebug() << "Ошибка запроса отчета: " << a_query.lastError().text();
         return;
    }

    // формирование отчета
    out << "<html>\n" << "<head>\n" << "meta Content=\"Text/html;charsrt=Windows-1251\">\n" <<
           QString("<title>%1</title>\n").arg("Report") <<
//           "<style>"
//           "   table {"
//           "    width: 100%; /* Ширина таблицы */"
//           "    border-collapse: collapse; /* Убираем двойные линии */"
//           "   }"
//           "   thead {"
//           "    background: #f5e8d0; /* Цвет фона заголовка */"
//           "   }"
//           "   td, th {"
//           "    padding: 5px; /* Поля в ячейках */"
//           "    border: 1px solid #333; /* Параметры рамки */"
//           "   }"
//           "   tbody tr:nth-child(even) {"
//           "    background: #f0f0f0; /* Зебра */"
//           "   }"
//           "  </style>"
           "</head>\n"
           "<body bgcolor = #ffffff link=#5000A0>\n";

    //заголовки
//    out << "<thead><tr bgcolor=#f0f0f0>";
//    //    int fieldNo = a_query.record().indexOf("country");

//       out << QString("<th> %1 </th>").arg("ID");
//       out << QString("<th> %1 </th>").arg("NAME");
//       out << QString("<th> %1 </th>").arg("age");
//       out << QString("<th> %1 </th>").arg("sex");
//       out << QString("<th> %1 </th>").arg("derartment");

//      //  out << QString("<th>%1</th>").arg(ui->tableView_Workers->model()->headerData(column,Qt::Horizontal).toString());
//    out << "</tr></head>\n";

    // маркеры смены группы
    QString val01= ""; // место проведения
    QString val02= ""; // вопрос
    // счетчик для количества ответов в вопросе
    int countA=0;

    out <<  "<table>\n";

    // титульный
//    out << "<tr>";
//    out <<  QString("<td>1</td>");
//    out <<  QString("<td>2</td>");
//    out <<  QString("<td>3</td>");
//    out <<  QString("<td>4</td>");
//    out << "</tr>\n";

    // данные
    while (a_query.next()) {
        // печать категорий
        // Место проведения
        if (val01!=a_query.value(0).toString()) {
            // если поменялось, то пропечатываем
            out << "<tr>";
            out <<  QString("<th colspan=\"4\" bkcolor=0 align=left>%1 </th>").arg(a_query.value(0).toString());
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
            out <<  QString("<th colspan=\"3\" bkcolor=0 align=left>%1 </th>").arg((!a_query.value(3).toString().isEmpty())? a_query.value(3).toString():QString("&nbsp;"));
            // запоминаем новое
            val02= a_query.value(3).toString();
            out << "</tr>\n";
        }

        out << "<tr>";
        // печать основных данных
        out <<  QString("<td></td>");
        out <<  QString("<td></td>");
        out <<  QString("<td bkcolor=0 width=\"80%\">%1 </td>").arg((!a_query.value(5).toString().isEmpty())? a_query.value(5).toString():QString("&nbsp;"));
        out <<  QString("<td bkcolor=0>%1 </td>").arg((!a_query.value(6).toString().isEmpty())? a_query.value(6).toString():QString("&nbsp;"));

        //добавляем текущее значение
        countA=countA+a_query.value(6).toInt();

        out << "</tr>\n";
    }
    out << "</table>\n";


//    out << tr("---------------------------------------------------------------------------------------\n");
//    out << tr("<h3> По категориям </h3>\n");

//    //выборка департаментов
//    QSqlQuery d_query = QSqlQuery(database);
//    if (!d_query.exec("SELECT department.ID, department.name from department order by department.name")) {
//         qDebug() << "Ошибка запроса отчета: " << d_query.lastError().text();
//         return;
//    }

//    out << "<table boarder = 1 cellspacing=0 celladding=2>\n";
//    QSqlQuery w_query(database);
//    while (d_query.next()) {
//        out << "<tr>";
//        out <<  QString("<td bkcolor=0>%1 </td>").arg((!d_query.value(1).toString().isEmpty())? d_query.value(1).toString():QString("&nbsp;"));
//        out << "<tr>";
//        // данные работников
//        w_query.prepare("SELECT workers.name, age, sex.name  FROM workers inner join sex on workers.sex=sex.ID WHERE workers.department = :id ORDER BY workers.name");
//        w_query.bindValue(":id",d_query.value(0).toString());

//        if (!w_query.exec()) {
//             qDebug() << "Ошибка запроса работников: " << w_query.lastError().text();
//             return;
//        }
//        while (w_query.next()) {

//            out << "<tr>";
//            // поколоночно работники
//            out <<  QString("<td bkcolor=0>   </td>");
//            out <<  QString("<td bkcolor=0>%1 </td>").arg((!w_query.value(0).toString().isEmpty())? w_query.value(0).toString():QString("&nbsp;"));
//            out <<  QString("<td bkcolor=0>%1 </td>").arg((!w_query.value(1).toString().isEmpty())? w_query.value(1).toString():QString("&nbsp;"));
//            out <<  QString("<td bkcolor=0>%1 </td>").arg((!w_query.value(2).toString().isEmpty())? w_query.value(2).toString():QString("&nbsp;"));
//            out << "</tr>\n";
//        }

//        out << "</tr\n>";
//        // расчет итогов
//        w_query.prepare("SELECT COUNT(name) FROM workers WHERE department = :id");
//        w_query.bindValue(":id",d_query.value(0).toString());
//        if (!w_query.exec()) {
//             qDebug() << "Ошибка запроса работников: " << w_query.lastError().text();
//             return;
//        }
//        w_query.next();
//        out <<  QString("<font size = 1><i>Количество работников в отделе: %1 </i></font>\n").arg(w_query.value(0).toString());


//        out << "</tr>\n";
//    }
//    out << "</table>\n";


    out << "</body>\n" << "</html>\n";

//    qDebug() << out.readAll();
    // печать
    QTextDocument document;
    document.setHtml(stringStream);

    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPaperSize(QPrinter::A4);
    printer.setOutputFileName("rep_.pdf");
    printer.setPageMargins(QMarginsF(15, 15, 15, 15));

    document.print(&printer);

    // откровем созданный отчет
    QDesktopServices::openUrl(QUrl(QUrl::fromLocalFile("rep_.pdf")));



}
