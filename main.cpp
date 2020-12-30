#include "backend.h"

//#include <QApplication>
#include <QGuiApplication>
#include <QOpenGLContext>
#include <QQmlApplicationEngine>

#include <QQmlContext>

#include "heimapmodel.h"

#include <Qt3DQuickExtras/qt3dquickwindow.h>
#include <Qt3DQuick/QQmlAspectEngine>

void setSurfaceFormat()
{
	QSurfaceFormat format;
#ifdef QT_OPENGL_ES_2
	format.setRenderableType(QSurfaceFormat::OpenGLES);
#else
	if (QOpenGLContext::openGLModuleType() == QOpenGLContext::LibGL) {
		format.setVersion(4, 3);
		format.setProfile(QSurfaceFormat::CoreProfile);
	}
#endif
	format.setDepthBufferSize(24);
	format.setSamples(4);
	format.setStencilBufferSize(8);
	QSurfaceFormat::setDefaultFormat(format);
}

int main(int argc, char *argv[])
{
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

	QGuiApplication app(argc, argv);
	//setSurfaceFormat();
//	QSurfaceFormat::setDefaultFormat(QQuick3D::idealSurfaceFormat());

	qmlRegisterType<HeiMapModel>("My", 1, 0, "HeiMapModel");
	QQmlApplicationEngine engine;

	Qt3DExtras::Quick::Qt3DQuickWindow view;

	// Expose the window as a context property so we can set the aspect ratio
	engine.rootContext()->setContextProperty("_window", &view);

	const QUrl url(QStringLiteral("qrc:/QML/main.qml"));
	QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
		&app, [url](QObject *obj, const QUrl &objUrl) {
			if (!obj && url == objUrl)
				QCoreApplication::exit(-1);
		}, Qt::QueuedConnection);


	qmlRegisterType<InstanseModel>("My", 1, 0, "InstanseModel");

	Backend back;
	engine.rootContext()->setContextProperty("backend", &back);
	engine.rootContext()->setContextProperty("projectParams", &back.proj);
	engine.rootContext()->setContextProperty("searchSettings", &back.proj.searchSetts);

	engine.load(url);
	back.root = engine.rootObjects().first();
	back.loadSettings();
//	back.loadImage("D:\\1.tif");
//	back.loadImage("D:\\Учеба\\БАР\\Москва\\50_60_1_1_2m_v3.0_reg_dem-002.tif");
//	TiffReader img;
//	img.open("D:\\1.tif");
//	img.close();
//	return 0;
	back.block = false;
	return app.exec();
}
