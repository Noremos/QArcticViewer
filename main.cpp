#include "backend.h"
#include "tiffreader.h"

//#include <QApplication>
#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QQmlContext>

#include "heimapmodel.h"

int main(int argc, char *argv[])
{
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

	QGuiApplication app(argc, argv);

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
//	back.test();
//	TiffReader img;
//	img.open("D:\\1.tif");
//	img.close();
//	return 0;
	return app.exec();
}
