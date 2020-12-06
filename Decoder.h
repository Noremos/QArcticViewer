#pragma once
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <QDebug>

#include "base.h"
using namespace std;


inline long long st(long long st)
{
	int step = 1;
	for (long long i = 1; i <= st; i++)
		step *= 2;
	return step;
}

class decorder
{
public:
	const int MIN_BITS = 9;
	const int CLEAR_CODE = 256; // clear code
	const int EOI_CODE = 257; // end of information
	const int MAX_BYTELENGTH = 12;
	int arrLen;

/*	int getByte2(uchar *arry, int pos, int len = 8)
	{
		//9 bit
		int k = pos / 8;
		int l = pos % 8;
		int r = 0;
		int lnm = len / 8;
		int mask = 0;
		for (uchar cs = 0; cs <= lnm; ++cs)
		{
			r |= arry[k + cs] << (lnm - cs) * 8;
			mask |= 255 << (lnm - cs) * 8;
		}
		lnm += 1;
//		int r = int(arry[k] << 8 | arry[k + 1]);
		char ds = lnm * 8 - len - l;
		if (ds < 0)
		{
			cout << ("ran off the end of the buffer before finding EOI_CODE (end on input code)");
			return EOI_CODE;
		}
		r = (r << l) & mask;
		r = r >> (ds + l);
		return r;
	}*/
	ushort getByte3(uchar *arry, uint pos, int len = 9)
	{
		//9 bit
		uint k = pos / 8;
		uint l = pos % 8;
		if (l + len <= 16)// l + len <= 16
		{
			ushort r = ushort(arry[k] << 8 | arry[k + 1]);
			r = r << l;
			r = r >> (16 - len);
			return r;
		}
		else
		{
			uint r = uint(0 | arry[k] << 16 | (k + 1<arrLen? arry[k + 1] << 8 : 0) | (k + 2<arrLen? arry[k + 2]: 0));
			r = r << (l + 8);
			r = r >> (32 - len);
			return ushort(r);
		}
	}
	ushort getByte(uchar* arry,const int position, const int length)
	{
		const int d = position % 8;// bit start pos
		const int a = position / 8;// byte start pos
		const int de = 8 - d;// bits in byte that needs
		const int ef = (position + length) - ((a + 1) * 8);// bits to reed?
		int fg = (8 * (a + 2)) - (position + length);// start offset from second byte
		const int dg = ((a + 2) * 8) - position;//need to reeds?
		fg = max(0, fg);
		if (a >= arrLen)
		{
			cout << ("ran off the end of the buffer before finding EOI_CODE (end on input code)");
			return EOI_CODE;
		}

		int chunk1 = arry[a] & (st(8 - d) - 1);
		chunk1 = chunk1 << (length - de);
		int chunks = chunk1;//get bits from first byte
		if (a + 1 < arrLen)
		{
			int chunk2 = arry[a + 1] >> fg;
			chunk2 <<= max(0, (length - dg));
			chunks += chunk2;
		}
		if (ef > 8 && a + 2 < arrLen)
		{
			const int hi = ((a + 3) * 8) - (position + length);
			const int chunk3 = arry[a + 2] >> hi;
			chunks += chunk3;
		}
		return chunks;
	}

	void appendReversed(buffer& dest, const buffer& source)
	{
		for (int i = (int)source.size() - 1; i >= 0; i--)
			dest.push_back(source[i]);
	}
	void getDictionaryReversed(int n, buffer& rev)
	{
		for (int i = n; i != 4096; i = dictionaryIndex[i])
		{
			rev.push_back(dictionaryChar[i]);
		}
	}

	inline void initDictionary()
	{
		dictionaryLength = 258;
		byteLength = MIN_BITS;
	}
	ushort getNext(uchar* arry)
	{
		//2295-b2 2335-b3
//		const int byte2 = getByte(arry, position, byteLength);
		const ushort byte2 = getByte3(arry, position, byteLength);

		position += byteLength;
		return byte2;
	}
	void addToDictionary(int i, uchar c)
	{
		assert(dictionaryLength < 4093); //864689
//		if (position == 864689)
//			qDebug() << "";
		dictionaryChar[dictionaryLength] = c;
		dictionaryIndex[dictionaryLength++] = i;
	}

	int dictionaryIndex[4093];
	short dictionaryChar[4093];
	int dictionaryLength = 258;
	int byteLength = MIN_BITS;
	uint position = 0;

public:
	void decompress(uchar* input, offu64 size, buffer& result, uint maxVal = UINT32_MAX)
	{
		memset(&dictionaryIndex, 0, 4093 * 4);
		memset(&dictionaryChar, 0, 4093  * 2);
		arrLen = size;
		for (int i = 0; i <= 257; i++)
		{
			dictionaryIndex[i] = 4096;
			dictionaryChar[i] = i;
		}
		uint added = (uint)result.size();
		initDictionary();
		auto arry = input;
		ushort code = getNext(arry);
		ushort oldCode = 0;
		while (code != EOI_CODE)
		{
			if (code == CLEAR_CODE)
			{
				initDictionary();
				code = getNext(arry);
				while (code == CLEAR_CODE)
				{
					code = getNext(arry);
				}

				if (code == EOI_CODE)
				{
					break;
				}
				else if (code > CLEAR_CODE)
				{
					throw std::exception();//"corrupted code at scanline ${ code }");
				}
				else
				{
					buffer val;
					getDictionaryReversed(code, val);
					appendReversed(result, val);
					added += val.size();
					if (result.size() - added >= maxVal)
						return;

					oldCode = code;
				}
			}
			else if (code < dictionaryLength)
			{
				buffer val;
				getDictionaryReversed(code, val);
				appendReversed(result, val);
				added += val.size();
				if (result.size() - added >= maxVal)
					return;

				addToDictionary(oldCode, val[val.size() - 1]);
				oldCode = code;
			}
			else
			{
				buffer oldVal;
				getDictionaryReversed(oldCode, oldVal);

				if (oldVal.empty())
				{
					throw std::exception();//"Bogus entry.Not in dictionary, ${ oldCode } / ${ dictionaryLength }, position: ${ position }");
				}
				appendReversed(result, oldVal);
				result.push_back(oldVal[oldVal.size() - 1]);
				if (result.size() - ++added >= maxVal)
					return;

				addToDictionary(oldCode, oldVal[oldVal.size() - 1]);
				oldCode = code;
			}

			if (dictionaryLength + 1 >= st(byteLength))
			{
				if (byteLength == MAX_BYTELENGTH)
				{
					oldCode = -1;
				}
				else
				{
					byteLength++;
				}
			}
			code = getNext(arry);
		}
	}
};
