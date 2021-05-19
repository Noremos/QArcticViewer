#ifndef IMAGESEARCHER_H
#define IMAGESEARCHER_H
#include "../base.h"
#include "subclasses.h"
#include "tiffreader.h"

#include <QFile>
#include <QObject>
#include <vector>

#include <src/types/types.h>
#include <src/types/img.h>
#include "side-src/Barcode/PrjBarlib/include/barcodeCreator.h"
using namespace bc;


class Beaf
{
public:
	static bc::BarImg<float> getFromRawData(int wid, int hei, float *data);
	static Img importBeafData(const QString &path);
	static void exportDataAsBeaf(const QString &path,  int wid, int hei, float *data);
	static void exportDataAsPng(const QString &path, bc::BarImg<float> &bimg);
	static void exportHeightAsPng(const QString &path, const bc::BarImg<float> &bimg, bool vert);
	static void exportHeightAsPngInner(const QString &path, bc::BarImg<float> &bimg);
	static void exportDataAsPngInner(const QString &path, int wid, int hei, float *data);
};

class SeachingSettings : public QObject
{
public:
	Q_OBJECT
	Q_PROPERTY(float coof MEMBER coof)
	Q_PROPERTY(int diamertMin READ diametrMin WRITE setDiametrMin)
	Q_PROPERTY(int diamertMax READ diametrMax WRITE setDiametrMax)
	Q_PROPERTY(float heightMin READ heightMin WRITE setHeightMin)
	Q_PROPERTY(float heightMax READ heightMax WRITE setHeightMax)
	Q_PROPERTY(float bottomProc MEMBER bottomProc)

public:
	float coof;
	TRange<int> diamert;
	TRange<float> height;
	int bottomProc;

	SeachingSettings()
	{
		// coof = 0;
		// diamert = {0, 0};
		// height = {0, 0};
		// bottomProc = 0;

		bottomProc = 10;
		coof = 1.7f;
		diamert = TRange<int>(10, 400);
		height = TRange<float>(2, 6);
	}
	// FOR PORPERTYs

	int diametrMin() { return diamert.start; }

	int diametrMax() { return diamert.end; }
	float heightMin() { return height.start; }
	float heightMax() { return height.end; }


public slots:
	void setDiametrMin(int val) { diamert.start = val;}
	void setDiametrMax(int val) { diamert.end = val;}
	void setHeightMin(float val)  { height.start = val;}
	void setHeightMax(float val)  { height.end = val;}
	void setCoof(float val)  { coof = val;}
	void setBottomProc(float val)  { bottomProc = val;}


};

namespace cv
{
	class Mat;
}

class ImageSearcher
{
//	cv::Mat* mat;

	TiffReader *reader;
    int tileWid;
    int tileHei;
    int tilesInWid;
    int tilesInHei;
	float *image;
	// наложение одного тайла на другой
	int diffset = 100;

	// Процент отсечения снизу
	int bottomLevel = 0;


	PointerCache<Img *> cachedTiles;
	boundy getBounty(barline<float> *line);
public:
	ImageSearcher(TiffReader *reader);

	constexpr int getTileOffset(int tx, int ty)
	{
		return ty* tilesInWid + tx;
	}

	uint getTilesInWid()
    {
		return tilesInWid;
	}
	uint getTilesInHei()
    {
		return tilesInHei;
	}
	int getMaxTiles();
	size_t findROIs(FileBuffer &bounds, FileBuffer &bars, int start, int len, int bottom, volatile bool &valStop);

	void setFillTileRowCache()
	{
		reader->setRowsCacheSize(tileHei + diffset + 10);
	}

	bool checkCircle(Img &ret, float hei, float coof);

	Img getTile(int index);
	Img getTile(int tx, int ty);

	Size2 getTileSize()
	{
		return Size2(tileWid, tileHei);
	}

	void getOffset(int index, int &x, int &y)
	{
		int tx = index % tilesInWid;
		int ty = index / tilesInWid;
		x = tx * tileWid;
		y = ty * tileHei;
	}

    void getFileOffset(int index, boundy bb, int& x, int &y)
    {
        // diffset 100
        int startX = index % tilesInWid;
//        int endX = bb.endX / tileWid;
        int startY = index / tilesInWid;
//        int endY = bb.endY / tileHei;

       x = (int)bb.x - startX * tileWid;
	   y = (int)bb.y - startY * tileHei;
	}
	bool checkCircle2(Img &ret, float hei, float minDiametr);

	void savemat();
	~ImageSearcher();
	void segment(bc::barline<float> *line, int i, const int &tx, const int &ty);
protected:
	void mark(bc::barvector<float> &mart, int ind, const int &tx, const int &ty);
};

#endif // IMAGESEARCHER_H
