#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName( "QarkDown" );
    a.setApplicationVersion( "1.0.0" );
    a.setOrganizationDomain( "com.github/pasnox/qarkdown" );
    a.setOrganizationName( a.applicationName() );
    a.setWindowIcon( QIcon( ":/icons/application.png" ) );

    MainWindow w;
    w.showMaximized();

    QObject::connect(&a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );
    return a.exec();
}
