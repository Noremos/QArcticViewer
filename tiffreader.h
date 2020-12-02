#ifndef TIFFREADER_H
#define TIFFREADER_H
//

typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned short ushort;

enum class Tags
{
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

};


struct TiffTags
{
	//A general indication of the kind of data contained in this subfile.
	int NewSubfileType = 0;

	//A general indication of the kind of data contained in this subfile.
	int SubfileType = 0;

	//The number of columns in the image, i.e., the number of pixels per row.
	int ImageWidth = 0;

	//The number of rows of pixels in the image.
	int ImageLength = 0;

	//Number of bits per component.
	int BitsPerSample = 0;

	//Compression scheme used on the image data.
	int Compression = 0;

	//The color space of the image data.
	int PhotometricInterpretation = 0;

	//For black and white TIFF files that represent shades of gray, the technique used to convert from gray to black and white pixels.
	int Threshholding = 0;

	//The width of the dithering or halftoning matrix used to create a dithered or halftoned bilevel file.
	int CellWidth = 0;

	//The length of the dithering or halftoning matrix used to create a dithered or halftoned bilevel file.
	int CellLength = 0;

	//The logical order of bits within a byte.
	int FillOrder = 0;

	//A string that describes the subject of the image.
	int ImageDescription = 0;

	//The scanner manufacturer.
	int Make = 0;

	//The scanner model name or number.
	int Model = 0;

	//For each strip, the byte offset of that strip.
	int StripOffsets = 0;

	//The orientation of the image with respect to the rows and columns.
	int Orientation = 0;

	//The number of components per pixel.
	int SamplesPerPixel = 0;

	//The number of rows per strip.
	int RowsPerStrip = 0;

	//For each strip, the number of bytes in the strip after compression.
	int StripByteCounts = 0;

	//The minimum component value used.
	int MinSampleValue = 0;

	//The maximum component value used.
	int MaxSampleValue = 0;

	//The number of pixels per ResolutionUnit in the ImageWidth direction.
	int XResolution = 0;

	//The number of pixels per ResolutionUnit in the ImageLength direction.
	int YResolution = 0;

	//How the components of each pixel are stored.
	int PlanarConfiguration = 0;

	//For each string of contiguous unused bytes in a TIFF file, the byte offset of the string.
	int FreeOffsets = 0;

	//For each string of contiguous unused bytes in a TIFF file, the number of bytes in the string.
	int FreeByteCounts = 0;

	//The precision of the information contained in the GrayResponseCurve.
	int GrayResponseUnit = 0;

	//For grayscale data, the optical density of each possible pixel value.
	int GrayResponseCurve = 0;

	//The unit of measurement for XResolution and YResolution.
	int ResolutionUnit = 0;

	//Name and version number of the software package(s) used to create the image.
	int Software = 0;

	//Date and time of image creation.
	int DateTime = 0;

	//Person who created the image.
	int Artist = 0;

	//The computer and/or operating system in use at the time of image creation.
	int HostComputer = 0;

	//A color map for palette color images.
	int ColorMap = 0;

	//Description of extra components.
	int ExtraSamples = 0;

	//Copyright notice.
	int Copyright = 0;

};


#include <stdio.h>
#include <string>
#include <vector>
#include <queue>
#include <unordered_map>
enum class ImageType { int8, int16, int32, float8, float16, float32, float64, rdb8, argb8 };
class ImageReader
{
public:
	virtual void* getRowData(int ri)=0;
	virtual bool open(const char *path)=0;
	virtual void close()=0;
	virtual ImageType getType()=0;
	virtual int widght()=0;
	virtual int height()=0;
	std::queue<int> cashedRowIndexs;
	std::unordered_map<int, void *> cashedRows;

	void *getRow(int i)
	{
		auto t = cashedRows.find(i);
		if (t != cashedRows.end())
		{
			return t->second;
		}
		else
		{
			if (cashedRows.size() > 0)
			{
				int ol = cashedRowIndexs.front();
				float *temp = reinterpret_cast<float *>(cashedRows[ol]);
				delete[] temp;
				cashedRowIndexs.pop();
				cashedRows.erase(ol);
			}
			void *data = getRowData(i);
			cashedRows.insert(std::pair<int,void*>(i,data));
			cashedRowIndexs.push(i);
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
public:
	TiffReader();

	bool open(const char *path) override;
	void close() override;
	~TiffReader();
	void printValue(int x, int y);
	void printIFD(int offset);
	std::string dectode(uchar* bf, int len);
	void convert(uchar *bytes, int size);
	void printHeader(uchar *buffer);
	int getTagIntValue(int offOrValue, int count, char format);
	ushort toShort(uchar *mbytes);
	uint toInt(uchar *bytes);
	float toFloat(uchar* bytes);
	double toDouble(uchar *bytes);
	void printTag(uchar *buffer);
	void read(uchar* buffer, int offset, int len);

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
			convert(in + i * 4, out[i]);

		return out;
	}
};



#endif // TIFFREADER_H
