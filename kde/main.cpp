#include <KApplication>
#include <KAboutData>
#include <KCmdLineArgs>

#include "mainwindow.h"

int main( int argc, char **argv ) {

	KAboutData aboutData( "demp", 0, ki18n("Demo"), "0.0.0" , ki18n("Demo program to demo KTextEditor interface"),
	                      KAboutData::License_GPL,
	                      ki18n("(c) 2008 Derrick J. Wippler (thrawn01@gmail.com)") );

    KCmdLineArgs::init(argc, argv, &aboutData);

    KApplication app;
    MainWindow* main = new MainWindow();

    // Tell the MainWindow object to show it's self
    main->show();

    // Start the event loop
    return app.exec();

}

