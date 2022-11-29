#include "mainwindow.h"
#include "defining.h"

#include <QApplication>



int main(int argc, char *argv[])
{

    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName(ORGANIZATION_NAME);
    QCoreApplication::setOrganizationDomain(ORGANIZATION_DOMAIN);
    QCoreApplication::setApplicationName(APPLICATION_NAME);

    MainWindow w;
    w.show();
    return a.exec();
}
