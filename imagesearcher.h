#ifndef IMAGESEARCHER_H
#define IMAGESEARCHER_H
#include "base.h"
#include "tiffreader.h"
#include "barcodeCreator.h"
using namespace bc;


struct boundy
{
public:
	uint x, y, endX, endY;
	float z, endZ;
	inline boundy(uint x, uint y, uint endX, uint endY): x(x), y(y), endX(endX), endY(endY)
	{
	}

	inline boundy(uint x, uint y, float z, uint endX, uint endY, float endZ): x(x), y(y), endX(endX), endY(endY), z(z), endZ(endZ)
	{
	}

	inline void setXifLess(uint ix)
	{
		if (ix < this->x)
			this->x = ix;
	}

	inline void setYifLess(uint iy)
	{
		if (iy < this->y)
			this->y = iy;
	}

	inline void setEndXifMore(uint ix)
	{
		if (ix >endX)
			endX = ix;
	}

	inline void setEndYifMore(uint iy)
	{
		if (iy > endY)
			endY = iy;
	}

	inline uint wid() const
	{
		return endX - x;
	}

	inline uint hei() const
	{
		return endY - y;
	}
	inline float zei() const
	{
		return endZ - z;
	}
	void addXoffset(int off)
	{
		x += off;
		endX += off;
	}
	inline void addYoffset(int off)
	{
		y += off;
		endY += off;
	}
	inline void divStep(float step)
	{
		x /= step;
		y /= step;
		z /= step;
		endX /= step;
		endY /= step;
		endZ /= step;
	}
	inline QString getStr()
	{
		return QString::number(x) + " "+  QString::number(y) + " " + QString::number(z) + " " +
			QString::number(endX) + " "+  QString::number(endY) + " " + QString::number(endZ);
	}
};
enum class ProcessType
{
	Otsu = 0,
	NaburPorog,
	Manuly,
	HistFromMI,
	Waterfall,
	Canny,
	Smoof,
	AvgSdvig
};
enum class AvgType
{
	AvgNsum = 512,
	AvgNsumFromMI = 1024,
	Hist = 2048,
	Manuly = 4096,
	MaxMinusMin = 8192
};
struct Img
{
	int minVal = -9999;
	int maxVal = -9999;
	float *data;
	int wid;
	int hei;
	Img(float *data = nullptr, int widgth = 0, int height = 0) : data(data), wid(widgth), hei(height) {}
	Img(int widgth, int height) : wid(widgth), hei(height) { data = new float[wid * hei]; }

	float get(int x, int y) const { return data[y * wid + x]; }
	void set(int x, int y, float val) { data[y * wid + x] = val; }
	void relese() { delete[] data; }

	float getSafe(int x, int y) const
	{
		x = MAX(0, MIN(x, wid-1));
		y = MAX(0, MIN(y, hei-1));
		return data[y * wid + x];
	}

	inline Img clone() const
	{
		Img clo(new float[wid * hei], wid, hei);
		memcpy(clo.data, data, wid * hei * sizeof(float));
		return clo;
	}
	inline uint getTotal()
	{
		return wid*hei;
	}

	void zeroing() { memset(data, 0, wid * hei * sizeof(float)); }
	void setInRow(int y, int offsetInDest, float* inputData, int len)
	{
		memcpy(data + wid*y +offsetInDest, inputData, len* sizeof(float));
	}
	inline Img zeroClone() const
	{
		Img clo(new float[wid * hei], wid, hei);
		clo.zeroing();
		return clo;
	}
	inline void release()
	{
		delete[] data;
	}
};

template<class T>
struct TRange
{
	T start{};
	T end{};
	TRange(T start, T end) : start(start), end(end) {}
	TRange()
	{}
};

struct lowline
{
    lowline()
    {
        start = 0;
        len = 0;
        bar3d = nullptr;
    }
    bool isEmpty()
    {
        return bar3d==nullptr;
    }
    lowline(barline<float>* org)
    {
        this->start = org->start;
        this->len = org->len;
        this->bar3d = org->bar3d;
        org->bar3d = nullptr;
    }
    float start;
    float len;
    bc::barcounter<float>* bar3d;
    float end()
    {
        return start+len;
    }
    void getStr(QString & str)
    {
        if (isEmpty())
            return;

       str+= QString::number(start) + " " +  QString::number(len);
       qDebug() << bar3d->size();
       for(auto& l:(*bar3d))
       {
           str+= " " + QString::number(l.value)+ " " + QString::number(l.count);
       }
    }


};
typedef vector<lowline*> veclines;

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
		coof = 0;
		diamert = {0, 0};
		height = {0, 0};
		bottomProc = 0;
	}
// FOR PORPERTYs

	int diametrMin() { return diamert.start; }
	void setDiametrMin(int val) { diamert.start = val;}

	int diametrMax() { return diamert.end; }
	void setDiametrMax(int val) { diamert.end = val;}

	float heightMin() { return height.start; }
	void setHeightMin(float val)  { height.start = val;}

	float heightMax() { return height.end; }
	void setHeightMax(float val)  { height.end = val;}
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

	PointerCache<Img *> cachedTiles;
	Img getTile(int index);

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
	void findZones(vector<boundy> &bounds, int start, int len);

	bool checkCircle(boundy &bb);

	Img getTile(int tx, int ty);
};

#endif // IMAGESEARCHER_H
