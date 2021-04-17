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


			uchar *tile = reader->getTile(ty * tilesInWid + tx);

			float *data = static_cast<float *>(reader->processData(tile));

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
	float bottomLvl = minT + (maxT - minT) / bottomLevel;

	for (size_t i = 0, total = points.size(); i < total; ++i)
	{
		barvalue<float> &val = points[i];
		if(val.value < bottomLvl)
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
		barsOut.writeLine("{num:" + tileNumstr + ", barcodes:");

		Img img = getTile(i);
		int wid = img.wid, hei = img.hei;

		bc::BarcodeCreator<float> creator;

		Barcontainer<float> *bars = creator.searchHoles(img.data, wid, hei);
		Baritem<float> *item = bars->getItem(0);

		barsOut.writeLine("{");
		barsOut.write("bars:[ ");


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


			line->getJsonObject(out);
			out += ",";
		}
		out[out.length() - 1] = ']';

		QString swr = QString::fromStdString(out);
		swr += nl;
		if (i != totalTiles - 1)
			swr += "},";
		else
			swr += "}";

		barsOut.writeLine(swr);

		totalAdded += item->barlines.size();

		delete bars;

		if (valStop)
			break;
	}
	qDebug() << "st:" << start << "ed:" << len << "size:" << totalAdded;
	return totalAdded;
}

Img ImageSearcher::getRect(boundy &bb, int &maxX, int &maxY)
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
		cachedTiles.storeData(index, g);
	}

	Img ret(bb.wid(), bb.hei());

	//bb.x==1054 || startX * tileWid==1000 || startInTile = 54
	int startInTileX = bb.x - startX * tileWid;
	int startInTileY = bb.y - startY * tileHei;


	float maxval = -9999;

	for (uint j = 0; j < bb.hei(); ++j)
	{
		for (uint i = 0; i < bb.wid(); ++i)
		{
			float d = g->getSafe(startInTileX + i, startInTileY + j);
			ret.set(i, j, d);

			if (d > maxval)
			{
				maxX = i;
				maxY = j;
				maxval = d;
			}
		}
	}
	ret.maxVal = maxval;

	return ret;
}

bool ImageSearcher::checkCircle(boundy &bb, float eps)
{
	int xc=0, yc=0;
	Img ret = getRect(bb, xc,yc);

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

	return (t1 <= eps && t2 <= eps && t3 <= eps && t4 <= eps);
}

void exportDataAsBeaf(const QString &path, int wid, int hei, float *data)
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

void exportDataAsPng(const QString &path, bc::BarImg<float> &bimg)
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

union conv
{
	char data[4];
	float val;
	int ival;
};


Img importBeafData(const QString &path)
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
