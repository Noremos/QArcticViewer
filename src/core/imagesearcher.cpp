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
void ImageSearcher::findZones(std::vector<boundy> &bounds, int start, int len)
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

Img *ImageSearcher::getRect(boundy &bb)
{
	int startX = bb.x / tileWid;
	int endX = bb.endX / tileWid;
	int startY = bb.y / tileHei;
	int endY = bb.endY / tileHei;

	if (startX != endX || startY != endY)
		return nullptr;


	int index = getTid(startX, startY, tilesInWid);

	Img *null = nullptr;
	Img *g = this->cachedTiles.getData(index, null);
	if (g == null)
	{
		Img tg = getTile(startX, startY);
		g = new Img(tg.data, tg.wid, tg.hei);
		tg.data = new float[1];
		cachedTiles.storeData(index, g);
	}

	Img *ret = new Img(bb.wid(), bb.hei());

	//bb.x==1054 || startX * tileWid==1000 || startInTile = 54
	int startInTileX = bb.x - startX * tileWid;
	int startInTileY = bb.y - startY * tileHei;
	for (uint j = 0; j < bb.hei(); ++j)
	{
		for (uint i = 0; i < bb.wid(); ++i)
		{
			float d = g->getSafe(startInTileX + i, startInTileY + j);
			ret->set(i, j, d);
		}
	}
	return ret;
}

bool ImageSearcher::checkCircle(boundy &bb)
{
	int startX = bb.x / tileWid;
	int endX = bb.endX / tileWid;
	int startY = bb.y / tileHei;
	int endY = bb.endY / tileHei;

	if (startX != endX || startY != endY)
		return false;


	int index = getTid(startX, startY, tilesInWid);

	Img *null = nullptr;
	Img *g = this->cachedTiles.getData(index, null);
	if (g == null)
	{
		Img tg = getTile(startX, startY);
		g = new Img(tg.data, tg.wid, tg.hei);
		tg.data = new float[1];
		cachedTiles.storeData(index, g);
	}

	Img ret(bb.wid(), bb.hei());

	float maxval = -9999;
	int xc = 0, yc = 0;
	for (uint i = 0; i < bb.wid(); ++i)
	{
		for (uint j = 0; j < bb.hei(); ++j)
		{
			float d = g->getSafe(bb.x + i, bb.y + j);
			ret.set(i, j, d);
			if (d > maxval)
			{
				xc = i;
				yc = j;
				maxval = d;
			}
		}
	}
	float sum = 0;
	int radius = 20;
	int total = 0;
	for (int var = 0; var < 40; ++var)
	{
		int xst = xc - radius + var;
		int yreal = sqrt(radius * radius - (xst - xc) * (xst - xc));

		xst = MAX(0, MIN(xst, ret.wid));
		yreal = MAX(0, MIN(yreal, ret.hei));

		float val1 = ret.getSafe(xst, yreal), val2;
		if (yreal > yc)
			val2 = ret.getSafe(xst, yreal - (yreal - yc) * 2);
		else //(yreal < yc)
			val2 = ret.getSafe(xst, yreal + (yc - yreal) * 2);

		sum += val1 + val2;
		total += 2;
	}

	sum /= total;

	float t1 = abs(sum - ret.getSafe(xc - radius, yc));
	float t2 = abs(sum - ret.getSafe(xc, yc - radius));
	float t3 = abs(sum - ret.getSafe(xc - radius, yc - radius));
	float t4 = abs(sum - ret.getSafe(xc + radius, yc + radius));

	float eps = 5;
	return (t1 <= eps && t2 <= eps && t3 <= eps && t4 <= eps);
}
