#ifndef TIFFREADER_H
#define TIFFREADER_H
//

typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned short ushort;

enum class Tags {
	//A general indication of the kind of data contained in this subfile.
	NewSubfileType = 254,

	//A general indication of the kind of data contained in this subfile.
	SubfileType = 255,

	//The number of columns in the image, i.e., the number of pixels per row.
	ImageWidth = 256,

	//The number of rows of pixels in the image.
	ImageLength = 257,

	//Number of bits per component.
	BitsPerSample = 258,

	//Compression scheme used on the image data.
	Compression = 259,

	//The color space of the image data.
	PhotometricInterpretation = 262,

	//For black and white TIFF files that represent shades of gray, the technique used to convert from gray to black and white pixels.
	Threshholding = 263,

	//The width of the dithering or halftoning matrix used to create a dithered or halftoned bilevel file.
	CellWidth = 264,

	//The length of the dithering or halftoning matrix used to create a dithered or halftoned bilevel file.
	CellLength = 265,

	//The logical order of bits within a byte.
	FillOrder = 266,

	//A string that describes the subject of the image.
	ImageDescription = 270,

	//The scanner manufacturer.
	Make = 271,

	//The scanner model name or number.
	Model = 272,

	//For each strip, the byte offset of that strip.
	StripOffsets = 273,

	//The orientation of the image with respect to the rows and columns.
	Orientation = 274,

	//The number of components per pixel.
	SamplesPerPixel = 277,

	//The number of rows per strip.
	RowsPerStrip = 278,

	//For each strip, the number of bytes in the strip after compression.
	StripByteCounts = 279,

	//The minimum component value used.
	MinSampleValue = 280,

	//The maximum component value used.
	MaxSampleValue = 281,

	//The number of pixels per ResolutionUnit in the ImageWidth direction.
	XResolution = 282,

	//The number of pixels per ResolutionUnit in the ImageLength direction.
	YResolution = 283,

	//How the components of each pixel are stored.
	PlanarConfiguration = 284,

	//For each string of contiguous unused bytes in a TIFF file, the byte offset of the string.
	FreeOffsets = 288,

	//For each string of contiguous unused bytes in a TIFF file, the number of bytes in the string.
	FreeByteCounts = 289,

	//The precision of the information contained in the GrayResponseCurve.
	GrayResponseUnit = 290,

	//For grayscale data, the optical density of each possible pixel value.
	GrayResponseCurve = 291,

	//The unit of measurement for XResolution and YResolution.
	ResolutionUnit = 296,

	//Name and version number of the software package(s) used to create the image.
	Software = 305,

	//Date and time of image creation.
	DateTime = 306,

	//Person who created the image.
	Artist = 315,

	//The computer and/or operating system in use at the time of image creation.
	HostComputer = 316,

	//A color map for palette color images.
	ColorMap = 320,

	//Description of extra components.
	ExtraSamples = 338,

	//Copyright notice.
	Copyright = 33432,

	TileWidth = 322,
	TileLength = 323,
	TileOffsets = 324,
	TileByteCounts = 325
};


struct TiffTags
{
	//A general indication of the kind of data contained in this subfile.
	uint NewSubfileType = 0;

	//A general indication of the kind of data contained in this subfile.
	uint SubfileType = 0;

	//The number of columns in the image, i.e., the number of pixels per row.
	uint ImageWidth = 0;

	//The number of rows of pixels in the image.
	uint ImageLength = 0;

	//Number of bits per component.
	uint BitsPerSample = 0;

	//Compression scheme used on the image data.
	uint Compression = 0;

	//The color space of the image data.
	uint PhotometricInterpretation = 0;

	//For black and white TIFF files that represent shades of gray, the technique used to convert from gray to black and white pixels.
	uint Threshholding = 0;

	//The width of the dithering or halftoning matrix used to create a dithered or halftoned bilevel file.
	uint CellWidth = 0;

	//The length of the dithering or halftoning matrix used to create a dithered or halftoned bilevel file.
	uint CellLength = 0;

	//The logical order of bits within a byte.
	uint FillOrder = 0;

	//A string that describes the subject of the image.
	uint ImageDescription = 0;

	//The scanner manufacturer.
	uint Make = 0;

	//The scanner model name or number.
	uint Model = 0;

	//For each strip, the byte offset of that strip.
	uint StripOffsets = 0;

	//The orientation of the image with respect to the rows and columns.
	uint Orientation = 0;

	//The number of components per pixel.
	uint SamplesPerPixel = 0;

	//The number of rows per strip.
	uint RowsPerStrip = 0;

	//For each strip, the number of bytes in the strip after compression.
	uint StripByteCounts = 0;

	//The minimum component value used.
	uint MinSampleValue = 0;

	//The maximum component value used.
	uint MaxSampleValue = 0;

	//The number of pixels per ResolutionUnit in the ImageWidth direction.
	uint XResolution = 0;

	//The number of pixels per ResolutionUnit in the ImageLength direction.
	uint YResolution = 0;

	//How the components of each pixel are stored.
	uint PlanarConfiguration = 0;

	//For each string of contiguous unused bytes in a TIFF file, the byte offset of the string.
	uint FreeOffsets = 0;

	//For each string of contiguous unused bytes in a TIFF file, the number of bytes in the string.
	uint FreeByteCounts = 0;

	//The precision of the information contained in the GrayResponseCurve.
	uint GrayResponseUnit = 0;

	//For grayscale data, the optical density of each possible pixel value.
	uint GrayResponseCurve = 0;

	//The unit of measurement for XResolution and YResolution.
	uint ResolutionUnit = 0;

	//Name and version number of the software package(s) used to create the image.
	uint Software = 0;

	//Date and time of image creation.
	uint DateTime = 0;

	//Person who created the image.
	uint Artist = 0;

	//The computer and/or operating system in use at the time of image creation.
	uint HostComputer = 0;

	//A color map for palette color images.
	uint ColorMap = 0;

	//Description of extra components.
	uint ExtraSamples = 0;

	//Copyright notice.
	uint Copyright = 0;

	uint TileWidth = 0;

	uint TileLength = 0;

	uint TileOffsets = 0;

	uint TileByteCounts = 0;

};


#include <stdio.h>
#include <string>
#include <vector>
#include <queue>
#include <unordered_map>

//#include <bits/unique_ptr.h>

#include "Decoder.h"

template<class T>
class Cache
{
protected:
	std::queue<int> cacheIndexs;
	std::unordered_map<int, T> cachedData;
	size_t maxElementsSize = 16;
	size_t maxCacheSize = 10000000;
	size_t elementSize;
public:
	Cache(size_t maxElemCount = 16, size_t maxCachSize = 10000000, size_t sizeOfElement = sizeof(T)):
		  maxElementsSize(maxElemCount), maxCacheSize(maxCachSize), elementSize(sizeOfElement)
	{ }

	virtual void free(int index)
	{

	}

	void setMaxElems(size_t number)
	{
		maxElementsSize = number;
		while( cachedData.size() > number)
		{
			int ol = cacheIndexs.front();
			cacheIndexs.pop();
			cachedData.erase(ol);
		}
	}
	void remove()
	{
		int ol = cacheIndexs.front();
		free(ol);
		cacheIndexs.pop();
		cachedData.erase(ol);
	}
	void setMaxSize(size_t size)
	{
		maxCacheSize = size;
		while( cachedData.size() * elementSize > size)
			remove();
	}
	void storeData(int i, T data)
	{
		size_t s = cachedData.size();
		if (s > maxElementsSize || s * elementSize > maxCacheSize)
			remove();

		cachedData.insert(std::pair<int, T>(i, data));
		cacheIndexs.push(i);
	}

	bool isInCache(int i) { return cachedData.find(i) != cachedData.end(); }

	T getData(int i)
	{
		auto t = cachedData.find(i);
		if (t != cachedData.end())
			return t->second;
		else
			throw exception();

	}
	T getData(int i, T& defaultValue)
	{
		auto t = cachedData.find(i);
		if (t != cachedData.end())
			return t->second;
		else
			return defaultValue;

	}
	void clear() {
		for (std::pair<int, T> &it : cachedData)
		{
			free(it.first);
		}
		cachedData.clear();
		std::queue<int> empty;
		std::swap( cacheIndexs, empty );
	}
};

template<class T>
class PointerCache : public Cache<T>
{
public:
	PointerCache(size_t maxElemCount = 16, size_t maxCachSize = 10000000, size_t sizeOfElement = sizeof(T)) :
		  Cache<T>(maxElemCount, maxCachSize, sizeOfElement) {}

	void free(int index) override
	{
		auto it = this->cachedData.find(index);
		delete it->second;
		it->second = nullptr;
	}
};

template<class T>
class PointerArrayCache : public Cache<T>
{
public:
	PointerArrayCache(size_t maxElemCount = 16, size_t maxCachSize = 10000000, size_t sizeOfElement = sizeof(T)):
		  Cache<T>(maxElemCount, maxCachSize,sizeOfElement)
	{  }

	void free(int index) override
	{
		auto it = this->cachedData.find(index);
		delete[] it->second;
		it->second = nullptr;
	}
};


enum class ImageType { int8, int16, int32, float8, float16, float32, float64, rdb8, argb8 };
class ImageReader
{
public:
	virtual void* getRowData(int ri)=0;
	virtual bool open(const wchar_t *path)=0;
	virtual void close()=0;
	virtual ImageType getType()=0;
	virtual int widght()=0;
	virtual int height()=0;
	PointerArrayCache<void*> cachedRows;
	bool ready = false;
	bool isTile = true;

	void *getRow(int i)
	{
		void *d = nullptr;
		void *data = cachedRows.getData(i, d);
		if (data != nullptr)
			return data;
		else
		{
			data = getRowData(i);
			cachedRows.storeData(i, data);
			return data;
		}
	}
	virtual ~ImageReader()
	{

	}
};

class TiffReader: public ImageReader
{
	FILE * pFile;
	TiffTags tiff;
	uchar imgByteOrder;
	uchar sysByteOredr;
	uint tilesCount = 0;
	Cache<uchar*> cachedTiles;


public:
	TiffReader();

	bool open(const wchar_t *path) override;
	void close() override;
	~TiffReader();
	void printValue(int x, int y);
	void printIFD(offu64 offset);
//	std::string dectode(uchar* bf, offu64 len);
	void printHeader(uchar *buffer);
	int getTagIntValue(offu64 offOrValue, offu64 count, char format);
	ushort toShort(uchar *mbytes);
	uint toInt(uchar *bytes);
	float toFloat(uchar* bytes);
	double toDouble(uchar *bytes);
	void printTag(uchar *buffer);
	void read(uchar* buffer, offu64 offset, offu64 len);

	// ImageReader interface
	void *getRowData(int ri) override;

	int widght() override;
	int height() override;

	// ImageReader interface

	// ImageReader interface
public:
	ImageType getType() override;


	void convert(uchar *bytes, float &out) { out = toFloat(bytes); }
	void convert(uchar *bytes, int &out) { out = toInt(bytes);}
	void convert(uchar* bytes, uchar& out)	{ out = bytes[0];}
	void convert(uchar* bytes, double& out)	{out = toDouble(bytes);}
	void convert(uchar* bytes, ushort& out)	{out = toShort(bytes);}
	void convert(uchar* bytes, short& out)	{out = toShort(bytes);}

	template<class T>
	T *setData(uchar *in, int len)
	{
		T *out = new T[len];
		for (int i = 0; i < len; ++i)
			convert(in + i * sizeof(T), out[i]);

		return out;
	}
	void reorder(uchar *bytes, int size);
	void **checkTileInCache(int x, int y);
};



#endif // TIFFREADER_H
