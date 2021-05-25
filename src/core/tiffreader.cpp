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


std::unordered_map<int, CoordProjection> CoordProjection::map{{4326, {1,-1}}};


string dectodeTEST(uchar* buffer, int len);
TiffReader::TiffReader()
{
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

/*
 * http://geotiff.maptools.org/spec/geotiff2.7.html#2.7
		 * Header={KeyDirectoryVersion, KeyRevision, MinorRevision, NumberOfKeys}
		 * KeyEntry = { KeyID, TIFFTagLocation, Count, Value_Offset }
		 * If TIFFTagLocation=0, then Value_Offset contains the actual (SHORT)
		 * TIFFTagLocation=GeoKeyDirectoryTag
		 * Example:
  GeoKeyDirectoryTag=(   1,     1, 2,     6,
					  1024,     0, 1,     2,
					  1026, 34737,12,     0,
					  2048,     0, 1, 32767,
					  2049, 34737,14,    12,
					  2050,     0, 1,     6,
					  2051, 34736, 1,     0 )
		 */
void TiffReader::processGeoHeader(uchar *buffer)
{
	ushort KeyDirectoryVersion = toShort(buffer);
	ushort KeyRevision = toShort(buffer + 2);
	ushort MinorRevision = toShort(buffer + 4);
	ushort NumberOfKeys = toShort(buffer + 6);

	OUT << "";
	OUT << "KeyDirectoryVersion: " << KeyDirectoryVersion;
	OUT << "KeyRevision: " << KeyRevision;
	OUT << "MinorRevision: " << MinorRevision;
	OUT << "NumberOfKeys: " << NumberOfKeys;
	OUT << "";

	for (int i = 0; i < NumberOfKeys; ++i)
	{
		processGeoentry(buffer + 8 + i * 8);
	}
	OUT << "";
}

void TiffReader::readAsciiFromBuffer(std::string &output, int offset, int Count)
{
	output.resize(Count, ' ');
	uchar *buff = (uchar *) output.c_str();
	read(buff, offset, Count);
}

void TiffReader::processGeoentry(uchar *buffer)
{
	ushort KeyID = toShort(buffer);
	ushort TIFFTagLocation = toShort(buffer + 2);
	ushort Count = toShort(buffer + 4);
	ushort Value_Offset = toShort(buffer + 6);

	size_t value = 0;

	if (TIFFTagLocation == 0)
		value = Value_Offset;

	switch (KeyID)
	{
	case GeotiffTags::GTModelTypeGeoKey:
		geotags.GTModelTypeGeoKey = value;
		OUT << "GTModelTypeGeoKey:" << value;
		break;
	case GeotiffTags::GTCitationGeoKey:
		readAsciiFromBuffer(geotags.GTCitationGeoKey, Value_Offset, Count);
		OUT << "GTCitationGeoKey:" << geotags.GTCitationGeoKey.c_str();
		break;
	case GeotiffTags::GTRasterTypeGeoKey:
//		geotags.GTRasterTypeGeoKey = value;
		OUT << "GTRasterTypeGeoKey (DEPR):" << value;
		break;
	case GeotiffTags::GeogCitationGeoKey:
		readAsciiFromBuffer(geotags.GeogCitationGeoKey, Value_Offset, Count);
		OUT << "GeogCitationGeoKey:" << geotags.GeogCitationGeoKey.c_str();
		break;
	case GeotiffTags::GeographicTypeGeoKey:
		geotags.GeographicTypeGeoKey = value;
		OUT << "GeographicTypeGeoKey:" << value;
		break;
	case GeotiffTags::ProjectedCSTypeGeoKey:
		geotags.ProjectedCSTypeGeoKey = value;
		OUT << "ProjectedCSTypeGeoKey:" << value;
		break;
	default:
		OUT << "Unknown geotag:" << KeyID << ", Value_Offset = " << Value_Offset;
//		return;
		break;
	}

//	OUT << "KeyID: " << KeyID;
//	OUT << "TIFFTagLocation: " << TIFFTagLocation;
//	OUT << "Count: " << Count;
//	OUT << "Value_Offset: " << Value_Offset;
}

#include "side-src/fast_float/fast_float.h"


float TiffReader::getFloatFromAscii(size_t offOrValue, size_t count, char /*format*/, bool /*is64*/)
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
//	fast_float::from_chars((const char*)buffer, (const char*)(buffer + count), val);
	return std::stof(s);
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
	case Tags::SampleFormat:
		this->tiff.SampleFormat = static_cast<tifftype>(value);
		break;

	case Tags::ModelTiepointTag:
	{
		uchar *buffer = new uchar[count * sizeof(double)];
		read(buffer, value, count * sizeof(double));
		int k = count / 6;
		this->tiff.ModelTiepointTag.resize(k);
		for (int lk = 0; lk < k; ++lk)
		{
			this->tiff.ModelTiepointTag.add(buffer + lk * 6 * sizeof(double));
		}
		delete[] buffer;
		OUT << "ModelTiepointTag";
		break;
	}
	case Tags::ModelPixelScaleTag: {
		assert(count == 3);

		uchar buffer[3 * sizeof(double)];
		read(buffer, value, count * sizeof(double));
		this->tiff.ModelPixelScaleTag.x = toDouble(buffer);
		this->tiff.ModelPixelScaleTag.y = toDouble(buffer +  sizeof(double));
		this->tiff.ModelPixelScaleTag.z = toDouble(buffer +  sizeof(double) * 2);

		OUT << "ModelPixelScaleTag";
		break;
	}
	case Tags::ModelTransformationTag: {
		assert(count == 16);
		uchar buffer[16 * sizeof(double)];
		read(buffer, value, count * sizeof(double));
		for (size_t i = 0; i < count; ++i)
		{
			this->tiff.ModelTransformationTag[i] = toDouble(buffer + sizeof(double) * i);
		}

		OUT << "ModelTransformationTag" << this->tiff.ModelTransformationTag;
		break;
	}
	case Tags::NoData:
		this->tiff.NoDataValue = getFloatFromAscii(value, count, type, is64);
		OUT << "Nodata";
		break;

	case Tags::GeoKeyDirectoryTag:
	{
		uchar* buffer = new uchar[count * sizeof(short)];
		read(buffer, value, count * sizeof(short));
		processGeoHeader(buffer);
		break;
	}

	default:
		OUT << "Tag:" << (tag) << "; Value: " << value;
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
	switch (tiff.SampleFormat)
	{
	case tifftype::tiff_uint:
		return ImageType::int32;
		break;
	case tifftype::tiff_short:
		return ImageType::int16;
		break;
	case tifftype::tiff_float:
		return ImageType::float32;
		break;
	case tifftype::tiff_void:
	default:
		std::exception();
	}
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

		//data
		uchar *buff = new uchar[count + 3];
		read(buff, off, count);
		buff[count] = 0;
		buff[count + 1] = 0;
		buff[count + 2] = 0;
	
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
	rowptr data = nullptr;
	switch (getType())
	{
	case ImageType::int16:
		data = (rowptr)setData<short>(bytes, len);
		break;
	case ImageType::float32:
		data = setData<float>(bytes, len);
		break;
	default:
		break;
	}
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

		/*
				col0  col1  col2
		row0	/\ /\ /\ /\ /\
				\/ \/ \/ \/ \/
		row1	/\ /\ /\ /\ /\
				\/ \/ \/ \/ \/
		row2	/\ /\ /\ /\ /\
				\/ \/ \/ \/ \/
		*/

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

		uchar *buff = new uchar[count + 3];
		read(buff, off, count);
		buff[count] = 0;
		buff[count + 1] = 0;
		buff[count + 2] = 0;

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

	uchar* buff = new uchar[count + 3];
	read(buff, off, count);
	buff[count] = 0;
	buff[count + 1] = 0;
	buff[count + 2] = 0;


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
		increnenter::GLOBALloclByteOreder = buffer[0];
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
	this->close();
}

void TiffReader::close()
{
	if (pFile)
		fclose(pFile);

	cachedRows.clear();
	cachedTiles.clear();
}
