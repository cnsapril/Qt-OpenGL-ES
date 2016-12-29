#include <QtWidgets\qapplication.h>
#include <MeMainWindow.h>
#include <QtWidgets\qwidget.h>

int main(int argc, char *argv[])
{
	QCoreApplication::addLibraryPath("./");
	QApplication app(argc, argv);

	MeMainWindow meWindow;
	meWindow.show();

	return app.exec();
}