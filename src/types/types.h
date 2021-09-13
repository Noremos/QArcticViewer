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
		z = 0;
		endZ = 0;
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

	float bottom()
	{
		return endZ < z ? endZ: z;
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

// enum class glColor {
// 	Green = 0, // vec3(0.0, 1.0, 0.0);
// 	Purpure = 2, // vec3(0.54, 0.0, 0.5);
// 	Blue = 4, // vec3(0.0, 0.3, 0.7);
// 	Red = 8, //vec3(1.0, 0.0, 0.0);
// 	Brown = 16, //vec3(0.58, 0.29, 0.0);
// 	Oragne = 32 //vec3(1.0, 0.5, 0.0);
// };

enum class glColor {
	Green = 1, // vec3(0.0, 1.0, 0.0);
	Purpure = 2, // vec3(0.54, 0.0, 0.5);
	Blue = 3, // vec3(0.0, 0.3, 0.7);
	Red = 4, //vec3(1.0, 0.0, 0.0);
	Brown = 5, //vec3(0.58, 0.29, 0.0);
	Oragne = 6, //vec3(1.0, 0.5, 0.0);
	null = 7
};

#endif // !TYPES_H
