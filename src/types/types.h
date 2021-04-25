#ifndef TYPES_H
#define TYPES_H
#include "../base.h"

#include <QString>

struct boundy
{
public:
	float x, y, endX, endY;
	float z, endZ;
	inline boundy(uint x, uint y, uint endX, uint endY): x(x), y(y), endX(endX), endY(endY)
	{
	}

	inline boundy(uint x, uint y, float z, uint endX, uint endY, float endZ): x(x), y(y), endX(endX), endY(endY), z(z), endZ(endZ)
	{
		if (endZ < z)
		{
			std::swap(endZ, z);
		}
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

	inline float wid() const
	{
		return endX - x;
	}

	inline float hei() const
	{
		return endY - y;
	}
	inline float zei() const
	{
		return endZ > z? endZ - z : z - endZ;
		// return/* endZ > z? */endZ - z /*: z - endZ*/;
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
		// we sacle all the verts
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


template<class T>
struct TRange
{
	T start{};
	T end{};
	TRange(T start, T end) : start(start), end(end) {}
	TRange()
	{}
};

#endif // !TYPES_H
