#ifndef IMAGESEARCHER_H
#define IMAGESEARCHER_H
#include "../base.h"
#include "tiffreader.h"

#include <QFile>
#include <QObject>
#include <vector>

#include <src/types/types.h>
#include <src/types/img.h>
#include "side-src/Barcode/PrjBarlib/include/barcodeCreator.h"
using namespace bc;

#include "fstream"

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

struct FileBuffer
{
	QString buffer;
	QFile outfile;
	QTextStream *stream = nullptr;
	int maxBufferSize = 10000;

	bool openFileStream(QString path, int maxBufferSize = 10000)
	{
		outfile.setFileName(path);

		//		if (outfile.exists())outfile.remove();
		if (!outfile.open(QFile::WriteOnly | QFile::Truncate))
			return false;

		if (stream != nullptr)
			delete stream;
		stream = new QTextStream(&outfile);

		this->maxBufferSize = maxBufferSize;

		return true;
	}

	void write(const QString &part)
	{
		buffer += part;
		if (buffer.size() > maxBufferSize)
		{
			stream->operator<<(buffer);
			buffer.clear();
		}
	}


	void writeLine(const QString &part= "")
	{
		buffer += part + nl;
		if (buffer.size() > maxBufferSize)
		{
			stream->operator<<(buffer);
			buffer.clear();
		}
	}

	void close()
	{
		stream->operator<<(buffer);
		buffer.clear();

		outfile.close();
	}
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

	// Процент отсечения снизу
	int bottomLevel = 0;


	PointerCache<Img *> cachedTiles;
	boundy getBounty(barline<float> *line);
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
	size_t findROIs(FileBuffer &bounds, FileBuffer &bars, int start, int len, int bottom, volatile bool &valStop);

	void setFillTileRowCache()
	{
		reader->setRowsCacheSize(tileHei + diffset + 10);
	}

	bool checkCircle(Img &ret, float eps  = 5);

	Img getTile(int index);
    Img getTile(int tx, int ty);

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
};

#endif // IMAGESEARCHER_H
