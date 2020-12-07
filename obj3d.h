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


#define USE_ROW


struct Point3f
{
	Point3f(float x, float y, float z) : X(x), Y(y), Z(z) {}
	float X, Y, Z;
};
#include <fstream> // for std::filebuf
#include <iterator> // for std::{i,o}streambuf_iterator

typedef QPair<objoff, objoff> NullIndex;
enum class ProcessMode{
	performance = 0,
	speed
};

class Obj3d
{
	int width, height;
	int NAN_VALUE = -9999;
	ProcessMode mode = ProcessMode::performance;
	ImageReader *reader;
	QString name;
	float *data[2];
#ifdef USE_ROW
	objoff *currNullRow;
	objoff *prevNullRow;
//	objoff* nullRow[2];
	objoff getIndex(int x, int y, int /*realH*/)
	{
		return y==0 ? prevNullRow[x/step] : currNullRow[x/step];
	}
#else
	QVector<NullIndex> nulls;

	objoff getIndex(int x, int y, int realH)
	{
		if (data[y][x] == NAN_VALUE)
			return 0;

		objoff st = width * realH / (step*step)  + x;

		objoff nullCount = 0;
		objoff poz = 1, total = nulls.size();
		for (; poz < total; ++poz)
			if (st < nulls[poz].first)
				break;

		nullCount = nulls[poz - 1].second;
		return st - nullCount;
	}
#endif

	int step = 1;
public:
	void setStep(int step) { this->step = step; }

//	Obj3d(int wid, int hei, int step = 1) : width(wid), height(hei), step(step)
//	{
//		//		width = 500;
//		//		height = 500;
//		data = new float *[hei];
//	}
	Obj3d(ImageReader *reader)
	{
		this->reader = reader;
		data[0] = nullptr;
		data[1] = nullptr;

		width = reader->widght();
		height =reader->height();

		//		data = new float *[hei];
	}

	bool check(objoff face[3]) { return face[0] != 0 && face[1] != 0 && face[2] != 0; }
	void write(QString path, Point3f scale = Point3f(0.1f, 0.1f, 0.1f))
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

		data[0] = nullptr;
		const objoff sWidth = width / this->step;
#ifndef USE_ROW
		objoff nullCounter = 0;
		objoff lastCount = 0;
		nulls.clear();
		// starts with 1
		nulls.push_back(NullIndex(0, -1));
#else
		objoff counter = 0;
		const objoff typeSize = sizeof(objoff);

		prevNullRow = new objoff[sWidth];
		currNullRow = new objoff[sWidth];

		memset(prevNullRow, 0, sWidth * typeSize);
		memset(currNullRow, 0, sWidth * typeSize);
#endif
		for (int h = 0; h < height; h += step)
		{
			if (h != 0)
			{
#ifndef USE_ROW
				nulls.remove(1, lastCount);
				lastCount = nulls.size() - 1;
#else
				objoff *temp = prevNullRow;
				prevNullRow = currNullRow;
				currNullRow = temp;
				memset(currNullRow, 0, sWidth * typeSize);
#endif
				if (data[0] != nullptr)
					delete[] data[0];
				data[0] = data[1];
			}

			data[1] = reinterpret_cast<float *>(reader->getRowData(h));

			QString vers;
			float *dataPointer = data[1];
			for (int w = 0; w < width; w += step, dataPointer+=step)
			{
				float value = *dataPointer;

				if (value == -9999)
				{
#ifndef USE_ROW
					++nullCounter;
#endif
					continue;
				}

#ifndef USE_ROW
				if (nullCounter != 0)
				{
					int add =  nulls[nulls.size() - 1].second;
					nulls.push_back(NullIndex( sWidth * h / step + w, nullCounter + add));
					nullCounter = 0;
				}
#endif

				if (value < min)
					min = value;
				if (value > max)
					max = value;

				sw.append("v " + normConv(w, scale.X) + " " + normConv(value, scale.Z) + " " + normConv(h, scale.Y) + nl);
				sw.append("vt " + QString::number((float)w/width) + " " + QString::number((float)h/height) + nl);

#ifdef USE_ROW
				currNullRow[w/step] = ++counter;
#endif

				if (h > 0 && w > 0)
				{
					//0*
					//00
					objoff i_tr = getIndex(w, 0, h - 1);
					//00
					//0*
#ifndef USE_ROW
					objoff i_br = (sWidth * h + w - nulls[nulls.size() - 1].second);
#else
					objoff i_br = getIndex(w, 1, h);

#endif
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
						Face3d::createVstr(face, face, 3, vers);
						sw.append(vers);
					}

					//10
					//23
					objoff face1[3]{i_tl, i_bl, i_br};
					if (check(face1))
					{
						Face3d::createVstr(face1, face1, 3, vers);
						sw.append(vers);
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
		delete[] data[0];
		delete[] data[1];
#ifdef USE_ROW
		delete[] prevNullRow;
		delete[] currNullRow;
#else
		nulls.clear();
#endif
	}


	QString normConv(float f, double scale)
	{
		f *= scale;
		//		if (f < .000001 || f > 100000)

		QString s = QString::number(f).replace(",", ".");
		return s.replace(",", ".");
	}

	~Obj3d() { clear(); }
	void clear()
	{
	}
	ProcessMode getMode() const;
	void setMode(const ProcessMode &value);
};

#endif // OBJ3D_H
