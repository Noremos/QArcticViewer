#include "mainwindow.h"

#include <QApplication>
#include <QSurfaceFormat>
#include <QMatrix4x4>
#include <QVector2D>

int main(int argc, char *argv[])
{
	QSurfaceFormat fmt;
	fmt.setRenderableType(QSurfaceFormat::OpenGL);
	fmt.setVersion(3, 3);
	fmt.setDepthBufferSize(24);
	QSurfaceFormat::setDefaultFormat(fmt);

	QApplication app(argc, argv);
	MainWindow w;
    w.show();
	return app.exec();
}
