#include "imagesearcher.h"

#include <opencv2/opencv.hpp>
#include "opencv2/core/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#ifdef USE_OPENCV
using namespace cv;
#endif


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

	tilesInWid = reader->widght() / tileWid;
	tilesInHei = reader->height() / tileHei;
	qDebug() << tilesInWid * tilesInHei;

//	settings->bottomProc = 0.1f;
//	settings->coof= 1.7f;
//	settings->diamert = TRange<int>(10,300);
//	settings->height = TRange<float>(2,6);
}

constexpr int getTid(int tx, int ty, int wid)
{
	return ty* wid + tx;
}

//Img ImageSearcher::getTile(int tx, int ty)


Img ImageSearcher::getTile(int index)
{
	int tx = index % tilesInWid;
	int ty = index / tilesInWid;

	if (tx<0 || tx>=tilesInWid || ty<0 || ty>= tilesInHei)
		return nullptr;

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

			for (int i = 0; i < tileWid * tileHei; ++i)
				img.data[i] = -9999;


			uchar *tile = reader->getTile(ty * tilesInWid + tx);

			float *data = static_cast<float *>(reader->processData(tile));

			for (int j = 0; j < lenY; ++j)
			{
				int fromdataOff = j * tileWid;
				img.setInRow(istY + j, istX, data + fromdataOff, lenX);
			}
		}
//load 9
// * * *
// * * *
// * * *
		return img;
//		reader->getRowData()
	}

	Img img(tileWid + diffset, tileHei + diffset);
	for (int i = 0; i < tileWid * tileHei; ++i)
		img.data[i] = -9999;

	// Pixel    "st" is a column number is source image
	// *  *  *
	// *  st *
	// *  *  *
	int st = tileWid * tx;
	int len = (tx == tilesInWid - 1 ? tileWid : tileWid + diffset);

	// Pixel  "i" is a row number in source image
	// * * *
	// i * *
	// * * *
	for (int i = ty * tileHei, total = min((ty + 1) * tileHei + diffset, reader->height()), k = 0; i < total; ++i, ++k)
	{
		img.setInRow(k, 0, static_cast<float*>(reader->getRow(i)) + st, len);
	}
	return img;
}

int ImageSearcher::getMaxTiles()
{
	return reader->widght() * reader->height() / (tileWid * tileHei);
}
#include "barcodeCreator.h"
using namespace bc;


void findBootm()
{}
boundy getBounty(pmap &points)
{
	int minX = points[0].first.x, maxX = points[0].first.x;
	int minY = points[0].first.y, maxY = points[0].first.y;
	float minT = points[0].second, maxT = points[0].second;
	for (size_t i = 1, total = points.size(); i < total; ++i)
	{
		ppair &val = points[i];

		if (minT > val.second)
			minT = val.second;

		if (maxT < val.second)
			maxT = val.second;

	}
// если поставить 10, то слипшийся покажется
	float bottomLvl = minT + (maxT - minT) / 10;

	minT = maxT;
	for (size_t i = 0, total = points.size(); i < total; ++i)
	{
		ppair &val = points[i];
		if(val.second<bottomLvl)
			continue;
//		if (maxT - val.second >= 2)
//			continue;

		if (minX > val.first.x)
			minX = val.first.x;

		if (maxX < val.first.x)
			maxX = val.first.x;

		if (minY > val.first.y)
			minY = val.first.y;

		if (maxY < val.first.y)
			maxY = val.first.y;

		if (minT > val.second)
			minT = val.second;
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
void ImageSearcher::findZones(vector<boundy> &bounds, int start, int len)
{


	//!!!!!!!!!!!!!!!!!!!!!!!
//	diffset = 0;
//	tileWid = reader->widght();
//	tileHei = reader->height();
	//!!!!!!!!!!!!!!!!!!!!!


	//	image = new float[(tileWid + diffset) * (tileHei + diffset)];

//	start = 0;//519;
//	start = 519;
	for (int i = start, totalTiles = MIN(start + len, tilesInWid * tilesInHei); i < totalTiles; ++i)
	{
//		qDebug() << i;
		Img img = getTile(i);
		int wid = img.wid, hei = img.hei;

		bc::barcodeCreator creator;

		Barcontainer *bars = creator.searchHoles(img.data, wid, hei);
		Baritem *item = bars->get(0);
		img.release();

		int tx = (i % tilesInWid) * tileWid;
		int ty = (i / tilesInWid) * tileHei;

		for (size_t i = 0, total = item->bar.size(); i < total; ++i)
		{
			bline *line = item->bar[i];
//			check(line->matr);
			boundy b = getBounty(*line->matr);


			b.addXoffset(tx);
			b.addYoffset(ty);
			bounds.push_back(b);
		}

		boundy b(0, 0, wid-1,  hei-1);
		b.addXoffset(tx);
		b.addYoffset(ty);

		b.z = 0;
		b.endZ = 50;
//		bounds.push_back(b);
		qDebug() <<"size:" <<bounds.size();
		delete bars;
	}

}
