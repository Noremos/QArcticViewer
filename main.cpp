#include "mainwindow.h"

#include <QApplication>
#include <QSurfaceFormat>

int main(int argc, char *argv[])
{
	QSurfaceFormat fmt;
	fmt.setRenderableType(QSurfaceFormat::OpenGL);
	fmt.setVersion(3, 3);
	fmt.setDepthBufferSize(24);
	QSurfaceFormat::setDefaultFormat(fmt);

	QApplication a(argc, argv);
	MainWindow w;
	w.show();
	qDebug() << QCoreApplication::applicationDirPath() + QDir::separator() + "qt.conf";
	return a.exec();
}
