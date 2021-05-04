#include "imagesearcher.h"
#include "src/core/project.h"

ImageSearcher::ImageSearcher(TiffReader *reader): reader(reader)
{
	if (reader->tiff.TileWidth != 0)
	{
		tileWid = reader->tiff.TileWidth;
		tileHei = reader->tiff.TileLength;
		reader->setTitleCacheSize(9);
	}
	else
	{
		tileWid = 1000;
		tileHei = 1000;
		reader->setRowsCacheSize(tileHei);
	}

    tilesInWid = (uint)reader->widght() / tileWid;
    tilesInHei = (uint)reader->height() / tileHei;
	qDebug() << tilesInWid * tilesInHei;
	cachedTiles.setMaxSize(10);

//	settings->bottomProc = 0.1f;
//	settings->coof= 1.7f;
//	settings->diamert = TRange<int>(10,300);
//	settings->height = TRange<float>(2,6);
}

constexpr int getTid(int tx, int ty, int wid)
{
	return ty* wid + tx;
}

Img ImageSearcher::getTile(int index)
{
	int tx = index % tilesInWid;
	int ty = index / tilesInWid;
	return getTile(tx,ty);
}

Img ImageSearcher::getTile(int tx, int ty)
{

	if (tx<0 || tx>=tilesInWid || ty<0 || ty>= tilesInHei)
		return nullptr;

	int index = ty * tilesInWid + tx;
#ifdef IMPPART
	if (Project::getProject()->isTileCached(index))
	{
		return importBeafData(Project::getProject()->getTilePath(index));
	}
#endif


	if (reader->tiff.TileWidth != 0)
	{

		char offs[4][2] = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
		Img img(tileWid + diffset, tileHei + diffset);


		for (int i = 0; i < 4; ++i)
		{
			int tcurX = tx + offs[i][0];
			int tcurY = ty + offs[i][1];
			if (tcurX < 0 || tcurY < 0 || tcurX >= tilesInWid || tcurY >= tilesInHei)
				continue;

			// in img
			int istX = 0, istY = 0;

			int lenX = tileWid,
				lenY = tileHei;

			if (offs[i][0] == 1)
			{
				istX = tileWid;
				lenX = diffset;
			}
			if (offs[i][1] == 1)
			{
				istY = tileHei;
				lenX = diffset;
			}

			for (int i = 0; i < img.wid * img.hei; ++i)
				img.data[i] = -9999;


			float *data = reader->getTile(index);

			for (int j = 0; j < lenY; ++j)
			{
				int fromdataOff = j * tileWid;
				img.setInRow(istY + j, istX, data + fromdataOff, lenX);
			}
		}
#ifdef IMPPART
		exportDataAsBeaf(Project::getProject()->getTilePath(index), img.wid, img.hei, img.data);
#endif
//load 9
// * * *
// * * *
// * * *
		return img;
//		reader->getRowData()
	}

	Img img(tileWid + diffset, tileHei + diffset);
	for (int i = 0; i < img.wid * img.hei; ++i)
		img.data[i] = -9999;

	// Pixel    "st=columnNum" is a column number is source image
	// *  *  *
	// *  st *
	// *  *  *
	int columnNum = tileWid * tx;
	int len = (tx == tilesInWid - 1 ? tileWid : tileWid + diffset);

	// Pixel  "i=firstRow" is a row number in source image
	// * * *
	// i * *
	// * * *
	int firstRow = ty * tileHei;
	int lastRow = std::min((ty + 1) * tileHei + diffset, reader->height());
	for (int rowInDest = 0; firstRow < lastRow; ++firstRow, ++rowInDest)
	{
		float *data = reader->getRow(firstRow);
		img.setInRow(rowInDest, 0, data + columnNum, len);
	}

#ifdef IMPPART
	exportDataAsBeaf(Project::getProject()->getTilePath(index), img.wid, img.hei, img.data);
#endif

	return img;
}

int ImageSearcher::getMaxTiles()
{
	return tilesInWid* tilesInHei;
//	return reader->widght() * reader->height() / (tileWid * tileHei);
}

void findBootm()
{}
boundy ImageSearcher::getBounty(barline<float> *line)
{
	auto &points = line->matr;
	int minX = points[0].getX(), maxX = points[0].getX();
	int minY = points[0].getY(), maxY = points[0].getY();
	// f255t0: end is the top, start is a bottom, e.g. [st,end]=[255,3]
	// float minT = line->end(), maxT = line->start;
	float minT = line->start, maxT = line->end();

	if (minT > maxT)
	{
		float temp = minT;
		minT = maxT;
		maxT = temp;
	}
// если поставить 10, то слипшийся покажется
    float bottomPorog = (maxT - minT) / bottomLevel;

	for (size_t i = 0, total = points.size(); i < total; ++i)
	{
		barvalue<float> &val = points[i];
        if(abs(val.value) < bottomPorog)
			continue;
//		Wif (maxT - val.second >= 2)
//			continue;

		if (minX > val.getX())
			minX = val.getX();

		if (maxX < val.getX())
			maxX = val.getX();

		if (minY > val.getY())
			minY = val.getY();

		if (maxY < val.getY())
			maxY = val.getY();
	}

	boundy b(minX, minY, maxX, maxY);
	b.z = minT;
	b.endZ = maxT;

//	b.sizeWid = (maxX - minX) * resol;
//	b.sizeHei = (maxY - minY) * resol;
//	b.sizeTop = (maxT - minT);
//	if (b.sizeTop > 8)
//		qDebug() << maxT << " " << minT;
	return b;
}

void check(void *ptr)
{
	if (ptr == nullptr)
		qDebug() << "PTR IS NULL";
}
#include "fstream"

#include <QColor>
#include <QImage>
#include <QPixmap>

size_t ImageSearcher::findROIs(FileBuffer &boundsOut, FileBuffer &barsOut,
							   int start, int len, int bottom,
							   volatile bool &valStop)
{
	//!!!!!!!!!!!!!!!!!!!!!!!
//	diffset = 0;
//	tileWid = reader->widght();
//	tileHei = reader->height();
	//!!!!!!!!!!!!!!!!!!!!!

	this->bottomLevel = bottom;
//	start = 0;//519;
//	start = 519;
	size_t totalAdded = 0;
	for (int i = start, totalTiles = MIN(start + len, tilesInWid * tilesInHei); i < totalTiles; ++i)
	{
		QString tileNumstr = QString::number(i);
		boundsOut.writeLine("t " + tileNumstr);
		// barsOut.writeLine("{num:" + tileNumstr + ", barcodes:");

		Img img = getTile(i);
		int wid = img.wid, hei = img.hei;

		bc::BarcodeCreator<float> creator;

		Barcontainer<float> *bars = creator.searchHoles(img.data, wid, hei);
		Baritem<float> *item = bars->getItem(0);

		// barsOut.writeLine("{");
		// barsOut.write("bars:[ ");


		// delete data and delete pointer on it
		img.release();
		reader->removeTileFromCache(i);


		int tx = (i % tilesInWid) * tileWid;
		int ty = (i / tilesInWid) * tileHei;

		//boundy b(0, 0, wid-1,  hei-1);
		//b.addXoffset(tx);
		//b.addYoffset(ty);
		//b.z = 0;
		//b.endZ = 50;
		//bounds.push_back(b);
		std::string out = "[ ";
		for (size_t i = 0, total = item->barlines.size(); i < total; ++i)
		{
			bc::barline<float> *line = item->barlines[i];
//			check(line->matr);
			boundy b = getBounty(line);

			b.addXoffset(tx);
			b.addYoffset(ty);

			boundsOut.writeLine(b.getStr());


			// line->getJsonObject(out);
			// out += ",";
		}
		// out[out.length() - 1] = ']';

		// QString swr = QString::fromStdString(out);
		// swr += nl;
		// if (i != totalTiles - 1)
		// 	swr += "},";
		// else
		// 	swr += "}";

		// barsOut.writeLine(swr);

		totalAdded += item->barlines.size();

		delete bars;

		if (valStop)
			break;
	}
	qDebug() << "st:" << start << "ed:" << len << "size:" << totalAdded;
	return totalAdded;
}

#include <opencv2/opencv.hpp>
//#include <opencv2/imgproc.hpp>

#include <opencv2/core/mat.hpp>

float sqr(float X)
{
	return ((float) (X) * (X));
}

void show(cv::Mat &mat)
{
	cv::namedWindow("res", cv::WINDOW_NORMAL);
	cv::imshow("res", mat);
	cv::waitKey(1);
}


struct Direction
{
	static const int COUNT = 4;
	Direction() { memset(dirs, 0, COUNT * sizeof(int)); }
	int dirs[COUNT];
	void add(int x, int y)
	{
		if (x == 0 && y == 0)
		{
			qDebug() << "Ass";
		}
		//(-1 -1) = 0; (-1 0) = 1; (-1 1) = 2; (0 -1) = 3;
		//(0 1) = 5; (1 -1) = 6; (1 0) = 7; (1 1) = 8;
		int off = (y + 1) * 3 + (x + 1);
		if (off < 4)
			++dirs[off];
		else
			--dirs[off-5];
	}

	int sums()
	{
		int sm = 0;
		for (int i = 0; i < COUNT; ++i)
		{
			sm += dirs[i];
		}
		return sm;
	}
};

bool ImageSearcher::checkCircle(Img &ret, float hei, float coof)
{
	cv::Mat mat(ret.hei, ret.wid, CV_8UC1);
	float maxval = -9999;
	for (int j = 0; j < ret.hei; ++j)
	{
		for (int i = 0; i < ret.wid; ++i)
		{
			float d = ret.get(i, j);
			if (d > maxval)
			{
				maxval = d;
			}
		}
	}

	maxval -= hei;
	for (int j = 0; j < ret.hei; ++j)
	{
		for (int i = 0; i < ret.wid; ++i)
		{
			float d = ret.get(i, j);
			mat.at<uchar>(j, i) = (d > maxval) ? 255 : 0;
		}
	}
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(mat, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);

	if (contours.size() > 2)
		return false;

	int cur = (contours.size() == 2 && contours[1].size() > contours[0].size()) ? 1 : 0;
	size_t size = contours[cur].size();

	Direction realsum;
	cv::Point prevc = contours[cur][0];
	int minx = contours[cur][0].x, miny = contours[cur][0].y, maxx = contours[cur][0].x, maxy = contours[cur][0].y;
	for (size_t i = 1; i < size; ++i)
	{
		auto c = contours[cur][i];
		minx = MIN(minx, c.x);
		maxx = MAX(maxx, c.x);

		miny = MIN(miny, c.y);
		maxy = MAX(maxy, c.y);

		realsum.add((c.x - prevc.x), (c.y - prevc.y));

		prevc = c;
	}

	show(mat);
	float tcoof = 1.0f - (realsum.sums() / size);
	return tcoof >= coof;
	//	return coofX >= ds && coofY >= ds;
}
#include <math.h>


bool ImageSearcher::checkCircle2(Img &ret, float hei, float coof)
{
	cv::Mat mat(ret.hei, ret.wid, CV_8UC1);
	float maxval = -9999;
	for (int j = 0; j < ret.hei; ++j)
	{
		for (int i = 0; i < ret.wid; ++i)
		{
			float d = ret.get(i, j);
			if (d > maxval)
			{
				maxval = d;
			}
		}
	}

	maxval -= hei;
	for (int j = 0; j < ret.hei; ++j)
	{
		for (int i = 0; i < ret.wid; ++i)
		{
			float d = ret.get(i, j);
			mat.at<uchar>(j, i) = (d > maxval) ? 255 : 0;
//			mat.at<uchar>(j, i) = 255;
		}
	}
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(mat, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);

	if (contours.size() > 2)
		return false;

	cv::namedWindow("res", cv::WINDOW_NORMAL);
	size_t step = 5;
	int totoal = 0;
	float baseAngel = 0;

	float coofPi = 180.f / M_PI;

	for (size_t i = step*2; i < contours[0].size(); i += step)
	{
		auto prev = contours[0][i - step*2];
		auto center = contours[0][i - step];
		auto cur = contours[0][i];
		++totoal;


		prev -= center;
		cur -= center;
		/*
		cos α = a·b
			  |a|·|b|
		 */

		//a.b
		float ab = prev.x * cur.x + prev.y * cur.y;

		// |a|·|b|
		float labl = sqrt(prev.x * prev.x + prev.y * prev.y) * sqrt(cur.x * cur.x + cur.y * cur.y);

		// [0, 180].
		float res = std::acos(ab / labl) * coofPi;
		if (i != step*2 && (res - baseAngel) > 30)
		{
			prev += center;
			cur += center;
			cv::Mat md;
			cv::cvtColor(mat, md, cv::COLOR_GRAY2BGR);
			md.at<cv::Vec3b>(prev.y, prev.x)[1] = 0;
			md.at<cv::Vec3b>(prev.y, prev.x)[0] = 255;
			md.at<cv::Vec3b>(center.y, center.x)[1] = 255;
			md.at<cv::Vec3b>(center.y, center.x)[0] = 0;
			md.at<cv::Vec3b>(cur.y, cur.x)[1] = 0;
			md.at<cv::Vec3b>(cur.y, cur.x)[0] = 255;
			md.at<cv::Vec3b>(contours[0][i - step*3].y, contours[0][i - step*3].x)[1] = 0;
			md.at<cv::Vec3b>(contours[0][i - step*3].y, contours[0][i - step*3].x)[0] = 255;
			cv::imshow("res", md);
			cv::waitKey(1);
			return false;
		}

		baseAngel = res;
	}
	cv::imshow("res", mat);
	cv::waitKey(1);
	return true;
}

void Beaf::exportDataAsBeaf(const QString &path, int wid, int hei, float *data)
{
	//		 EXPOT AS RAW
	QFile out(path);
	if (out.exists())
		out.remove();

	if (!out.open(QFile::WriteOnly | QFile::Truncate))
		return;

	int size[] = {wid, hei};
	char *byteArr = reinterpret_cast<char *>(size);
	out.write("b", 1);
	out.write(byteArr, 2*sizeof(int));

	out.write((char *)data, wid * hei * sizeof(float));
	out.close();
}

void Beaf::exportDataAsPng(const QString &path, bc::BarImg<float> &bimg)
{
	//SAVE PNG
	QImage imggray(bimg.wid(), bimg.hei(), QImage::Format::Format_Grayscale8);

	float max = bimg.max();
	float min = bimg.min();
	float maxmin = max - min;
	for (int y = 0; y < bimg.hei(); ++y)
	{
		for (int x = 0; x < bimg.wid(); ++x)
		{
			int grayVal = 255 * ((bimg.get(x, y)-min) / maxmin);
			grayVal = MAX(0, MIN(255, grayVal));
			imggray.setPixelColor(x, y, QColor(grayVal));
		}
	}
	imggray.save(path);
}

void Beaf::exportHeightAsPng(const QString &path, const bc::BarImg<float> &bimg, bool vert)
{
	int wids = bimg.wid();
	int heis = bimg.hei();


	float max = bimg.max();
	float min = bimg.min();
	float maxmin = max - min;

	heis = maxmin * 100  + 1;

	QImage imggray(wids, heis, QImage::Format::Format_Grayscale8);
	imggray.fill(QColor(0,0,0));

	for (int y = 0; y < bimg.hei(); ++y)
	{
		for (int x = 0; x < bimg.wid(); ++x)
		{
			int grayVal = (maxmin - (bimg.get(x, y) - min)) * 100;

			QRgb val = imggray.pixel(x, grayVal);
			QColor col(val);
			col.setRed( col.red() + 1);
			col.setRed( 255);
			imggray.setPixelColor(x, grayVal, col);
		}
	}
	imggray.save(path);
}


void Beaf::exportDataAsPngInner(const QString &path, int wid, int hei, float *data)
{
	bc::BarImg<float> bimg(wid, hei, 1, (uchar *) data, false, false);
	exportDataAsPng(path, bimg);
}

union conv
{
	char data[4];
	float val;
	int ival;
};


bc::BarImg<float> Beaf::getFromRawData(int wid, int hei, float *data)
{
	return bc::BarImg<float>(wid, hei, 1, (uchar *) data, false, false);
}

Img Beaf::importBeafData(const QString &path)
{
	QFile file(path);

	file.open(QFile::OpenModeFlag::ReadOnly | QFile::OpenModeFlag::ExistingOnly);

	QByteArray arr = file.readAll();

	char type[8];
	file.read(type, 1);
	int w = 60, h = 60;
	if (type[0] == 'b')
	{
		conv con1;

		file.read(type, 8);
		memcpy(con1.data, type, 4);

		w = con1.ival;

		memcpy(con1.data, type + 4, 4);
		h = con1.ival;
	}
	char *flaotData = new char[w * h + 1];
	file.read(flaotData, w * h);
	Img ret((float*)flaotData, w, h);

	return ret;
}

