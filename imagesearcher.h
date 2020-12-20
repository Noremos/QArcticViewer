#ifndef IMAGESEARCHER_H
#define IMAGESEARCHER_H
#include "base.h"
#include "tiffreader.h"

#ifdef USE_OPENCV
namespace cv
{
class Mat;
}
typedef cv::Mat RetImg;
#else
typedef Img RetImg;
#endif


struct boundy
{
	uint x, y, endX, endY;

	boundy(uint x, uint y, uint endX, uint endY): x(x), y(y), endX(endX), endY(endY)
	{
	}

	void setXifLess(uint ix)
	{
		if (ix < this->x)
			this->x = ix;
	}

	void setYifLess(uint iy)
	{
		if (iy < this->y)
			this->y = iy;
	}

	void setEndXifMore(uint ix)
	{
		if (ix >endX)
			endX = ix;
	}

	void setEndYifMore(uint iy)
	{
		if (iy > endY)
			endY = iy;
	}

	uint wid()
	{
		return endX - x;
	}

	uint hei()
	{
		return endY - y;
	}
	void addXoffset(int off)
	{
		x += off;
		endX += off;
	}
	void addYoffset(int off)
	{
		y += off;
		endY += off;
	}
	float localMax;
	void setMax(float val) { localMax = val; }
	float getMax() { return localMax; }
	void divStep(float step)
	{
		x /= step;
		y /= step;
		endX /= step;
		endY /= step;
		localMax /= step;
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
	Img clone() const
	{
		Img clo(new float[wid * hei], wid, hei);
		memcpy(clo.data, data, wid * hei * sizeof(float));
		return clo;
	}
	uint getTotal()
	{
		return wid*hei;
	}

	void zeroing() { memset(data, 0, wid * hei * sizeof(float)); }
	void setInRow(int y, int offsetInDest, float* inputData, int len)
	{
		memcpy(data + wid*y +offsetInDest, inputData, len* sizeof(float));
	}
	Img zeroClone() const
	{
		Img clo(new float[wid * hei], wid, hei);
		clo.zeroing();
		return clo;
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

	void binFindObjects(const Img &img, vector<boundy> &objects);

public:
	ImageSearcher(TiffReader *reader);

//	Img getTile(int tx, int ty);
	Img getTile(int index);
	int getMaxTiles();

	void findZones(vector<boundy> &bounds, int start);
	RetImg process(Img tile, AvgType avgType, ProcessType funType, uchar porog);
	float findHistAVG(Img tile);
	float findSumdivAVG(Img &tile);
};

#endif // IMAGESEARCHER_H
