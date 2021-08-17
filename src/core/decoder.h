#pragma once
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <QDebug>

#include "../base.h"

using std::cout;

#define CHECKRET \
	if (added + 1 >= maxSize) \
		{\
			qDebug() << "Out of tile";\
			return;\
		}

/*
read a character k;
   output k;
   w = k;
   while ( read a character k )    
  // k could be a character or a code.
        {
         if k exists in the dictionary
			entry = dictionary entry for k;
			output entry;
			add w + entry[0] to dictionary;
			w = entry;
         else
			output entry = w + firstCharacterOf(w);
			add entry to dictionary;
			w = entry;
        }

*/
class decorder
{
	static const int MIN_BITS = 9;
	static const int CLEAR_CODE = 256; // clear code
	static const int EOI_CODE = 257; // end of information
	static const int MAX_BYTELENGTH = 12;

	const int st[MAX_BYTELENGTH+1]{
		1,//0
		2,//1
		4,//2
		8,//3
		16,//4
		32,//5
		64,//6
		128,//7
		256,//8
		512,//9
		1024,//10
		2048,//11
		4096//12
	};

	int comprType;
public:
	size_t arrLen;

	decorder(int compressType) : comprType(compressType)
	{
	}

	ushort getByte3(uchar *arry)
	{
		//9 bit
		size_t k = position / 8;
		size_t l = position % 8;
		if (k >= arrLen)
		{
			cout << ("ran off the end of the buffer before finding EOI_CODE (end on input code)");
			return EOI_CODE;
		}
		if (l + byteLength <= 16)// l + len <= 16
		{
//			ushort r2 = ushort(arry[k] << 8 | ((k + 1) < arrLen ? arry[k + 1] : 0));
			ushort r = (ushort(arry[k]) << 8) | ushort(arry[k + 1]);
//			if (r2 != r)
//				qDebug() << "";
			r = r << l;
			r = r >> (16 - byteLength);
			return r;
		}
		else
		{
//			 uint r2 = uint(0 | arry[k] << 16 | ((k + 1)<arrLen? arry[k + 1] << 8 : 0) | ((k + 2) < arrLen? arry[k + 2]: 0));
			uint r = (uint(arry[k]) << 16) | (uint(arry[k + 1]) << 8) | uint(arry[k + 2]);
//			if (r2 != r)
//				qDebug() << "";
			r = r << (l + 8);
			r = r >> (32 - byteLength);
			return ushort(r);
		}
	}

	// void swap(uchar&a, uchar&b)
	// {
	// 	uchar temp =a;
	// 	a =b;
	// 	b = temp;
	// }
	buffer rev;

	uchar getAppedRev(ushort cod2e)
	{

		rev.clear();
		for (int i = cod2e; i != 4096; i = dictionaryIndex[i])
		{
			rev.push_back(dictionaryChar[i]);
		}
		for (int i = (int)rev.size() - 1; i >= 0; --i)
			this->result->push_back(rev[i]);
		return rev[rev.size() - 1];
	}

	void appendReversed(const buffer& source)
	{
		for (int i = (int) source.size() - 1; i >= 0; --i)
		{
			result->push_back(source[i]);
		}
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

	void getNext()
	{
//		if (position == 75953)			qDebug() << "";
//		const ushort byte2 = getByte(arry, position, byteLength);
		code = getByte3(arry);
		position += byteLength;
	}

    void addToDictionary(int i, ushort c)
	{
		assert(dictionaryLength < 4093); //864689
//		if (position == 864689)
//			qDebug() << "";
		dictionaryChar[dictionaryLength] = c;
		dictionaryIndex[dictionaryLength++] = i;
	}

	//ushort because index always less than 4096
	int dictionaryIndex[4093];

	// short because code can be more than 256
	short dictionaryChar[4093];

	ushort dictionaryLength = 258;
	int byteLength = MIN_BITS;
	size_t position = 0, added = 0, maxSize= 0;

        sbuffer* result;
	// uchar *result;
	uchar *arry;
	short code;
public:

	void decompress(uchar* input, offu64 size, sbuffer& result, size_t maxVal = UINT64_MAX)
	{
		if (comprType == 1)
		{
			result.insert(result.begin(), input, input + size);
			//			output = input;
			return;
		}

		this->result = &result;
		this->arry = input;

		this->maxSize = maxVal;
		this->arrLen = size;
		this->position = 0;
		this->added = 0;

		memset(&dictionaryIndex, 0, 4093 * 4);
		memset(&dictionaryChar, 0, 4093  * 2);
		for (ushort i = 0; i <= 257; i++)
		{
			dictionaryIndex[i] = 4096;
			dictionaryChar[i] = i;
		}
		initDictionary();
		getNext();
		short oldCode = 0;

		buffer val;

		while (code != EOI_CODE)
		{
			if (code == CLEAR_CODE)
			{
				initDictionary();
				getNext();
				while (code == CLEAR_CODE)
				{
					getNext();
				}

				if (code == EOI_CODE)
				{
					val.clear();
					break;
				}
				else if (code > CLEAR_CODE)
				{
					val.clear();
					throw std::exception();//"corrupted code at scanline ${ code }");
				}
				else
				{
					// getAppedRev(code);
					// CHECKRET

					getDictionaryReversed(code, val);
					appendReversed(val);
					added += val.size();
					if (result.size() - added >= maxVal)
						return;

					val.clear();
					oldCode = code;
				}
			}
			else if (code < dictionaryLength)
			{
				getDictionaryReversed(code, val);
				appendReversed(val);
				added += val.size();
				if (result.size() - added >= maxVal)
					return;

				addToDictionary(oldCode, val[val.size() - 1]);

				// ushort last = getAppedRev(code);
				// addToDictionary(oldCode, last);
				// CHECKRET
				val.clear();
				oldCode = code;
			}
			else
			{
				getDictionaryReversed(oldCode, val);

				if (val.empty())
				{
					throw std::exception();//"Bogus entry.Not in dictionary, ${ oldCode } / ${ dictionaryLength }, position: ${ position }");
				}
				appendReversed(val);
				result.push_back(val[val.size() - 1]);
				if (result.size() - ++added >= maxVal)
					return;

				addToDictionary(oldCode, val[val.size() - 1]);

				// ushort last = getAppedRev(code);

				// this->result->push_back(last);
				// CHECKRET

				// // result[added++] = last;//oldVal[oldVal.size() - 1];
				// addToDictionary(oldCode, last);
				val.clear();
				oldCode = code;
			}

			if (dictionaryLength + 1 >= st[byteLength])
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
			getNext();
		}
	}
};
