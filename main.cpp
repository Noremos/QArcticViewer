#include "mainwindow.h"

#include <QApplication>
#include <QSurfaceFormat>

int main(int argc, char *argv[])
{
//	QSurfaceFormat fmt;
//	fmt.setVersion(3, 1);
//	fmt.setDepthBufferSize(24);
//	QSurfaceFormat::setDefaultFormat(fmt);

	QApplication a(argc, argv);
	MainWindow w;
	w.show();
	return a.exec();
}
