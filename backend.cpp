#include "backend.h"

#include <QImage>

using namespace cv;
Backend::Backend(QObject *parent) : QObject(parent)
{

}

void Backend::test(QString path)
{

}

cv::Mat Backend::imgread(QString path)
{
	QFile file(path);
	if (!file.open(QIODevice::ReadOnly))
		return cv::Mat();

	QByteArray blob = file.readAll();
	std::vector<unsigned char> data(blob.begin(), blob.end());
//	return imdecode(cv::Mat(data), 1);
	return cv::Mat();
}


void  Backend::loadImage(QString path)
{
	if (reader)
	{
		delete reader;
		reader = nullptr;
	}

	int type = 0;
	switch (type)
	{
	default:
		reader = new TiffReader();
		break;
	}
	reader->open(path.toStdWString().c_str());
	if (!reader->ready)
		return;
//	int hei = 500;
	Obj3d object(reader);


	object.write("D:\\1.obj");
}
