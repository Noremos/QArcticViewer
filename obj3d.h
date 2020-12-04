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

struct Point3f
{
	Point3f(float x, float y, float z) : X(x), Y(y), Z(z) {}
	float X, Y, Z;
};
#include <fstream> // for std::filebuf
#include <iterator> // for std::{i,o}streambuf_iterator

class Obj3d
{
	ImageReader *reader;
	QString name;
	int step;
	int width, height;
	int NAN_VALUE = -9999;

public:
	//	QVector<Face3d *> faces;
	//	QVector<Point3f> grid;
	float **data;
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
	void setStep(int step) { this->step = step; }
	//	void setRow(int ind, float *rowI) { data[ind] = rowI; }

	void getXY(objoff index, int &x, int &y)
	{
		x = index % width;
		y = index / width;
	}

	objoff getIndex(int x, int y, int xOr, int yOr)
	{
		if (data[y][x] == NAN_VALUE)
			return 0;

		objoff st = width * y + x;
		objoff ed = width * yOr + xOr;
		objoff cou = 0;
		for (objoff i = st; i <= ed; ++i)
		{
			int px, py;
			getXY(i, px, py);

			if (data[py][px] != NAN_VALUE)
				++cou;
		}
		return -cou;
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
		bool first = false;
		for (int h = 0; h < 500; ++h)
		{
			if (h == 0)
			{
			}
			else
			{
				delete[] data[0];
				data[0] = data[1];
				data[1] = reinterpret_cast<float *>(reader->getRowData(h));

			}
			data[1] = reinterpret_cast<float *>(reader->getRowData(h));

			for (int w = 0; w < width; ++w)
			{
				float value = data[1][w];
				if (value == -9999)
					continue;

				if (value < min)
					min = value;
				if (value > max)
					max = value;

				sw.append("v " + normConv(w, scale.X) + " " + normConv(value, scale.Z) + " " + normConv(h, scale.Y));
				sw.append(nl);


				if (h > 0 && w > 0)
				{
					//0*
					//00
					objoff i_tr = getIndex(w, 0, w, 1);

					//00
					//0*
					objoff i_br = getIndex(w, 1, w, 1);

					//*0
					//00
					objoff i_tl = getIndex(w - 1, 0, w, 1);

					//00
					//*0
					objoff i_bl = getIndex(w - 1, 1, w, 1);

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
