#include "imagesearcher.h"
#include "src/core/project.h"

#include "opencv2/opencv.hpp"
#include "opencv2/core/mat.hpp"

ImageSearcher::ImageSearcher(TiffReader *reader) : reader(reader), image(nullptr)
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


#ifdef ENABLE_EXTRACT_BARIMG
	mat = new cv::Mat(reader->height(), reader->widght(), CV_8UC3);
#endif

	//	settings->bottomProc = 0.1f;
//	settings->coof= 1.7f;
//	settings->diamert = TRange<int>(10,300);
//	settings->height = TRange<float>(2,6);
}


DataRect ImageSearcher::getTile(int index)
{
	int tx = index % tilesInWid;
	int ty = index / tilesInWid;

	if (tx<0 || tx>=tilesInWid || ty<0 || ty>= tilesInHei)
		return nullptr;

	return reader->getRect(tx * tileWid, ty * tileHei, tileHei + diffset, tileWid + diffset);
}

DataRect ImageSearcher::getTile(int tx, int ty)
{
	if (tx<0 || tx>=tilesInWid || ty<0 || ty>= tilesInHei)
		return nullptr;

	return reader->getRect(tx * tileWid, ty * tileHei, tileHei + diffset, tileWid + diffset);
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
	int minX = points[0].getX(line->getWid()), maxX = points[0].getX(line->getWid());
	int minY = points[0].getY(line->getWid()), maxY = points[0].getY(line->getWid());
	// f255t0: end is the top, start is a bottom, e.g. [st,end]=[255,3]
	// float minT = line->end(), maxT = line->start;
	float maxT = line->start, minT =  line->start -  line->len;

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

		// Скорей всего не работает
		if (abs(val.value) < bottomPorog)//TODO CHECK CORRECT
			continue;
//		Wif (maxT - val.second >= 2)
//			continue;
		auto tempV = val.getX(line->getWid());
		if (minX > tempV)
			minX = tempV;

		if (maxX < tempV)
			maxX = tempV;

		tempV = val.getY(line->getWid());

		if (minY > tempV)
			minY = tempV;

		if (maxY < tempV)
			maxY = tempV;
	}

	boundy b(minX, minY, maxX, maxY);
	b.z = minT + bottomPorog;//TODO CHECK CORRECT
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


void ImageSearcher::mark(bc::barvector<float>& matr, int ind, const int &tx, const int &ty)
{
	static cv::Vec3b colors[8]{
		{0, 0, 255},
		{0, 255, 0},
		{255, 0, 0},
		{0, 255, 255},
		{255, 0, 255},
		{255, 255, 0},
		{0, 255, 255},
		{255, 255, 255},
		};

	int tileWidCur = this->tileWid + (tx == (tilesInWid - 1)  * tileWid ? 0 : this->diffset);
	const auto &col = colors[ind % 8];
	for (auto& a : matr)
	{
		int x = tx + a.getX(tileWidCur);
		int y = ty + a.getY(tileWidCur);
		assert(x < this->reader->widght() && y < reader->height());
		// Если хотя бы одно не 0, значи тут уже есть цвет
		auto &colref = mat->at<cv::Vec3b>(y, x); //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		if (colref[0] == 0 && colref[1] == 0 && colref[2] == 0)
		{
			colref[0] = col[0];
			colref[1] = col[1];
			colref[2] = col[2];
		}
	}
}

void ImageSearcher::segment(bc::barline<float> *line, int i, const int &tx, const int &ty)
{

	for(auto& c: line->childrens)
	{
		segment(c, i+1, tx,ty);
	}
	mark(line->matr, i, tx, ty);
}

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

		// barsOut.writeLine("{num:" + tileNumstr + ", barcodes: { bars:[ ");

		DataRect img = getTile(i);
		int wid = img.wid, hei = img.hei;

		bc::BarcodeCreator<float> creator;

		Barcontainer<float> *bars = creator.searchHoles(img.data, wid, hei);
		Baritem<float> *item = bars->getItem(0);

		int tx = (i % tilesInWid) * tileWid;
		int ty = (i / tilesInWid) * tileHei;
		std::string out = "[ ";
		size_t addded = 0;

//#ifdef ENABLE_EXTRACT_BARIMG
//		segment(item->getRootNode(), 0, tx, ty);
//#endif

		for (size_t ib = 0, total = item->barlines.size(); ib < total; ++ib)
		{
			bc::barline<float> *line = item->barlines[ib];

#ifdef ENABLE_EXTRACT_BARIMG
			mark(line->matr, ib, tx, ty);
#endif

			if (line->childrens.size() != 0)
				continue;

			// float deep = 0;
			// bc::barline<float> *temp = line->parrent;
			// while (temp && temp != item->getRootNode())
			// {
			// 	deep += abs(temp->len);
			// 	temp = temp->parrent;
			// }

			// if (deep> abs(line->len))
			// 	continue;

			boundy b = getBounty(line);

			if (round(b.x)<=1.0 || round(b.y<=1.0) || round(b.endX) >= (wid-2) || round(b.endY)>=(hei-2))
				continue;

			b.addXoffset(tx);
			b.addYoffset(ty);

			// if (!Project::checkHolm(b, img, tx, ty))
			// 	continue;

			boundsOut.writeLine(b.getStr());
			addded += 1;

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

		totalAdded += addded;

		delete bars;
		img.release();

		// delete data and delete pointer on it
		reader->removeTileFromCache(i);

		if (valStop)
			break;
	}

	qDebug() << "st:" << start << "ed:" << len << "size:" << totalAdded;
	return totalAdded;
}



float sqr(float X)
{
	return ((float) (X) * (X));
}

void show(cv::Mat &mat)
{
	cv::destroyAllWindows();
	cv::namedWindow("res", cv::WINDOW_NORMAL);
	cv::imshow("res", mat);
	cv::waitKey(1);
}


struct Direction
{
	static const int COUNT = 4;
	Direction() { memset(dirs, 0, COUNT * sizeof(int)); }
	int dirs[COUNT];
	int totalSum = 0;
	void add(int x, int y)
	{
		if (x == 0 && y == 0)
		{
			qDebug() << "Ass";
		}
		//(-1 -1) = 0; (-1  0) = 1; (-1 1) = 2; (0 -1) = 3;
		//( 0  1) = 5; ( 1 -1) = 6; ( 1 0) = 7; (1  1) = 8;
		int off = (y + 1) * 3 + (x + 1);
		if (off < 4)
			++dirs[off];
		else
			--dirs[8 - off];

//		totalSum += (abs(x) + abs(y));
		totalSum += 1;
	}

	float sums()
	{
		int sm = 0;
		for (int i = 0; i < COUNT; ++i)
		{
			sm += abs(dirs[i]);
		}
		return 1.0f -  static_cast<float>(sm) / totalSum;
	}
};

struct DirectionCircle
{
	static const int COUNT = 8;
	DirectionCircle() { memset(dirs, 0, COUNT * sizeof(int)); }
	int dirs[COUNT];
	int totalSum = 0;
	void add(int x, int y)
	{
		if (x == 0 && y == 0)
		{
			qDebug() << "Ass";
		}
		//(-1 -1) = 0; (-1  0) = 1; (-1 1) = 2; (0 -1) = 3;
		//( 0  1) = 5; ( 1 -1) = 6; ( 1 0) = 7; (1  1) = 8;
		int off = (y + 1) * 3 + (x + 1);
		++dirs[off];

		//		totalSum += (abs(x) + abs(y));
		totalSum += 1;
	}

	float sums()
	{
		int sm = 0;
		int as_avg = totalSum / COUNT;
		for (int i = 0; i < COUNT; ++i)
		{
			sm += abs(dirs[i] - as_avg);
		}
		return  1.0f -  static_cast<float>(sm) / totalSum;
	}
};

bool cehckCoof(float a, float b)
{
	float coof = (a > b ? a / b : b / a);
	return coof <= 2.0;
}

bool ImageSearcher::checkCircle(DataRect &ret, float hei, float coof)
{
	cv::Mat mat(ret.hei, ret.wid, CV_8UC1);

	float maxval = -9999;
	for (int j = 0, total = ret.hei *  ret.wid; j < total; ++j)
	{
		float d = ret.get(j);
		if (d > maxval)
		{
			maxval = d;
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

	if (contours.size() > 2 || contours.size() == 0)
		return false;

	int cur = (contours.size() == 2 && contours[1].size() > contours[0].size()) ? 1 : 0;
	size_t size = contours[cur].size();

	DirectionCircle realsum;
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

	int dx = maxx - minx;
	int dy = maxy - miny;

//	show(mat);
	float tcoof =realsum.sums();
	return tcoof >= coof && cehckCoof(dx, dy);
	//	return coofX >= ds && coofY >= ds;
}
#include <math.h>


bool ImageSearcher::checkCircle2(DataRect &ret, float hei, float coof)
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

void ImageSearcher::savemat()
{
#ifdef ENABLE_EXTRACT_BARIMG
	QString ds = Project::proj->getPath(BackPath::root);
	cv::imwrite((ds + "barvision.jpg").toStdString(), *mat);
	mat->release();
#endif
}

ImageSearcher::~ImageSearcher()
{
#ifdef ENABLE_EXTRACT_BARIMG
	delete mat;
#endif
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

DataRect Beaf::importBeafData(const QString &path)
{
	QFile file(path);

	file.open(QFile::OpenModeFlag::ReadOnly | QFile::OpenModeFlag::ExistingOnly);

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
	DataRect ret((float*)flaotData, w, h);

	return ret;
}

