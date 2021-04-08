#include "imagesearcher.h"

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

void findBootm()
{}
boundy getBounty(barvector<float> &points)
{
	int minX = points[0].getX(), maxX = points[0].getX();
	int minY = points[0].getY(), maxY = points[0].getY();
	float minT = points[0].value, maxT = points[0].value;
	for (size_t i = 1, total = points.size(); i < total; ++i)
	{
		bc::barvalue<float> &val = points[i];

		if (minT > val.value)
			minT = val.value;

		if (maxT < val.value)
			maxT = val.value;

	}
// если поставить 10, то слипшийся покажется
	float bottomLvl = minT + (maxT - minT) / 10;

	minT = maxT;
	for (size_t i = 0, total = points.size(); i < total; ++i)
	{
		barvalue<float> &val = points[i];
		if(val.value<bottomLvl)
			continue;
//		if (maxT - val.second >= 2)
//			continue;

		if (minX > val.getX())
			minX = val.getX();

		if (maxX < val.getX())
			maxX = val.getX();

		if (minY > val.getY())
			minY = val.getY();

		if (maxY < val.getY())
			maxY = val.getY();

		if (minT > val.value)
			minT = val.value;
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

		bc::BarcodeCreator<float> creator;

		Barcontainer<float> *bars = creator.searchHoles(img.data, wid, hei);
		Baritem<float> *item = bars->getItem(0);
		img.release();

		int tx = (i % tilesInWid) * tileWid;
		int ty = (i / tilesInWid) * tileHei;

		for (size_t i = 0, total = item->barlines.size(); i < total; ++i)
		{
			bc::barline<float> *line = item->barlines[i];
//			check(line->matr);
			boundy b = getBounty(line->matr);


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
