#ifndef IMAGESEARCHER_H
#define IMAGESEARCHER_H
#include "../base.h"
#include "tiffreader.h"

#include <QObject>
#include <vector>

#include <src/types/types.h>
#include <src/types/img.h>
#include "side-src/Barcode/PrjBarlib/include/barcodeCreator.h"
using namespace bc;


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
	float bottomProc;

	SeachingSettings()
	{
		// coof = 0;
		// diamert = {0, 0};
		// height = {0, 0};
		// bottomProc = 0;
		
		bottomProc = 0.1f;
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

class ImageSearcher
{
	TiffReader *reader;
    int tileWid;
    int tileHei;
    int tilesInWid;
    int tilesInHei;
	float *image;
	// наложение одного тайла на другой
	int diffset = 100;

	Img getTile(int index);

	PointerCache<Img *> cachedTiles;
public:
	ImageSearcher(TiffReader *reader);

    uint getTilesInWid()
    {
        return tilesInWid;
    }
    uint getTilesInHei()
    {
        return tilesInHei;
    }
	int getMaxTiles();
	void findZones(std::vector<boundy> &bounds, int start, int len);

	bool checkCircle(boundy &bb);

	Img getTile(int tx, int ty);
	Img *getRect(boundy &bb);
};

#endif // IMAGESEARCHER_H
