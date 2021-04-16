#pragma once
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <QDebug>

#include "../base.h"

using std::cout;

#define CHECKRET \
	if (added + 1 >= maxVal) \
		{\
			qDebug() << "Out of tile";\
			return;\
		}

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
			ushort r = ushort(arry[k] << 8 | ((k + 1)<arrLen? arry[k + 1] : 0));
			r = r << l;
			r = r >> (16 - byteLength);
			return r;
		}
		else
		{
			uint r = uint(0 | arry[k] << 16 | ((k + 1)<arrLen? arry[k + 1] << 8 : 0) | ((k + 2) < arrLen? arry[k + 2]: 0));
			r = r << (l + 8);
			r = r >> (32 - byteLength);
			return ushort(r);
		}
	}
//	ushort getByte(uchar* arry, const int length)
//	{
//		const int d = position % 8;// bit start pos
//		const int a = position / 8;// byte start pos
//		const int de = 8 - d;// bits in byte that needs
//		const int ef = (position + length) - ((a + 1) * 8);// bits to reed?
//		int fg = (8 * (a + 2)) - (position + length);// start offset from second byte
//		const int dg = ((a + 2) * 8) - position;//need to reeds?
//		fg = max(0, fg);
//		if (a >= arrLen)
//		{
//			cout << ("ran off the end of the buffer before finding EOI_CODE (end on input code)");
//			return EOI_CODE;
//		}

//		int chunk1 = arry[a] & (st(8 - d) - 1);
//		chunk1 = chunk1 << (length - de);
//		int chunks = chunk1;//get bits from first byte
//		if (a + 1 < arrLen)
//		{
//			int chunk2 = arry[a + 1] >> fg;
//			chunk2 <<= max(0, (length - dg));
//			chunks += chunk2;
//		}
//		if (ef > 8 && a + 2 < arrLen)
//		{
//			const int hi = ((a + 3) * 8) - (position + length);
//			const int chunk3 = arry[a + 2] >> hi;
//			chunks += chunk3;
//		}
//		return chunks;
//	}

	ushort getAppedRev(ushort code)
	{
		size_t len = 0;
		for (ushort i = code; i != 4096; i = dictionaryIndex[i], ++len)
			;

		assert(len>0);

		ushort last = dictionaryChar[code];
		for (ushort i = code; i != 4096; i = dictionaryIndex[i])
		{
			result[added++ + --len] = dictionaryChar[i];
		}

		return last;
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
	ushort dictionaryIndex[4093];

	// short because code can be more than 256
	ushort dictionaryChar[4093];

	ushort dictionaryLength = 258;
	int byteLength = MIN_BITS;
	size_t position = 0, added = 0, maxSize= 0;

	uchar *result;
	uchar *arry;
	ushort code;
public:

	void decompress(uchar* input, offu64 size, uchar* output, size_t maxVal = UINT64_MAX)
	{
		if (comprType == 1)
		{
			output = input;
			return;
		}

		this->result = output;
		arry = input;

		maxSize = maxVal;

		memset(&dictionaryIndex, 0, 4093 * 2);
		memset(&dictionaryChar, 0, 4093  * 2);
		arrLen = size;
		position = 0;
		for (int i = 0; i <= 257; i++)
		{
			dictionaryIndex[i] = 4096;
			dictionaryChar[i] = i;
		}
		added = 0;
		initDictionary();
		getNext();
		ushort oldCode = 0;
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
					break;
				}
				else if (code > CLEAR_CODE)
				{
					throw std::exception();//"corrupted code at scanline ${ code }");
				}
				else
				{
					getAppedRev(code);
					CHECKRET
					oldCode = code;
				}
			}
			else if (code < dictionaryLength)
			{
				ushort last = getAppedRev(code);
				addToDictionary(oldCode, last);
				CHECKRET
				oldCode = code;
			}
			else
			{
				ushort last = getAppedRev(code);

				result[added++] = last;//oldVal[oldVal.size() - 1];
				CHECKRET
				addToDictionary(oldCode, last);
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

class Decoder2
{
	int comprType;

public:
	Decoder2(int compressType) : comprType(compressType) {}
	const int MIN_BITS = 9;
	const int CLEAR_CODE = 256; // clear code
	const int EOI_CODE = 257; // end of information
	const int MAX_BYTELENGTH = 12;
	size_t arrLen;


	enum {
		dictionarySize = 4095, // maximum number of entries defined for the dictionary (2^12 = 4096)
		codeLength = 12, // the codes which are taking place of the substrings
		maxValue = dictionarySize - 1
	};

	/*
 * A singly linked list serving as a dictionary.
 *
 */

#include <stdio.h>
#include <stdlib.h>

	enum {
		emptyPrefix = -1 // empty prefix for ASCII characters
	};

	// the "string" in the dictionary consists of the last byte of the string and an index to a prefix for that string
	struct DictNode
	{
		int value; // the position in the list
		int prefix; // prefix for byte > 255
		int character; // the last byte of the string
		struct DictNode *next;
	};

	//void dictionaryInit();
	//void appendNode(struct DictNode *node);
	//void dictionaryDestroy();
	//int dictionaryLookup(int prefix, int character);
	//int dictionaryPrefix(int value);
	//int dictionaryCharacter(int value);
	//void dictionaryAdd(int prefix, int character, int value);

	// the dictionary
	struct DictNode *dictionary, *tail;

	// initialize the dictionary of ASCII characters @12bits
	void dictionaryInit()
	{
		int i;
		struct DictNode *node;
		for (i = 0; i < 256; i++)
		{ // ASCII
			node = (struct DictNode *) malloc(sizeof(struct DictNode));
			node->prefix = emptyPrefix;
			node->character = i;
			appendNode(node);
		}
	}

	// add node to the list
	void appendNode(struct DictNode *node)
	{
		if (dictionary != NULL)
			tail->next = node;
		else
			dictionary = node;
		tail = node;
		node->next = NULL;
	}

	// destroy the whole dictionary down to NULL
	void dictionaryDestroy()
	{
		while (dictionary != NULL)
		{
			dictionary = dictionary->next; /* the head now links to the next element */
		}
	}

	// is prefix + character in the dictionary?
	int dictionaryLookup(int prefix, int character)
	{
		struct DictNode *node;
		for (node = dictionary; node != NULL; node = node->next)
		{ // ...traverse forward
			if (node->prefix == prefix && node->character == character)
				return node->value;
		}
		return emptyPrefix;
	}

	int dictionaryPrefix(int value)
	{
		struct DictNode *node;
		for (node = dictionary; node != NULL; node = node->next)
		{ // ...traverse forward
			if (node->value == value)
				return node->prefix;
		}
		return -1;
	}

	int dictionaryCharacter(int value)
	{
		struct DictNode *node;
		for (node = dictionary; node != NULL; node = node->next)
		{ // ...traverse forward
			if (node->value == value)
			{
				//printf("\nNODE %i %i %i\n", node->value, node->prefix, node->character);
				return node->character;
			}
		}
		return -1;
	}

	// add prefix + character to the dictionary
	void dictionaryAdd(int prefix, int character, int value)
	{
		struct DictNode *node;
		node = (struct DictNode *) malloc(sizeof(struct DictNode));
		node->value = value;
		node->prefix = prefix;
		node->character = character;
		//printf("\n(%i) = (%i) + (%i)\n", node->value, node->prefix, node->character);
		appendNode(node);
	}



	int leftover = 0;
	int leftoverBits;

	size_t readed = 0, tsize = 0;
	int readBinary(uchar *input)
	{
		uchar code = input[readed++];
		if (readed == tsize)
			return 0;

		if (leftover > 0)
		{
			code = (leftoverBits << 8) + code;

			leftover = 0;
		}
		else
		{
			uchar nextCode = input[readed++];

			leftoverBits = nextCode & 0xF; // save leftover, the last 00001111
			leftover = 1;

			code = (code << 4) + (nextCode >> 4);
		}
		return code;
	}

	size_t writed = 0;
	//int decode(int code, FILE *outputFile);

	// decompression
	// to reconstruct a string from an index we need to traverse the dictionary strings backwards, following each
	//   successive prefix index until this prefix index is the empty index
	void decompress(uchar *inputSrc, offu64 size, uchar *result/*, size_t maxVal = UINT64_MAX*/)
	{
		//		result.reserve()
		readed = 0;
		writed = 0;
		this->tsize = size;
		uchar *inputCur = inputSrc;
		// int prevcode, currcode
		int previousCode;
		int currentCode;
		int nextCode = 256; // start with the same dictionary of 256 characters

		int firstChar;

		// prevcode = read in a code
		previousCode = readBinary(inputCur);
		if (previousCode == 0)
		{
			return;
		}
		result[writed++] = previousCode;

		// while (there is still data to read)
		while ((currentCode = readBinary(inputCur)) > 0)
		{ // currcode = read in a code

			if (currentCode >= nextCode)
			{
				firstChar = decode(previousCode, result);
				result[writed++] = firstChar;
			}
			else
			{
				firstChar = decode(currentCode, result); // first character returned! [1.]
			}
			// add a new code to the string table
			if (nextCode < dictionarySize)
				dictionaryArrayAdd(previousCode, firstChar, nextCode++);

			// prevcode = currcode
			previousCode = currentCode;
		}
		//printf("\n");
	}



	typedef struct
	{
		int prefix; // prefix for byte > 255
		int character; // the last byte of the string
	} DictElement;

	//void dictionaryArrayAdd(int prefix, int character, int value);
	//int dictionaryArrayPrefix(int value);
	//int dictionaryArrayCharacter(int value);

	DictElement dictionaryArray[4095];

	// add prefix + character to the dictionary
	void dictionaryArrayAdd(int prefix, int character, int value)
	{
		dictionaryArray[value].prefix = prefix;
		dictionaryArray[value].character = character;
	}

	int dictionaryArrayPrefix(int value) { return dictionaryArray[value].prefix; }

	int dictionaryArrayCharacter(int value) { return dictionaryArray[value].character; }

	int decode(int code, uchar *outputFile)
	{
		int character;
		int temp;

		if (code > 255)
		{ // decode
			character = dictionaryArrayCharacter(code);
			temp = decode(dictionaryArrayPrefix(code), outputFile); // recursion
		}
		else
		{
			character = code; // ASCII
			temp = code;
		}
		outputFile[writed++] = character;
		return temp;
	}
};
