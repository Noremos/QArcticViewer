#include "tiffreader.h"

#ifndef QSLOT_CODE
#include <iostream>
#define OUT cout << endl
#else
#include <QDebug>
#define OUT qDebug()
#endif // !_QT

#include <stdio.h>

#include <algorithm>

using std::cout;
using std::string;
using std::vector;

#include <string>
#include <cassert>
string dectodeTEST(uchar* buffer, int len);
TiffReader::TiffReader()
{
	uint16_t x = 0x0001;
	sysByteOredr = *((uint8_t*)&x) ? 77 : 73;//b : l
//	uchar cs[]{ 0,1,0,2,5,0,3,9,8,6,4 };
	//	dectodeTEST(cs, 11);
}


union toduble
{
	uchar data[8];
	double value;
};
union tofloat
{
	uchar data[4];
	float value;
};
struct increnenter
{
	increnenter(int st, int ed, bool reverse)
	{
		rev = reverse;
		i = rev ? ed : st;
	}
	bool rev;
	int i = 0;
	int v()
	{
		return rev ? i-- : i++;
	}
};

template<class T>
struct increnenterBytes
{
	increnenterBytes(uchar *bytes, int st, int ed, bool reverse)
	{
		this->bytes = bytes;
		rev = reverse;
		i = rev ? ed : st;
	}
	uchar *bytes;
	bool rev;
	int i = 0;
	T v(int off)
	{
		return (T)bytes[(rev ? i-- : i++)] << off;
	}
};

float TiffReader::toFloat(uchar* bytes)
{
	tofloat conv;
	memcpy(conv.data, bytes, 4);
	//reorder(conv.data, 4);
	return conv.value;
}

double TiffReader::toDouble(uchar* bytes)
{
	toduble conv;
	memcpy(conv.data, bytes, 8);
	reorder(conv.data, 8);
	return conv.value;
}

ushort TiffReader::toShort(uchar* bytes)
{
	increnenter t(0, 1, imgByteOrder != sysByteOredr);

	return short((unsigned char)(bytes[t.v()]) << 8 | (bytes[t.v()]));
}

uint TiffReader::toInt(uchar* bytes)
{
	//alternative int* y; y = reinterpret_cast<int*>(bytes); return *y;
	increnenter t(0, 3, imgByteOrder != sysByteOredr);
	return int((unsigned char)(bytes[t.v()]) << 24 | (bytes[t.v()]) << 16 | (bytes[t.v()]) << 8 | (bytes[t.v()]));
}

long long TiffReader::toInt64(uchar *bytes)
{
	//alternative int* y; y = reinterpret_cast<int*>(bytes); return *y;
	increnenterBytes<long long> t(bytes, 0, 7, imgByteOrder != sysByteOredr);
	return t.v(56) |t.v(48) |t.v(40) |t.v(32) |t.v(24) | t.v(16) | t.v(8) | t.v(0);
}


void TiffReader::reorder(uchar* bytes, int size)
{
	if (sysByteOredr != imgByteOrder)
	{
		uchar* istart = bytes, * iend = istart + size;
		std::reverse(istart, iend);
	}
}

int TiffReader::printHeader(uchar* buffer)
{
	OUT << "----Header----";
	OUT << "Byte order:" << buffer[0] << buffer[1]; //“II”(4949.H)“MM” (4D4D.H).

	short versNum = toShort(buffer + 2);
	OUT << "Version number :" << (toShort(buffer + 2));
	if (versNum == 42)
	{
		// Standart tiff
		OUT << "Offset to first IFD:" << (toInt(buffer + 4));
	}
	else
	{
		// BigTiff
		OUT << "Bytesize of offsets :" << (toShort(buffer + 4));

		OUT << "Constant :" << (toShort(buffer + 6));
	}

	return versNum;
}
size_t TiffReader::getTagIntValue(size_t offOrValue, size_t count, char format, bool is64)
{
	if (is64)
	{
		if (count > 8)
		{
			uchar buffer[8]{ 0,0,0,0,0,0,0,0 };
			read(buffer, offOrValue, format == 4 ? 4 : format == 8 ? 8 : 2);
			return toInt64(buffer);
		}
		else
			return offOrValue;
	}
	else
	{
		if (count > 4)
		{
			uchar buffer[4]{ 0,0,0,0 };
			read(buffer, offOrValue, format == 4 ? 4 : 2);
			return toInt(buffer);
		}
		else
			return offOrValue;
	}
}

#include "side-src/fast_float/fast_float.h"


float TiffReader::getFloatFromAscii(size_t offOrValue, size_t count, char format, bool is64)
{
	uchar *buffer = new uchar[count];
	memset(buffer, 0, count);

	read(buffer, offOrValue, count);

	std::string s;
	s.append((char *) buffer, count);
	for (size_t i = 0; i < s.length(); ++i)
	{
		if (s[i] == ',')
			s[i] = '.';
	}
	float val = std::stof(s);
//	fast_float::from_chars((const char*)buffer, (const char*)(buffer + count), val);
	return val;
}

template <class T>
void TiffReader::printTag(uchar* buffer, bool is64)
{
//	OUT << "----Tag data----";
	ushort tag = toShort(buffer);
	ushort type = toShort(buffer + 2);
	T count = is64 ? toInt64(buffer+4) : toInt(buffer + 4);
	T value = is64 ? toInt64(buffer+12) : toInt(buffer + 8);

	bool print = true;
	switch ((Tags)tag)
	{
	case Tags::TileWidth:
		this->tiff.TileWidth = getTagIntValue(value, count, type, is64);
		break;
	case Tags::TileLength:
		this->tiff.TileLength = getTagIntValue(value, count, type, is64);
		break;
	case Tags::ImageWidth:
		this->tiff.ImageWidth = getTagIntValue(value, count, type, is64);
		OUT << "ImageWidth";
		break;
	case Tags::ImageLength:
		this->tiff.ImageLength = getTagIntValue(value, count, type, is64);
		OUT << "ImageLength";
		break;
	case Tags::PlanarConfiguration:
		this->tiff.PlanarConfiguration = getTagIntValue(value, count, type, is64);
		OUT << "PlanarConfiguration";
		break;
	case Tags::TileOffsets:
		isTile = true;
		tilesCount = count;
		this->tiff.TileOffsets = value;
		this->tiff.TileOffsetsType = type;
		break;

	case Tags::StripOffsets:
		this->tiff.StripOffsets = value;
		this->tiff.StripOffsetsType = type;
//		compressedLen = count;
//		this->tiff.ImageLength = count;
		OUT << "StripOffsets";
		break;

	case Tags::TileByteCounts:
		this->tiff.TileByteCounts = value;
		this->tiff.TileByteCountsType = type;
		tilesCount = count;
		break;

	case Tags::StripByteCounts:
		this->tiff.StripByteCounts = value;
		this->tiff.StripByteCountsType = type;
//		this->tiff.ImageLength = count;

		OUT << "StripByteCounts";
		break;

	case Tags::BitsPerSample:
		this->tiff.BitsPerSample = getTagIntValue(value, count, type, is64);
		OUT << "BitsPerSample";
		break;
	case Tags::SamplesPerPixel:
		this->tiff.SamplesPerPixel = getTagIntValue(value, count, type, is64);
		OUT << "SamplesPerPixel";
		break;
	case Tags::Compression:
		this->tiff.Compression = getTagIntValue(value, count, type, is64);
		OUT << "Compression";
		break;
	case Tags::NoData:
	{
		this->tiff.NoDataValue = getFloatFromAscii(value, count, type, is64);
	}
	default:
		print = false;
		break;
	}
	if (print && false)
	{

		OUT << "Tag identifying code:" << (tag);
		OUT << "Datatype of tag data:" << type;//3 - short; 4 -int 12 --ascii
		OUT << "Number of values:" << count;
		//	wid = toInt(buffer + 4);
		//dataOffset = toInt(buffer + 8);
		OUT << "Tag data or offset to tag data see below:" << value;
	}

	//In other words, if the tag data is smaller than or equal to 4 bytes, it fits. Otherwise, it is stored elsewhere and pointed to.
}

void TiffReader::read(uchar* buffer, offu64 offset, offu64 len)
{
#ifdef _MSC_VER
	_fseeki64(pFile, offset, SEEK_SET);
#else
	fseeko64(pFile, offset, SEEK_SET);
#endif
	if (!fread(buffer, 1, len, pFile))
		OUT << "ERROR?";
	if (feof(pFile))
		OUT << "ERROR2?";
}

void TiffReader::setTitleCacheSize(size_t n)
{
	cachedTiles.setMaxElems(n);
}

void TiffReader::setRowsCacheSize(size_t n)
{
	cachedRows.setMaxElems(n);
}


int TiffReader::widght()
{
	return this->tiff.ImageWidth;
}

int TiffReader::height()
{
	return this->tiff.ImageLength;
}

ImageType TiffReader::getType()
{
	return ImageType::float32;
}

//void **TiffReader::checkTileInCache(int tileX, int tileY)
//{
//	int TilesAcross = (tiff.ImageWidth + tiff.TileWidth - 1) / tiff.TileWidth;

//	int index = tileY * TilesAcross + tileX;
//	return cachedTiles.getData(tileNum, nullptr);
//}

int getTypeSize(uchar type)
{
	//https://www.itu.int/itudoc/itu-t/com16/tiff-fx/docs/tiff6.pdf
	//  0			 1		  2			   3		 4
	//  nullptr,     "BYTE",  "ASCII",     "SHORT",  "LONG",
	//  5			 6		  7			   8		 9
	//	"RATIONAL",  "SBYTE", "UNDEFINED", "SSHORT", "SLONG",
	//  10			 11		  12		   13		 14
	//	"SRATIONAL", "FLOAT", "DOUBLE",    "IFD",    0,
	//  15			 16		  17		   18
	//  0,			 "LONG8", "SLONG8",    "IFD8"
	static const int sizes[]{0, 1, 1, 2, 4,/**/
							 0, 1, 0, 2, 4,/**/
							 0, 4, 8, 0, 0,/**/
							 0, 8, 8, 0};
	return sizes[type];
//	return type == 4 ? 4 : 2;
}

int getAddnl(int main, int minor)
{
	return (main / minor) + (main % minor == 0 ? 0 : 1);
}

int TiffReader::getTileWid(int x)
{
	//tiff.ImageWidth = 1001
	//tiff.TileWidth = 100
	//rowNum = 10
	//(1001 - 100*10) = 0001 < 100 = 1
	//(1001 - 100*5) = 501 > 100= 10
	size_t widleft = (tiff.ImageWidth - tiff.TileWidth * x);
	return widleft < tiff.TileWidth ? widleft : tiff.TileWidth;
}

//Offsets are ordered left-to-right and top-to-bottom.
rowptr TiffReader::getTile(int x, int y)
{
	int TilesInWid = getAddnl(tiff.ImageWidth, tiff.TileWidth);
	//	int TilesInHei = getAddnl(tiff.ImageLength, tiff.TileLength);
	int tileNum = y * TilesInWid + x;
	return getTile(tileNum);
}


rowptr TiffReader::getTile(int ind)
{
	rowptr null = nullptr;

	rowptr data = cachedTiles.getData(ind, null);
//	const int bytsInTileWid = tiff.TileWidth * sizeof(float);

	if (data == null)
	{
		//offset tile
		uchar buffer[8];

		char sie = getTypeSize(tiff.TileOffsetsType);
		read(buffer, tiff.TileOffsets + (ind) * sie, sie);
		size_t off = (sie==4? toInt(buffer) : toInt64(buffer));
		//************

		//Count
		sie = getTypeSize(tiff.TileOffsetsType);
		read(buffer, tiff.TileByteCounts + (ind) * sie, sie);
		size_t count = (sie==4? toInt(buffer) : toInt64(buffer));
		uchar *buff = new uchar[count];
		//*****

		//data
		read(buff, off, count);
		decorder decod(tiff.Compression);

		vector<uchar> temp;
		decod.decompress(buff, count, temp); // (rowInTile + 1) * bytsInTileWid
		delete[] buff;

//		size_t ft = bytsInTileWid * tiff.TileLength;
		data = processData(temp.data(), tiff.TileWidth* tiff.TileLength);
		cachedTiles.storeData(ind, data);
	}

	return data;
}

void TiffReader::removeTileFromCache(int ind)
{
	if (cachedTiles.isInCache(ind))
	{
		cachedTiles.setData(ind, nullptr);
	}
}

rowptr TiffReader::processData(uchar* bytes, int len)
{
	rowptr data;
//	switch (getType())
//	{
//	case ImageType::float32:
		data = setData<float>(bytes, len);
//	}
	return data;
}

rowptr TiffReader::getRowData(int y)
{
	if (tiff.TileWidth != 0)
	{
		//Offsets are ordered left-to-right and top-to-bottom.

		// Сначала вычисляет в каком тайле есть нужная строка,
		//
		int TilesInRow = getAddnl(tiff.ImageWidth, tiff.TileWidth);
//		int TilesDown = (tiff.ImageLength + tiff.TileLength - 1) / tiff.TileLength;
//		int TilesPerImage = TilesAcross * TilesDown;
		cachedTiles.setMaxElems(TilesInRow+2);

		//	    col0  col1  col2
		//row0	/\ /\ /\ /\ /\
		//		\/ \/ \/ \/ \/
		//row1	/\ /\ /\ /\ /\
		//		\/ \/ \/ \/ \/
		//row2	/\ /\ /\ /\ /\
		//		\/ \/ \/ \/ \/


		//|------|------$-x----|-----|----|-----
		//-------------rowTileNum----

		//Номер тайла Y
		int colTileNum = (y / tiff.TileLength);

		// Строка в тайле row == widht
		int rowInTileas = y % tiff.TileLength;

		rowptr imageRow = new float[tiff.ImageWidth];
		for (int i = 0; i < TilesInRow; ++i)
		{
			float* data = getTile(i, colTileNum) + rowInTileas * tiff.TileWidth;
			// Add to row
			//			ret.reserve( ret.size() + bytsInTileWid );
			const unsigned long currentTileWid = getTileWid(i);
			const unsigned long rowOffset = i * tiff.TileWidth;

			if (rowOffset + currentTileWid > tiff.ImageWidth)
				assert(false);

			memcpy(imageRow + rowOffset, data, currentTileWid * sizeof (float));
		}
		return imageRow;

	}
	else
	{
		vector<uchar> ret;
		ret.reserve(tiff.ImageWidth + 10);

		uchar buffer[4];

		char sie = getTypeSize(tiff.StripOffsetsType);
		read(buffer, tiff.StripOffsets + y * sie, sie);
		uint off =  sie == 2 ? toShort(buffer) : toInt(buffer);

		sie = getTypeSize(tiff.StripByteCountsType);
		read(buffer, tiff.StripByteCounts + y * sie, sie);
		uint count =  sie == 2 ? toShort(buffer) : toInt(buffer);
		uchar *buff = new uchar[count];

		read(buff, off, count);

		string st = ""; // dectode(buff, count);
		decorder decod(tiff.Compression);
		decod.decompress(buff, count, ret);
		delete[] buff;
		return processData(ret.data(), widght());
	}
}

void TiffReader::printIFD(offu64 offset)
{
	//OUT << "----IDF----";
	uchar* buffer;
	uchar temp[2];

	read((uchar*)temp, offset, 2);
	ushort tagNums = toShort(temp);
	//OUT << "Number of tags in IFD:" << tagNums << endl;

	//--- after all entries 2+n*12	32-bit	offset to next directory or zero
	// 2 for tagNums and 4 for offset to next directory or zero

	buffer = new uchar[12 * tagNums + 4];
	read(buffer, offset + 2, 12 * tagNums + 4);

	for (int i = 0; i < tagNums; ++i)
	{
		printTag<uint>(buffer + 12 * i, false);
	}
	uint oofset = toInt(buffer + tagNums * 12);

	delete[] buffer;

	if (oofset != 0)
		printIFD(oofset);
}

void TiffReader::printBigIFD(size_t offset)
{
	//OUT << "----IDF----";
	uchar *buffer;
	uchar temp[8];

	// number of directory entries
	read((uchar *) temp, offset, 8);
	size_t tagNums = toInt64(temp);
	//OUT << "Number of tags in IFD:" << tagNums << endl;


	//--- after all entries
	// 8+n*20	64-bit	offset to next directory or zero
	int TAG_SIZE = 20;
	int OFFSET_TO_FIRST_TEG = 8;
	int SIZE_OF_NEXT_IFD = 8;

	const int toReadCount = TAG_SIZE * tagNums + SIZE_OF_NEXT_IFD;

	buffer = new uchar[toReadCount];
	read(buffer, offset + OFFSET_TO_FIRST_TEG, toReadCount);

	for (size_t i = 0; i < tagNums; ++i)
	{
		printTag<long long>(buffer + TAG_SIZE * i, true);
	}
	size_t oofset = toInt64(buffer + tagNums * TAG_SIZE);

	delete[] buffer;

	if (oofset != 0)
		printBigIFD(oofset);
}

void TiffReader::printValue(int x, int y)
{
	uchar buffer[4];
	read(buffer, tiff.StripOffsets + y * 4, getTypeSize(tiff.StripOffsetsType));
	int off = toInt(buffer);

	read(buffer, tiff.StripByteCounts + y * 4, getTypeSize(tiff.StripByteCountsType));
	int count = toInt(buffer);

	uchar* buff = new uchar[count];
	read(buff, off, count);


	string st = "";// dectode(buff, count);
	vector<uchar> ret;
	decorder decod(tiff.Compression);
	decod.decompress(buff, count, ret);

		//tiff_lzw_decode(buff, count, st);
	OUT << st.length();
	//string vals = st.substr(x * 4, 4);
	//float temp = toFloat((uchar*)vals.c_str());
	float temp = toFloat(ret.data() +  x*4);

	int val = (int)round(temp * 10000000);
	OUT << val;
	OUT << (val) / 10000000.;
}

bool TiffReader::open(const wchar_t* path)
{
	uchar buffer[8];
	ready = false;
	isTile = false;

	_wfopen_s(&pFile, path, L"rb");
	if (pFile == NULL)
	{
		perror("Error opening file");
		return false;
	}
	else
	{
		read(buffer, 0, 8);
		imgByteOrder = buffer[0];
		int verNum = printHeader(buffer);
		if (verNum==42)
		{
			// Standart tiff
			uint idfOffset = toInt(buffer + 4);
			printIFD(idfOffset);
		}
		else
		{
			read(buffer, 8, 8);
			size_t idfOffset = toInt64(buffer);
			OUT << "Offset to first IFD:" << idfOffset;

			// BigTiff tiff
			printBigIFD(idfOffset);
		}
	}
	ready = true;
	return true;
}

TiffReader::~TiffReader()
{
	close();
}

void TiffReader::close()
{
	if (pFile)
		fclose(pFile);

	cachedRows.clear();
	cachedTiles.clear();
}
void parceTage(TiffTags& st, Tags tag, int value)
{

	switch (tag)
	{
	case Tags::NewSubfileType:
		st.NewSubfileType = value;
		break;
	case Tags::SubfileType:
		st.SubfileType = value;
		break;
	case Tags::ImageWidth:
		st.ImageWidth = value;
		break;
	case Tags::ImageLength:
		st.ImageLength = value;
		break;
	case Tags::BitsPerSample:
		st.BitsPerSample = value;
		break;
	case Tags::Compression:
		st.Compression = value;
		break;
	case Tags::PhotometricInterpretation:
		st.PhotometricInterpretation = value;
		break;
	case Tags::Threshholding:
		st.Threshholding = value;
		break;
	case Tags::CellWidth:
		st.CellWidth = value;
		break;
	case Tags::CellLength:
		st.CellLength = value;
		break;
	case Tags::FillOrder:
		st.FillOrder = value;
		break;
	case Tags::ImageDescription:
		st.ImageDescription = value;
		break;
	case Tags::Make:
		st.Make = value;
		break;
	case Tags::Model:
		st.Model = value;
		break;
	case Tags::StripOffsets:
		st.StripOffsets = value;
		break;
	case Tags::Orientation:
		st.Orientation = value;
		break;
	case Tags::SamplesPerPixel:
		st.SamplesPerPixel = value;
		break;
	case Tags::RowsPerStrip:
		st.RowsPerStrip = value;
		break;
	case Tags::StripByteCounts:
		st.StripByteCounts = value;
		break;
	case Tags::MinSampleValue:
		st.MinSampleValue = value;
		break;
	case Tags::MaxSampleValue:
		st.MaxSampleValue = value;
		break;
	case Tags::XResolution:
		st.XResolution = value;
		break;
	case Tags::YResolution:
		st.YResolution = value;
		break;
	case Tags::PlanarConfiguration:
		st.PlanarConfiguration = value;
		break;
	case Tags::FreeOffsets:
		st.FreeOffsets = value;
		break;
	case Tags::FreeByteCounts:
		st.FreeByteCounts = value;
		break;
	case Tags::GrayResponseUnit:
		st.GrayResponseUnit = value;
		break;
	case Tags::GrayResponseCurve:
		st.GrayResponseCurve = value;
		break;
	case Tags::ResolutionUnit:
		st.ResolutionUnit = value;
		break;
	case Tags::Software:
		st.Software = value;
		break;
	case Tags::DateTime:
		st.DateTime = value;
		break;
	case Tags::Artist:
		st.Artist = value;
		break;
	case Tags::HostComputer:
		st.HostComputer = value;
		break;
	case Tags::ColorMap:
		st.ColorMap = value;
		break;
	case Tags::ExtraSamples:
		st.ExtraSamples = value;
		break;
	case Tags::Copyright:
		st.Copyright = value;
		break;
	default:
		break;
	}
}
