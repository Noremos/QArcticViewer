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
struct stopPair
{
	size_t posInSource;
	size_t posInRes;
};


typedef std::vector<uchar> revbuffer;

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
		size_t k = positionInInput / 8;
		size_t l = positionInInput % 8;
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
	revbuffer rev;

	uchar getAppedRev(ushort cod2e)
	{
		rev.clear();
		for (int i = cod2e; i != 4096; i = dictionaryIndex[i])
		{
			rev.push_back(dictionaryChar[i]);
		}
		for (int i = (int)rev.size() - 1; i >= 0; --i)
			this->result[++resultSize] = (rev[i]);
		return rev[rev.size() - 1];
	}

	void appendReversed(const revbuffer& source)
	{
		for (int i = (int) source.size() - 1; i >= 0; --i)
		{
			result[++resultSize] = (source[i]);
		}
	}
	void getDictionaryReversed(int n, revbuffer& rev)
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
		positionInInput += byteLength;
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
	size_t positionInInput = 0, maxSize= 0;

	uchar *result;
	size_t resultSize = 0;
	// uchar *result;
	uchar *arry;
	short code;

	std::vector<stopPair> stops;
public:
	void loadStops(stopPair &_stops, int n)
	{
		stops.insert(stops.begin(), n, _stops);
	}

	void decompress(uchar* input, offu64 size, vbuffer& _result, int bytesPerRow, size_t offset = 0, size_t maxSize = UINT64_MAX)
	{
		_result.allocate(bytesPerRow);
		if (comprType == 1)
		{
			memcpy(_result.data(), input + offset, (maxSize == UINT64_MAX? size: MIN(maxSize, size)));
//            result.insert(result.begin(), input, input + size);
//			output = input;
			return;
		}

		if (stops.size() > 0)
		{
			size_t i = 0;
			for (i = 0; i < stops.size(); ++i)
			{
				if (stops[i].posInRes > offset)
				{
					break;
				}
			}
			//			resultSize = stops[i - 1].posInRes;
			positionInInput = stops[i - 1].posInSource;
		}
		else
		{
			this->positionInInput = 0;
		}



		this->resultSize = -1;
		// Чтобы обратиться к текущему индексу, пришлось бы делать resultSize++, т.е.
		// resultSize==0; result[resultSize++] = 1; resultSize==1; <=> resultSize==0; result[0] = 1; resultSize==1;
		// Но resultSize++ -- плохо, поэтому используем хак. Делаем result указателем на -1 элемнет и зменяем resultSize++ на ++resultSize
		// Таким образом получается, что: resultSize==0; result[++resultSize] =1 ; resultSize==1 <=> resultSize==0; result[1] = 1; resultSize==1;
		// result[1] <=> _result[0]
		this->result = _result.data();
		this->arry = input;

		this->maxSize = maxSize ;
		this->arrLen = size;

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

		revbuffer val;

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
				stops.push_back({resultSize, positionInInput});

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
					if (resultSize >= maxSize )
						return;

					val.clear();
					oldCode = code;
				}
			}
			else if (code < dictionaryLength)
			{
				getDictionaryReversed(code, val);
				appendReversed(val);
				if (resultSize >= maxSize )
					return;

				addToDictionary(oldCode, val[val.size() - 1]);

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
				result[++resultSize] = (val[val.size() - 1]);
				if (resultSize >= maxSize )
					return;

				addToDictionary(oldCode, val[val.size() - 1]);

				// CHECKRET

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

	//make sure result have enough size
	bool decompressLZW(uchar* input, offu64 size, vbuffer& result, int bytesPerRow)
	/*
		Works for RGB but not for RRGGBB (planarConfiguration = 2).  Requires tweak to pBuf.
	*/
	{
		const unsigned int CLEAR_CODE = 256;
		const unsigned int EOF_CODE = 257;
//		const unsigned int MAXCODE = 4095;      // 12 bit max less some head room

		#define LZW_STRING_SIZE 256
		#define LZW_STRINGS_SIZE 128000

		bool ret = false;
		// input and output pointers
		char* c =  (char*)(input);
		char* out = (char*)(result.data());

		char* s[4096];                                  // ptrs in strings for each possible code
		int8_t sLen[4096];                              // code string length
		memset(&sLen, 1, 256);                     // 0-255 one char strings

		char strings[LZW_STRINGS_SIZE];
		// initialize first 256 code strings
		for (int i = 0 ; i != 256 ; i++ ) {
			strings[i] = (char)i;
			s[i] = &strings[i];
		}
		strings[256] = 0;  s[256] = &strings[256];      // Clear code
		strings[257] = 0;  s[257] = &strings[257];      // EOF code
		const uint32_t maxCode = 4095;                  // max for 12 bits
		char* sEnd;                                     // ptr to current end of strings

		char ps[LZW_STRING_SIZE];                       // previous string
		size_t psLen = 0;                               // length of prevString
		uint32_t code;                                  // key to string for code
		uint32_t nextCode = 258;                        // used to preset string for next
		uint32_t incoming = (uint32_t)size;      // count down input chars
		int n = 0;                                      // output byte counter
		uint32_t iBuf = 0;                              // incoming bit buffer
		int32_t nBits = 0;                              // incoming bits in the buffer
		int32_t codeBits = 9;                           // number of bits to make code (9-12)
		uint32_t nextBump = 511;                        // when to increment code size 1st time
//		uint32_t pBuf = 0;                              // previous out bit buffer
		uint32_t mask = (1 << codeBits) - 1;            // extract code from iBuf

		uint32_t* pSrc;                                 // ptr to src for word copies
		uint32_t* pDst;                                 // ptr to dst for word copies

		// read incoming bytes into the bit buffer (iBuf) using the char pointer c
		while (incoming)
		{
			// GetNextCode
			iBuf = (iBuf << 8) | (uint8_t)*c++;         // make room in bit buf for char
			nBits += 8;
			--incoming;
			if (nBits < codeBits) {
				iBuf = (iBuf << 8) | (uint8_t)*c++;     // make room in bit buf for char
				nBits += 8;
				--incoming;
			}
			code = (iBuf >> (nBits - codeBits)) & mask; // extract code from buffer
			nBits -= codeBits;                          // update available bits to process

			// rest at start and when codes = max ~+ 4094
			if (code == CLEAR_CODE) {
				codeBits = 9;
				mask = (1 << codeBits) - 1;
				nextBump = 511;
				sEnd = s[257];
				nextCode = 258;
				psLen = 0;
				continue;
			}

			// finished (should not need as incoming counts down to zero)
			if (code == EOF_CODE)
			{
				return ret;
			}

			// new code then add prevString + prevString[0]
			// copy prevString
			if (code == nextCode)
			{
				s[code] = sEnd;
				switch(psLen) {
				case 1:
					*s[code] = ps[0];
					break;
				case 2:
					*s[code] = ps[0];
					*(s[code]+1) = ps[1];
					break;
				case 4:
					pDst = (uint32_t*)s[code];
					pSrc = (uint32_t*)&ps;
					*pDst = *pSrc;
					break;
				case 5:
				case 6:
				case 7:
				case 8:
					pDst = (uint32_t*)s[nextCode];
					pSrc = (uint32_t*)&ps;
					*pDst = *pSrc;
					*(pDst+1) = *(pSrc+1);
					break;
				default:
					memcpy(s[code], &ps, psLen);
				}

				// copy prevString[0]
				*(s[code] + psLen) = ps[0];
				sLen[code] = (int8_t)psLen + 1;
				sEnd = s[code] + psLen + 1;
			}

			for (uint32_t i = 0; i != (uint32_t)sLen[code]; i++)
			{
				if (n % bytesPerRow < 3)
				{
					*out++ = *(s[code] + i);
				}
				else
				{
					*out++ = *(s[code] + i) + *(out - 3);
				}
				++n;
				/*
				// output char string for code (add from left)
				// pBuf   00000000 11111111 22222222 33333333
				if (n % bytesPerRow == 0) pBuf = 0;
				char b = *(s[code] + i) + (uint8_t)(pBuf & 0xFF);
				*out++ = b;
				pBuf = (pBuf >> 8) | (uint32_t)((uint8_t)b << 16);
				++n;
				*/
			}

			// add string to nextCode (prevString + strings[code][0])
			// copy prevString
			if (psLen/* && nextCode <= MAXCODE*/)
			{
				s[nextCode] = sEnd;
				switch(psLen) {
				case 1:
					*s[nextCode] = ps[0];
					break;
				case 2:
					*s[nextCode] = ps[0];
					*(s[nextCode]+1) = ps[1];
					break;
				case 4:
					pDst = (uint32_t*)s[nextCode];
					pSrc = (uint32_t*)&ps;
					*pDst = *pSrc;
					break;
				case 5:
				case 6:
				case 7:
				case 8:
					pDst = (uint32_t*)s[nextCode];
					pSrc = (uint32_t*)&ps;
					*pDst = *pSrc;
					*(pDst+1) = *(pSrc+1);
					break;
				default:
					memcpy(s[nextCode], &ps, psLen);
				}

				// copy strings[code][0]
				*(s[nextCode] + psLen) = *s[code];

				sLen[nextCode] = (int8_t)(psLen + 1);
				sEnd = s[nextCode] + psLen + 1;
				++nextCode;
			}

			// strings[code][0] copy
			switch(sLen[code]) {
			case 1:
				ps[0] = *s[code];
				break;
			case 2:
				ps[0] = *s[code];
				ps[1] = *(s[code]+1);
				break;
			case 4:
				pSrc = (uint32_t*)s[code];
				pDst = (uint32_t*)&ps;
				*pDst = *pSrc;
				break;
			case 5:
			case 6:
			case 7:
			case 8:
				pSrc = (uint32_t*)s[code];
				pDst = (uint32_t*)&ps;
				*pDst = *pSrc;
				*(pDst+1) = *(pSrc+1);
				break;
			default:
				memcpy(&ps, s[code], (size_t)sLen[code]);
			}

			psLen = (size_t)sLen[code];

			// codeBits change
			if (nextCode == nextBump) {
				if (nextCode < maxCode) {
					nextBump = (nextBump << 1) + 1;
					++codeBits;
					mask = (1 << codeBits) - 1;
				}
				else if (nextCode == maxCode) continue;
				else {
					codeBits = 9;
					mask = (1 << codeBits) - 1;
					nextBump = 511;
					sEnd = s[257];
					nextCode = 258;
					psLen = 0;
				}
			}

		} // end while}

		return ret;
	}
};
