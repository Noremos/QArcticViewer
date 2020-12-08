#ifndef IMAGESEARCHER_H
#define IMAGESEARCHER_H
#include "tiffreader.h"
struct boundy
{
	uint x, y, wid, hei;
};

class ImageSearcher
{
	TiffReader *reader;
	int tileWid;
	int tileHei;
public:
	ImageSearcher(TiffReader *reader);

	void getTile(int ind);
	int getMaxTiles();

	void findZones(QVector<boundy> &bounds);
};

#endif // IMAGESEARCHER_H
