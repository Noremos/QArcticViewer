#ifndef OBJ3D_H
#define OBJ3D_H

#include <QObject>
#include <QString>
#include <QTemporaryFile>
#include <QTextStream>
#include <QVector>
#include <QDebug>
#include <QImage>

#include "face3d.h"
#include "tiffreader.h"
namespace
{
#ifdef _WIN32
#define nl "\r\n"
#elif defined macintosh // OS 9
#define nl = "\r"
#elif
#define nl = "\n" // Mac OS X uses \n
#endif
}
//#define USE_ROW


struct Point3f
{
	Point3f(float x, float y, float z) : X(x), Y(y), Z(z) {}
	float X, Y, Z;
};
#include <fstream> // for std::filebuf
#include <iterator> // for std::{i,o}streambuf_iterator

class Obj3d
{
	int width, height;
	int step;
	int NAN_VALUE = -9999;

	ImageReader *reader;
	QString name;
	float **data;
#ifdef USE_ROW
	int* nullRow;
#else
	typedef QPair<objoff,objoff> NullIndex;
	QVector<NullIndex> nulls;
	void setStep(int step) { this->step = step; }
	//	void setRow(int ind, float *rowI) { data[ind] = rowI; }

	objoff getIndex(int x, int y, int realH)
	{
		if (data[y][x] == NAN_VALUE)
			return 0;

		objoff st = width * realH  + x;
		//		objoff ed = width * (realH - 1 + yCur) + xCur;

		objoff nullCount = 0;
		objoff poz = 1, total = nulls.size();
		for (; poz < total; ++poz)
			if (st < nulls[poz].first)
				break;

		nullCount = nulls[poz - 1].second;
		//		{
		//			// curretns nulls - null.count before a
		//			nullCount = nulls[total - 1].second - nulls[poz - 1].second ;
		////			nullCount = nulls[nullCount].second  - nulls[total - 1].second;
		//		}

		//		for (objoff i = nulls.size(); i > 0; --i)
		//		{
		//			auto &ref = nulls[i - 1];
		//			if (ref.first >= st)
		//				break;

		//			nullCount += ref.second;
		//		}

		return st - nullCount;
	}
#endif

public:

	Obj3d(int wid, int hei, int step = 1) : width(wid), height(hei), step(step)
	{
		//		width = 500;
		//		height = 500;
		data = new float *[hei];
	}
	Obj3d(ImageReader *reader)
	{
		this->reader = reader;
		data = new float *[2] { nullptr, nullptr };

		width = reader->widght();
		height =reader->height();
		//		data = new float *[hei];
	}

	bool check(objoff face[3]) { return face[0] != 0 && face[1] != 0 && face[2] != 0; }
	void write(QString path, Point3f scale = Point3f(0.1, 0.1, 0.1))
	{
		QFile out(path);
		if (out.exists())
			out.remove();

		if (!out.open(QFile::WriteOnly | QFile::Truncate))
			return;

		QTextStream stream(&out);

		int BUFFER = 5000;
		QString sw;
		QString faces;
		sw = "";
		sw.append("o Main");
		sw.append(nl);

		float min = 99999;
		float max = -99999;
		int lastCount = 0;
		int nullCounter = 0;
		nulls.clear();
		// starts with 1
		nulls.push_back(NullIndex(0,-1));
		for (int h = 0; h < 500; ++h)
		{
			if (h != 0)
			{
				nulls.remove(1, lastCount);
				lastCount = nulls.size() - 1;
				delete[] data[0];
				data[0] = data[1];
			}

			data[1] = reinterpret_cast<float *>(reader->getRowData(h));

			float *dataPointer = data[1];
			for (int w = 0; w < width; ++w, ++dataPointer)
			{
				float value = *dataPointer;

				if (value == -9999)
				{
					++nullCounter;
					continue;
				}

				if (nullCounter != 0)
				{
					int add =  nulls[nulls.size() - 1].second;
					nulls.push_back(NullIndex( width * h + w, nullCounter + add));
					nullCounter = 0;
				}

				if (value < min)
					min = value;
				if (value > max)
					max = value;

				sw.append("v " + normConv(w, scale.X) + " " + normConv(value, scale.Z) + " " + normConv(h, scale.Y));
				sw.append(nl);
/*v 0 69.1359 0.2
v 0.1 68.3289 0.2
f 724 364 363   w==1 y==2
f 363 723 724*/
				if (w == 1 && h == 2)
					qDebug() << "";

				if (h > 0 && w > 0)
				{
					//0*
					//00
					objoff i_tr = getIndex(w, 0, h - 1);

					//00
					//0*
					objoff i_br = (width * h + w - nulls[nulls.size() - 1].second);

					//*0
					//00
					objoff i_tl = getIndex(w - 1, 0, h -1);

					//00
					//*0
					objoff i_bl = getIndex(w - 1, 1, h);

					//32
					//01
					objoff face[3]{i_br, i_tr, i_tl};
					if (check(face))
					{
						Face3d f0(face, 3);
						sw.append(f0.buildStr());
						sw.append(nl);
					}

					//10
					//23
					objoff face1[3]{i_tl, i_bl, i_br};
					if (check(face1))
					{
						Face3d f1(face1, 3);
						sw.append(f1.buildStr());
						sw.append(nl);
					}


				}

				if (sw.length() >= BUFFER)
				{
					stream << sw;
					sw.clear();
				}
			}

			if (h % 50 == 0)
				qDebug() << h;

		}

		qDebug() << "Min:" << min << " Max:" << max;
		stream << sw;
		out.close();
		sw.clear();
	}

	QString normConv(double f, double scale)
	{
		f *= scale;
		//		if (f < .000001 || f > 100000)
		if (f==-9999)
			f = 0;

		QString s = QString::number(f);
		return s.replace(",", ".");
	}

	~Obj3d() { clear(); }
	void clear()
	{
	}
};

#endif // OBJ3D_H
