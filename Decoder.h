#pragma once
#include <vector>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <string.h>

using namespace std;

typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned short ushort;

typedef vector<uchar> buffer;

//typedef unsigned long long offu64;
typedef unsigned int offu64;


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
	int getByte(uchar* arry, int position, int length)
	{
		const int d = position % 8;
		const int a = floor(position / 8);
		const int de = 8 - d;
		const int ef = (position + length) - ((a + 1) * 8);
		int fg = (8 * (a + 2)) - (position + length);
		const int dg = ((a + 2) * 8) - position;
		fg = max(0, fg);
		if (a >= arrLen)
		{
			cout << ("ran off the end of the buffer before finding EOI_CODE (end on input code)");
			return EOI_CODE;
		}

		int chunk1 = arry[a] & (st(8 - d) - 1);
		chunk1 = chunk1 << (length - de);
		int chunks = chunk1;
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
		for (int i = source.size() - 1; i >= 0; i--)
			dest.push_back(source[i]);
	}
	void getDictionaryReversed(int n, buffer& rev)
	{
		for (int i = n; i != 4096; i = dictionaryIndex[i])
		{
			rev.push_back(dictionaryChar[i]);
		}
	}

	void initDictionary()
	{
		dictionaryLength = 258;
		byteLength = MIN_BITS;
	}
	ushort getNext(uchar* arry)
	{
		const ushort byte2 = getByte(arry, position, byteLength);
		position += byteLength;
		return byte2;
	}
	int addToDictionary(ushort i, uchar c)
	{
		dictionaryChar[dictionaryLength] = c;
		dictionaryIndex[dictionaryLength] = i;
		dictionaryLength++;
		return dictionaryLength - 1;
	}

	ushort dictionaryIndex[4093];
	uchar dictionaryChar[4093];
	int dictionaryLength = 258;
	int byteLength = MIN_BITS;
	uint position = 0;

	void decompress(uchar* input, offu64 size, buffer& result, uint maxVal = UINT32_MAX)
	{
		memset(&dictionaryIndex, 0, 4093 * 2);
		memset(&dictionaryChar, 0, 4093);
		arrLen = size;
		for (int i = 0; i <= 257; i++)
		{
			dictionaryIndex[i] = 4096;
			dictionaryChar[i] = i;
		}
		uint added = result.size();
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
				if (result.size() - added >= maxVal)
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
