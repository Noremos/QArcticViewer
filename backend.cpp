#include "backend.h"

#include <QImage>
#include "tiffreader.h"

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


