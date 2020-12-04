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

#ifdef _WIN32
QString nl = "\r\n";
#elif defined macintosh // OS 9
QString nl = "\r";
#elif
QString nl = "\n"; // Mac OS X uses \n
#endif

struct Point3f
{
	Point3f(float x,float y,float z):X(x), Y(y), Z(z){

	}
	float X, Y, Z;
};
#include <fstream>     // for std::filebuf
#include <iterator>    // for std::{i,o}streambuf_iterator

class Obj3d
{
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
	void setStep(int step) { this->step = step; }
	void setRow(int ind, float *rowI) { data[ind] = rowI; }

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

	bool check(objoff face[3])
	{
		return face[0]!=0 && face[1]!=0 && face[2]!=0;
	}

	void write(QString path, Point3f scale = Point3f(0.1, 0.1, 0.1))
	{
		QFile out(path);
		if (out.exists())
			out.remove();

		if (!out.open(QFile::WriteOnly | QFile::Truncate))
			return;

		QTextStream stream(&out);
		QTemporaryFile tempFile;
		tempFile.setAutoRemove(false);

		if (!tempFile.open())
			return;

		QTextStream faceStream(&tempFile);


		int BUFFER = 5000;
		QString sw;
		//(BUFFER, Qt::Initialization:);
		QString faces;
		//(BUFFER);
		sw = "";
		sw.append("o Main");
		sw.append(nl);
		qDebug() << data[0][0];
//		qDebug() << data[5000][5000];
		float min = 99999;
		float max = -99999;
		for (int h = 0; h < height; ++h)
		{
			for (int w = 0; w < width; ++w)
			{
				float value = data[h][w];
				if (value == -9999)
					continue;

				if (value < min)
					min = value;
				if (value > max)
					max = value;

				sw.append("v " + normConv(w, scale.X) + " "  + normConv(value, scale.Z) + " "+ normConv(h, scale.Y));
				sw.append(nl);


				if (h > 0 && w > 0)
				{
					//0*
					//00
					objoff i_tr = getIndex(w, h - 1, w, h);

					//00
					//0*
					objoff i_br = getIndex(w, h, w, h);

					//*0
					//00
					objoff i_tl = getIndex(w - 1, h - 1, w,h);

					//00
					//*0
					objoff i_bl = getIndex(w - 1, h, w, h);

					//32
					//01
					objoff face[3]{i_br, i_tr, i_tl};
					if (check(face))
					{
						Face3d f0(face, 3);
						sw.append(f0.buildStr());
						sw.append(nl);
						//						faces.append(f0.buildStr());
						//						faces.append(nl);
					}

					//10
					//23
					objoff face1[3]{i_tl, i_bl, i_br};
					if (check(face1))
					{
						Face3d f1(face1, 3);
						sw.append(f1.buildStr());
						sw.append(nl);
//						faces.append(f1.buildStr());
//						faces.append(nl);
					}

//					if (faces.length() >= BUFFER)
//					{
//						faceStream << faces;
//						faces.clear();
//					}
				}

				if (sw.length() >= BUFFER)
				{
					stream << sw;
					sw.clear();
				}
			}

			if (h%50 ==0) qDebug() << h;

			//delete[] data[h];
		}

		QImage img(width, height, QImage::Format_Grayscale8);
		for (int h = 0; h < height; ++h)
		{
			for (int w = 0; w < width; ++w)
			{
				float val = data[h][w];
//				val -= min;
//				val /= (max - min);
//				val *= 255;
				val = (((val - min) * 255) / (max - min)) + 0;

				img.setPixel(w, h, QColor(val, val, val).rgb());
			}
			delete[] data[h];
		}
		delete[] data;
		img.save("D:\\temp.bmp", "BMP");
		qDebug() << "Min:" << min << " Max:" << max;
		stream << sw;
		sw.clear();

		faceStream << faces;
		faces.clear();

		faceStream.seek(0);
		sw = faceStream.readAll();
		stream << sw;
		out.close();
		tempFile.close();
		tempFile.remove();
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
	//public  void drawGrid(Mat aside, Mat top, int xOff, int asideYoff, int topYoff, bool skipWall = false,int xShag=1,int rShag=1)
	//	{
	//		if (top.Width != aside.Width)
	//		{
	//			Cv2.Resize(aside, aside, new OpenCvSharp.Size(top.Width, aside.Height));
	//		}
	//		int k = 360 / rShag; ;


	//		int st = grid.Count;
	//		int last = top.Width - 1;
	//		for (int x = 0; x < top.Width; x+= xShag)
	//		{
	//			var yst = calc(top, x, topYoff);

	//			var zst = calc(aside, x, asideYoff);

	//			float zk = zst.r / (float)yst.r;
	//			int r = yst.r / 2;
	//			if (float.IsNaN(zk) || yst.r == 0)
	//			{
	//				zk = 0;
	//				//   continue;
	//			}
	//			if (name != "")
	//			{
	//				last = x;
	//			}
	//			for (int i = 0; i < 360; i+= rShag)
	//			{
	//				float dy = (float)(r * Math.Cos(i * 0.0174533)) * 2;
	//				float dz = (float)(r * Math.Sin(i * 0.0174533)) * zk * 2;//точно надо

	//				grid.Add(new Point3f(xOff + x, yst.middle - dy, zst.middle + dz));

	//				if (i > 0 && x > 0)
	//				{
	//					int num = grid.Count;
	//					faces.Add(new Face(num - 1, num, num - k, num - (k+1)));
	//				}
	//			}
	//			if (x > 0)
	//			{
	//				int num = grid.Count;
	//				faces.Add(new Face(num, num - (k-1), num -(k*2-1), num - k));
	//			}
	//		}

	//		int endNum = grid.Count;

	//		var yst0 = calc(top, last, topYoff);
	//		var zst0 = calc(aside, last, asideYoff);

	//		grid.Add(new Point3f(xOff + last, yst0.middle, zst0.middle));
	//		int middlePoint = endNum + 1;
	//		//end
	//		for (int i = endNum - k + 2; i <= endNum; i++)
	//		{
	//			faces.Add(new Face(i - 1, i, middlePoint));
	//		}
	//		faces.Add(new Face(endNum - k + 1, endNum, middlePoint));

	//		if (!skipWall)
	//		{//start
	//			yst0 = calc(top, 0, topYoff);
	//			zst0 = calc(aside, 0, asideYoff);

	//			grid.Add(new Point3f(xOff, yst0.middle, zst0.middle));
	//			middlePoint = grid.Count;

	//			for (int i = st + 2; i <= st + k; i++)
	//			{
	//				faces.Add(new Face(i - 1, i, middlePoint));
	//			}
	//			faces.Add(new Face(st + 1, st + k, middlePoint));

	//		}
	//	}

	~Obj3d() { clear(); }
	void clear()
	{
	}
};

#endif // OBJ3D_H
