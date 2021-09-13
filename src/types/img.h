#ifndef IMG_H
#define IMG_H
#include "src/base.h"
#include  <cstring>

#include "types.h"

struct Img
{
	float minVal = -9999;
	float maxVal = -9999;
	float *data;
	int wid;
	int hei;
	Img(float *data = nullptr, int widgth = 0, int height = 0) : data(data), wid(widgth), hei(height) {}
	Img(int widgth, int height) : wid(widgth), hei(height) { data = new float[wid * hei]; }
	~Img()
	{}

//	Img(const Img &other) : wid(other.wid), hei(other.hei), maxVal(other.maxVal), minVal(other.minVal) /*: s(other.s)*/
//	{
//		assert(false);
//		data = new float[wid * hei];
//		memcmp(data, other.data, wid * hei * sizeof(float));
//	}
//	/*std::cout << "move failed!\n";*/
//	Img(Img &&other) : minVal(other.minVal),  maxVal(other.maxVal) /*: s(std::move(o.s))*/
//	{
//		data = std::exchange(other.data, nullptr); // leave other in valid state
//		wid = std::exchange(other.wid, 0); // leave other in valid state
//		hei = std::exchange(other.hei, 0); // leave other in valid state
//	}

//	// copy assignment
//	Img &operator=(const Img &other)
//	{
//		// Guard self assignment
//		if (this == &other)
//			return *this;

//		wid = other.wid; // leave other in valid state
//		hei = other.hei; // leave other in valid state

//		maxVal = other.maxVal;
//		minVal = other.minVal;
//		memcmp(data, other.data, wid * hei * sizeof(float));
//		return *this;
//	}

//	// move assignment
//	Img &operator=(Img &&other) noexcept
//	{
//		// Guard self assignment
//		if (this == &other)
//			return *this; // delete[]/size=0 would also be ok

//		wid = std::exchange(other.wid, 0); // leave other in valid state
//		hei = std::exchange(other.hei, 0); // leave other in valid state
//		data = std::exchange(other.data, nullptr); // leave other in valid state

//		maxVal = other.maxVal;
//		minVal = other.minVal;

//		return *this;
//	}


	float get(int off) const { return data[off]; }
	float get(int x, int y) const { return data[y * wid + x]; }

	uint minmaxX(int x)
	{
		return MAX(0, MIN(x, wid-1));
	}

	uint minmaxY(int x)
	{
		return MAX(0, MIN(x, hei-1));
	}
	float getSafe(int x, int y) const
	{
		x = MAX(0, MIN(x, wid-1));
		y = MAX(0, MIN(y, hei-1));
		return data[y * wid + x];
	}

	float getSafe(int x, int y, float defValue) const
	{
		if (x < 0 || x >= wid || y < 0 || y >= hei)
			return defValue;
		return data[y * wid + x];
	}

	void set(int x, int y, float val) { data[y * wid + x] = val; }

	inline Img clone() const
	{
		Img clo(new float[wid * hei], wid, hei);
		memcpy(clo.data, data, wid * hei * sizeof(float));
		clo.maxVal = maxVal;
		clo.minVal = minVal;
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
		if (data)
		{
			delete[] data;
			data = nullptr;
		}
	}

    void getRect(int x, int y, int wid, int hei, Img& ret)
	{
        ret.wid = wid;
        ret.hei = hei;
		ret.release();

		ret.data = new float[ret.wid * ret.hei];
        memset(ret.data, 0 ,ret.wid * ret.hei* sizeof(float));

        for (int j = 0; j < ret.hei; ++j)
		{
			uint cs = minmaxX(x);
			uint ce = minmaxX(x + wid);

			//j = row/y in dest;
			//j * ret.wid = float pointer start at row 'j' = ret[:,j]

			//y + j  = row/y in source;
			//(y + j) * this->wid + cs
			uchar *dsa = reinterpret_cast<uchar *>(ret.data + j * ret.wid);// ret[:,j]
			uchar *src = reinterpret_cast<uchar *>(this->data + (y + j) * this->wid + cs);// ret[cs:,(y+j)]
			memcpy(dsa, src, (ce - cs) * 4);
		}
//		ret.maxVal = maxval;
	}

	void genMinMax(float nullVal = -9999)
	{
		maxVal = get(0);
		minVal = maxVal;
		for (uint i = 0; i < getTotal(); ++i)
		{
			float v = get(i);
			if (v==nullVal)
				continue;

			if (v > maxVal)
				maxVal = v;
			if (v < minVal)
				minVal = v;
		}
	}
};
#endif // !IMG_H
