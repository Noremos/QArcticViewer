#include "tiffreader.h"

using namespace std;
#ifndef _QT
#include <iostream>
#define OUT cout << endl
#else
#define OUT qDebug();
#include <QDebug>
#endif // !_QT

#include <stdio.h>

#include <algorithm>


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


void TiffReader::reorder(uchar* bytes, int size)
{
	if (sysByteOredr != imgByteOrder)
	{
		uchar* istart = bytes, * iend = istart + size;
		std::reverse(istart, iend);
	}
}

void TiffReader::printHeader(uchar* buffer)
{
	OUT << "----Header----";
	OUT << "Byte order:" << buffer[0] << buffer[1]; //“II”(4949.H)“MM” (4D4D.H).
	OUT << "Version number :" << (toShort(buffer + 2));
	OUT << "Offset to first IFD:" << (toInt(buffer + 4)) << endl;
}
int TiffReader::getTagIntValue(offu64 offOrValue, offu64 count, char format)
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
void TiffReader::printTag(uchar* buffer)
{
//	OUT << "----Tag data----";
	ushort tag = toShort(buffer);
	ushort type = toShort(buffer + 2);
	offu64 count = toInt(buffer + 4);
	offu64 value = toInt(buffer + 8);

	bool print = true;
	switch ((Tags)tag)
	{
	case Tags::TileWidth:
		this->tiff.TileWidth = getTagIntValue(value, count, type);
		break;
	case Tags::TileLength:
		this->tiff.TileLength = getTagIntValue(value, count, type);
		break;
	case Tags::ImageWidth:
		this->tiff.ImageWidth = getTagIntValue(value, count, type);
		OUT << "ImageWidth";
		break;
	case Tags::ImageLength:
		this->tiff.ImageLength = getTagIntValue(value, count, type);
		OUT << "ImageLength";
		break;
	case Tags::PlanarConfiguration:
		this->tiff.PlanarConfiguration = getTagIntValue(value, count, type);
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
		this->tiff.BitsPerSample = getTagIntValue(value, count, type);
		OUT << "BitsPerSample";
		break;
	case Tags::SamplesPerPixel:
		this->tiff.SamplesPerPixel = getTagIntValue(value, count, type);
		OUT << "SamplesPerPixel";
		break;
	case Tags::Compression:
		this->tiff.Compression = getTagIntValue(value, count, type);
		OUT << "Compression";
		break;

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
		OUT << "Tag data or offset to tag data see below:" << value << endl;
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

int getTypeSize(char type)
{
	// { nullptr,     "BYTE",  "ASCII",     "SHORT",  "LONG",
	//		"RATIONAL",  "SBYTE", "UNDEFINED", "SSHORT", "SLONG",
	//	"SRATIONAL", "FLOAT", "DOUBLE",    "IFD",    "LONG8",
	//"SLONG8",    "IFD8" };
	static const int sizes[]{0, 1, 1, 2, 4, 0, 1, 0, 2, 4, 0, 4, 8, 0, 1, 1, 0};
	return sizes[type];
//	return type == 4 ? 4 : 2;
}

uchar* TiffReader::getTile(int ind)
{
	uchar *n = nullptr;

	uchar *data = cachedTiles.getData(ind, n);
	const int bytsInTileWid = tiff.TileWidth * sizeof(float);

	if (data == n)
	{
		//offset tile
		uchar buffer[4];

		char sie = getTypeSize(tiff.TileOffsetsType);
		read(buffer, tiff.TileOffsets + (ind) * sie, sie);
		uint off = toInt(buffer);
		//************

		//Count
		sie = getTypeSize(tiff.TileOffsetsType);
		read(buffer, tiff.TileByteCounts + (ind) * sie, sie);
		uint count = toInt(buffer);
		uchar *buff = new uchar[count];
		//*****

		//data
		read(buff, off, count);
		decorder decod(tiff.Compression);

		vector<uchar> temp;
		decod.decompress(buff, count, temp); // (rowInTile + 1) * bytsInTileWid
		delete[] buff;

		size_t ft = bytsInTileWid * tiff.TileLength;
		data = new uchar[ft];

		memcpy(data, temp.data(), ft);
		cachedTiles.storeData(ind, data);
	}
	return data;
}

rowdta TiffReader::processData(uchar* bytes)
{
	int len = widght();
	rowdta data;
	switch (getType())
	{
	case ImageType::float32:
		data = setData<float>(bytes, len);
	}
	return data;
}

rowdta TiffReader::getRowData(int y)
{
	vector<uchar> ret;
	ret.reserve(tiff.ImageWidth);

	if (tiff.TileWidth != 0)
	{
		int TilesAcross = (tiff.ImageWidth + tiff.TileWidth - 1) / tiff.TileWidth;
//		int TilesDown = (tiff.ImageLength + tiff.TileLength - 1) / tiff.TileLength;
//		int TilesPerImage = TilesAcross * TilesDown;
		cachedTiles.setMaxElems(TilesAcross);

		int tileNum = (y / tiff.TileLength) * TilesAcross;
		int rowInTile = y % tiff.TileLength;

		const int bytsInTileWid = tiff.TileWidth * sizeof(float);
		for (int i = 0; i < TilesAcross; ++i)
		{
			uchar* data = getTile(tileNum+i);
			data += rowInTile * bytsInTileWid;
//			ret.reserve( ret.size() + bytsInTileWid );

			ret.insert(ret.end(), data, data + bytsInTileWid);
			continue;
		}
	}else
	{
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
	}

	rowdta data = processData(ret.data());
	ret.clear();
	return data;
}

void TiffReader::printIFD(offu64 offset)
{
	//OUT << "----IDF----";
	uchar* buffer;
	uchar temp[2];

	read((uchar*)temp, offset, 2);
	ushort tagNums = toShort(temp);


	//OUT << "Number of tags in IFD:" << tagNums << endl;

	buffer = new uchar[12 * tagNums + 6];
	read(buffer, offset, 12 * tagNums + 6);

	for (int i = 0; i < tagNums; ++i)
	{
		printTag(buffer + 2 + 12 * i);
	}
	uint oofset = toInt(buffer + tagNums * 12 + 2);

	delete[] buffer;

	if (oofset != 0)
		printIFD(oofset);
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
	uchar buffer[16];
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
		printHeader(buffer);
		uint idfOffset = toInt(buffer + 4);
		printIFD(idfOffset);
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
