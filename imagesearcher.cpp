#include "imagesearcher.h"

ImageSearcher::ImageSearcher(TiffReader *reader): reader(reader)
{

}

void ImageSearcher::getTile(int ind)
{
	if (reader->tiff.TileWidth != 0)
	{
//		reader->getRowData()
	}

}

int ImageSearcher::getMaxTiles()
{
	return reader->widght() * reader->height() / (tileWid * tileHei);
}

void ImageSearcher::findZones(QVector<boundy> &bounds)
{
	if (reader->tiff.TileWidth != 0)
	{
		tileWid = reader->tiff.TileWidth + 100;
		tileHei = reader->tiff.TileLength + 100;
	}
	else
	{
		tileWid = 1000;
		tileHei = 1000;
	}
//	for (int var = 0; var < total; ++var) {

//	}

}
