#include "backend.h"

#include <QImage>

using namespace cv;
Backend::Backend(QObject *parent) : QObject(parent)
{
	reader = new TiffReader();
	reader->open(L"D:\\Учеба\\БАР\\Москва\\50_59_1_2_2m_v3.0\\50_59_1_2_2m_v3.0_reg_dem.tif");
	reader->getRowData(20020);
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

QString Backend::loadImage(QString path, int step, int type)
{
	if (reader)
	{
		delete reader;
		reader = nullptr;
	}

	int imgtype = 0;
	switch (imgtype)
	{
	default:
		reader = new TiffReader();
		break;
	}

	reader->open(path.toStdWString().c_str());
	if (!reader->ready)
		return "";
//	int hei = 500;
	Obj3d object(reader);
	object.setMode((::ProcessMode) type);
	object.setStep(step);
	object.write("D:\\2.obj");
	reader->close();
	delete reader;
	reader = nullptr;
	return "D:\\2.obj";
}
