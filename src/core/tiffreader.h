#ifndef TIFFREADER_H
#define TIFFREADER_H
//
#include "convertion.h"


typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned short ushort;

enum class Tags : int {
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

	TileByteCounts = 325,

	ModelPixelScaleTag = 33550,

	ModelTiepointTag = 33922,

	GeoKeyDirectoryTag = 34735,

	ModelTransformationTag = 34264,

	NoData = 42113
};

enum GeotiffTags: ushort
{
	GTModelTypeGeoKey = 1024,//SHORT
	GTRasterTypeGeoKey = 1025, //DEPR
	GTCitationGeoKey = 1026, //ASCII
	GeographicTypeGeoKey = 2048, //SHORT
	GeogCitationGeoKey = 2049, //ascii
	ProjectedCSTypeGeoKey = 3072,//SHORT
};

struct ModelTiepoint
{
	double I, J, K, X, Y, Z;
	ModelTiepoint()
	{}
	ModelTiepoint(double i, double j, double k, double x, double y, double z)
	{
		I = i;
		J = j;
		K = k;
		X = x;
		Y = y;
		Z = z;
	}
};


struct ModelTiepoints
{
	ModelTiepoint *points = nullptr;
	int c = 0;


	void resize(int n) { points = new ModelTiepoint[n]; }
	~ModelTiepoints() { delete[] points; }

	void add(uchar* buffer)
	{
		double temp[6];
		int o = 0;
		temp[o++] = toDouble(buffer);
		buffer += sizeof(double);
		temp[o++] = toDouble(buffer);
		buffer += sizeof(double);
		temp[o++] = toDouble(buffer);
		buffer += sizeof(double);
		temp[o++] = toDouble(buffer);
		buffer += sizeof(double);
		temp[o++] = toDouble(buffer);
		buffer += sizeof(double);
		temp[o++] = toDouble(buffer);

		add(temp[0], temp[1], temp[2], temp[3], temp[4], temp[5]);
	}

	void add(double i, double j, double k, double x, double y, double z)
	{
		points[c++] = ModelTiepoint(i, j, k, x, y, z);
	}
};
struct Scale3d
{
	double x, y, z;
	Scale3d(double X =0, double Y = 0, double Z = 0)
	{
		x = X;
		y = Y;
		z = Z;
	}
};

struct GeoTiffTags
{
	short GTModelTypeGeoKey = 0;
	std::string GTCitationGeoKey; //ASCII
	short GeographicTypeGeoKey = 0; //SHORT
	std::string GeogCitationGeoKey;
	short ProjectedCSTypeGeoKey = 0; //SHORT
	short ProjectionGeoKey = 0;
	short ProjLinearUnitsGeoKey = 0;
	short ProjCoordTransGeoKey = 0;
};

struct TiffTags
{
	//A general indication of the kind of data contained in this subfile.
	size_t NewSubfileType = 0;

	//A general indication of the kind of data contained in this subfile.
	size_t SubfileType = 0;

	//The number of columns in the image, i.e., the number of pixels per row.
	size_t ImageWidth = 0;

	//The number of rows of pixels in the image.
	size_t ImageLength = 0;

	//Number of bits per component.
	size_t BitsPerSample = 0;

	//Compression scheme used on the image data.
	size_t Compression = 0;

	//The color space of the image data.
	size_t PhotometricInterpretation = 0;

	//For black and white TIFF files that represent shades of gray, the technique used to convert from gray to black and white pixels.
	size_t Threshholding = 0;

	//The width of the dithering or halftoning matrix used to create a dithered or halftoned bilevel file.
	size_t CellWidth = 0;

	//The length of the dithering or halftoning matrix used to create a dithered or halftoned bilevel file.
	size_t CellLength = 0;

	//The logical order of bits within a byte.
	size_t FillOrder = 0;

	//A string that describes the subject of the image.
	size_t ImageDescription = 0;

	//The scanner manufacturer.
	size_t Make = 0;

	//The scanner model name or number.
	size_t Model = 0;

	//For each strip, the byte offset of that strip.
	size_t StripOffsets = 0;
	char StripOffsetsType = 0;
	//The orientation of the image with respect to the rows and columns.
	size_t Orientation = 0;

	//The number of components per pixel.
	size_t SamplesPerPixel = 0;

	//The number of rows per strip.
	size_t RowsPerStrip = 0;

	//For each strip, the number of bytes in the strip after compression.
	size_t StripByteCounts = 0;
	char StripByteCountsType = 0;

	//The minimum component value used.
	size_t MinSampleValue = 0;

	//The maximum component value used.
	size_t MaxSampleValue = 0;

	//The number of pixels per ResolutionUnit in the ImageWidth direction.
	size_t XResolution = 0;

	//The number of pixels per ResolutionUnit in the ImageLength direction.
	size_t YResolution = 0;

	//How the components of each pixel are stored.
	size_t PlanarConfiguration = 0;

	//For each string of contiguous unused bytes in a TIFF file, the byte offset of the string.
	size_t FreeOffsets = 0;

	//For each string of contiguous unused bytes in a TIFF file, the number of bytes in the string.
	size_t FreeByteCounts = 0;

	//The precision of the information contained in the GrayResponseCurve.
	size_t GrayResponseUnit = 0;

	//For grayscale data, the optical density of each possible pixel value.
	size_t GrayResponseCurve = 0;

	//The unit of measurement for XResolution and YResolution.
	size_t ResolutionUnit = 0;

	//Name and version number of the software package(s) used to create the image.
	size_t Software = 0;

	//Date and time of image creation.
	size_t DateTime = 0;

	//Person who created the image.
	size_t Artist = 0;

	//The computer and/or operating system in use at the time of image creation.
	size_t HostComputer = 0;

	//A color map for palette color images.
	size_t ColorMap = 0;

	//Description of extra components.
	size_t ExtraSamples = 0;

	//Copyright notice.
	size_t Copyright = 0;

	size_t TileWidth = 0;

	size_t TileLength = 0;

	size_t TileOffsets = 0;
	char TileOffsetsType = 0;

	size_t TileByteCounts = 0;
	char TileByteCountsType = 0;

	float NoDataValue = -9999;

	// raster->model tiepoint pairs in the order
	// ModelTiepointTag = (...,I,J,K, X,Y,Z...)
	ModelTiepoints ModelTiepointTag;

	Scale3d ModelPixelScaleTag;

	double ModelTransformationTag[16];
};


#include <stdio.h>
#include <string>
#include <vector>
#include <queue>
#include <unordered_map>
#include <QVector3D>
#include <unordered_map>

//#include <bits/unique_ptr.h>

#include "decoder.h"

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

	virtual void free(int /*index*/)
	{

	}

	void setMaxElems(size_t number)
	{
		maxElementsSize = number;
		while( cachedData.size() > number)
		{
			remove();
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
			throw std::exception();

	}
	void setData(int i, T value)
	{
		auto t = cachedData.find(i);
		if (t != cachedData.end())
			t->second = value;
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
		for (auto &it : cachedData)
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
	PointerArrayCache(size_t maxElemCount = 16, size_t maxCachSize = 10000000, size_t sizeOfElement = sizeof(T*)):
		  Cache<T>(maxElemCount, maxCachSize,sizeOfElement)
	{  }

	void free(int index) override
	{
		auto it = this->cachedData.find(index);
		// TODO Fix It
		if (it->second)
		{
			delete[](float *) it->second;
			it->second = nullptr;
		}
//		qDebug() << "FREEE!";
	}
};


enum class ImageType { int8, int16, int32, float8, float16, float32, float64, rdb8, argb8 };
typedef float* rowptr;

class ImageReader
{
public:
	virtual rowptr getRowData(int ri)=0;
	virtual bool open(const wchar_t *path)=0;
	virtual void close()=0;
	virtual ImageType getType()=0;
	virtual int widght()=0;
	virtual int height()=0;
	PointerArrayCache<rowptr> cachedRows;
	bool ready = false;
	bool isTile = true;
	float max, min;

	virtual float getNullValue() = 0;
	rowptr getRow(int i)
	{
		rowptr d = nullptr;
		rowptr data = cachedRows.getData(i, d);
		if (data != nullptr)
			return data;
		else
		{
			data = getRowData(i);
			cachedRows.storeData(i, reinterpret_cast<rowptr>(data));
			return data;
		}
	}
	virtual ~ImageReader()
	{

	}
};

struct CoordProjection
{
	int wid = 1, hei = 1;
	//GeographicTypeGeoKey
	static std::unordered_map<int, CoordProjection> map;

	static CoordProjection getCoors(int a)
	{
		return map.at(a);
	}
};

class TiffReader: public ImageReader
{
	FILE * pFile;
	uint tilesCount = 0;
	int compressType = 1;
	PointerArrayCache<rowptr> cachedTiles;


	void **checkTileInCache(int x, int y);
	template<class T>
	T *setData(uchar *in, int len)
	{
		T *out = new T[len];
		for (int i = 0; i < len; ++i)
			convert(in + i * sizeof(T), out[i]);

		return out;
	}
public:
	TiffTags tiff;
	GeoTiffTags geotags;

	TiffReader();

	bool open(const wchar_t *path) override;
	void close() override;
	virtual ~TiffReader();
	void printValue(int x, int y);
	void printIFD(offu64 offset);
	void printBigIFD(size_t offset);
//	std::string dectode(uchar* bf, offu64 len);
	int printHeader(uchar *buffer);


	size_t getTagIntValue(size_t offOrValue, size_t count, char format, bool is64);

	void processGeoHeader(uchar *buffer);
	void processGeoentry(uchar *buffer);


	template<class T>
	void printTag(uchar *buffer, bool is64);
	void read(uchar *buffer, offu64 offset, offu64 len);
	void setTitleCacheSize(size_t n);
	void setRowsCacheSize(size_t n);
	// ImageReader interface
	rowptr getRowData(int ri) override;

	int widght() override;
	int height() override;

	ImageType getType() override;

	rowptr getTile(int x, int y);
	rowptr getTile(int ind);
	void removeTileFromCache(int ind);
	rowptr processData(uchar *bytes, int len);

	QVector3D getMaxModelSize()
	{
		return convertRasterToModel(QVector3D(widght(), 0, height()));
	}
	QVector3D convertModelToRaster(QVector3D modelCoord)
	{
		auto coord = CoordProjection::getCoors(geotags.GeographicTypeGeoKey);
		double x =(modelCoord.x() - tiff.ModelTiepointTag.points[0].X) * coord.wid;
		double y = (modelCoord.y() - tiff.ModelTiepointTag.points[0].Y) * coord.hei;
		double z = modelCoord.z() - tiff.ModelTiepointTag.points[0].Z;

		x /= tiff.ModelPixelScaleTag.x;
		y /= tiff.ModelPixelScaleTag.y;
		//		z /= tiff.ModelPixelScaleTag.z;
		z = 0;
		return QVector3D(x, z, y);
	}
	// Raster in x,z,y; model in x,y,z

	QVector3D convertRasterToModel(QVector3D rasterCoord)
	{

		double x = rasterCoord.x();
		double y = rasterCoord.z();
		double z = rasterCoord.y();

		x *= tiff.ModelPixelScaleTag.x;
		y *= tiff.ModelPixelScaleTag.y;
		z *= tiff.ModelPixelScaleTag.z;

		auto coord = CoordProjection::getCoors(geotags.GeographicTypeGeoKey);
		x += tiff.ModelTiepointTag.points[0].X * coord.wid;
		y += tiff.ModelTiepointTag.points[0].Y * coord.hei;
		z += tiff.ModelTiepointTag.points[0].Z;

		return QVector3D(x,y,z);
	}

	virtual float getNullValue() override
	{
		return tiff.NoDataValue;
	}
	float getFloatFromAscii(size_t offOrValue, size_t count, char format, bool is64);
	int getTileWid(int rowNum);
protected:
	void readAsciiFromBuffer(std::string &output, int offset, int Count);
};



#endif // TIFFREADER_H
