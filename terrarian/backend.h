#ifndef BACKEND_H
#define BACKEND_H

#include "instansemodel.h"
#include "obj3d.h"
#include "project.h"

#include <QObject>
#include <QMetaObject>
#include <QQuickItem>

#include <QTransform>

#include "squircle.h"

//#include <opencv2/opencv.hpp>
//namespace cv
//{
//class Mat
//{};
//}

enum class BackPath
{
	texture1,
	texture2,
	object,
	project,
	barlist
};

class BackendProxy
{
	ImageReader* reader = nullptr;
public:
	Backend(QObject *parent = nullptr);
	bool block = true;
//	cv::Mat imgread(QString path);

    virtual ~Backend() override
    {
        if (reader!= nullptr)
        {
            reader->close();
            delete reader;
        }
    }
	Project proj;

//	Qt3DCore::QEntity* spotZone;
	Q_INVOKABLE void findByParams();
	Q_INVOKABLE void test(QString path);
    Q_INVOKABLE QString loadImage(QString path, int step, int type);
	Q_INVOKABLE void setFactorSpinBox(QQuickItem *spinBox);

	Q_INVOKABLE int getHei();
	Q_INVOKABLE void setSearchingsettings(/*float coof, int minD, int maxD, float minHei, float maxHei, float bottomLineProc*/);
	Q_INVOKABLE void saveSettings();

    Q_PROPERTY(bool block READ getBlock)

    void loadSettings();
	bool getBlock()
	{
		return block;
	}
    Q_INVOKABLE void processHiemap(int start, int end);

	Q_INVOKABLE void initScene(Squircle *scene)
	{
//		m_openrender = scene->m_renderer;
		scene->back = this;
	}

	Q_INVOKABLE void key_callback(Qt::Key key, int scancode, int action, int mode)
	{
		m_openrender->key_callback(key, scancode, action, mode);
	}

	Q_INVOKABLE void mouse_callback(double xpos, double ypos)
	{
		m_openrender->mouse_callback(xpos, ypos);
	}

	Q_INVOKABLE void mouseClick_callback(double xpos, double ypos, int button)
	{
		m_openrender->mouseClick_callback(xpos, ypos, static_cast<Qt::MouseButton>(button));
	}
	//	Q_PROPERTY(Project *projParams READ getProjSetts)
//	Project *getProjSetts() { return &proj; }
	bool checkBounty(boundy &bb);


	QString getPath(BackPath pathI)
	{
		switch (pathI) {
		case BackPath::project:
			return projectPath+ "proj.qwr";
			break;
		case BackPath::barlist:
			return projectPath+ "bds.lst";
			break;
		case BackPath::texture1:
			return proj.texturePath;
		case BackPath::texture2:
			return proj.texture2Path;
		case BackPath::object:
			return projectPath+ "2.obj";
		default:
			break;
		}

	}
signals:

};

#endif // BACKEND_H
