#ifndef IMG_H
#define IMG_H
#include "src/base.h"
#include  <cstring>

#include "types.h"

struct Img
{
	int minVal = -9999;
	int maxVal = -9999;
	float *data;
	int wid;
	int hei;
	Img(float *data = nullptr, int widgth = 0, int height = 0) : data(data), wid(widgth), hei(height) {}
	Img(int widgth, int height) : wid(widgth), hei(height) { data = new float[wid * hei]; }
	~Img()
	{}
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
	
	void set(int x, int y, float val) { data[y * wid + x] = val; }
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
};
#endif // !IMG_H
