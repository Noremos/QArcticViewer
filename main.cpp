#include "backend.h"

//#include <QApplication>
#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QQmlContext>
#include <QtQuick3D/qquick3d.h>

#include "heimapmodel.h"

int main(int argc, char *argv[])
{
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

	QGuiApplication app(argc, argv);
	QSurfaceFormat::setDefaultFormat(QQuick3D::idealSurfaceFormat());

	qmlRegisterType<HeiMapModel>("My", 1, 0, "HeiMapModel");
	QQmlApplicationEngine engine;
	const QUrl url(QStringLiteral("qrc:/QML/main.qml"));
	QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
		&app, [url](QObject *obj, const QUrl &objUrl) {
			if (!obj && url == objUrl)
				QCoreApplication::exit(-1);
		}, Qt::QueuedConnection);


	Backend back;
	engine.rootContext()->setContextProperty("backend", &back);

	engine.load(url);
	back.root = engine.rootObjects().first();
	back.loadImage("D:\\1.tif");
//	back.loadImage("D:\\Учеба\\БАР\\Москва\\50_60_1_1_2m_v3.0_reg_dem-002.tif");
//	TiffReader img;
//	img.open("D:\\1.tif");
//	img.close();
//	return 0;
	return app.exec();
}
