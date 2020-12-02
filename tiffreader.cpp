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
#include "Decoder.h"

#include <algorithm>

string dectodeTEST(uchar* buffer, int len);
#include <map>
#include <unordered_map>
typedef vector<int> bytesCode;
//typedef unordered_map<string, int> dict;
//typedef pair<string, int> ppair;
typedef unordered_map<int, string> dictT;
typedef pair< int, string> ppairT;
typedef unordered_map<int, string> dict;
typedef pair< int, string> ppair;
#include <string>
#include <cassert>
string dectodeTEST(uchar* buffer, int len);
TiffReader::TiffReader()
{
	uint16_t x = 0x0001;
	sysByteOredr = *((uint8_t*)&x) ? 77 : 73;//b : l
	uchar cs[]{ 0,1,0,2,5,0,3,9,8,6,4 };
	//	dectodeTEST(cs, 11);
}


// Method to convert integer to its binary value
string converToBinary(uchar value, int len = 8)
{
	string finalValue = "";
	uchar mask = 128;
	for (size_t i = 0; i < 8; i++)
	{
		finalValue += value & mask ? "1" : "0";
		mask >> 1;
	}

	return finalValue;
}

string dectodeTEST(uchar* buffer, int len)
{
	dict codes;
	for (uchar i = 0; i < 5; i++)
	{
		char c = 'a' + i;
		string st = " ";
		st[0] = c;
		codes.insert(ppair(i, st));
	}

	string full = "";
	string res = "";
	bytesCode codestrim;
	int lastI = 5;
	string chast = "";
	for (size_t i = 0; i < len; i++)
	{
		string out = codes[buffer[i]];
		res += out;
		full = chast + out[0];
		bool found = false;
		for (auto& val : codes)
		{
			if (val.second == full)
			{
				found = true;
				break;
			}
		}
		if (!found)
		{
			codes.insert(ppair(lastI++, full));
			//full = chast;
		}
		chast = out;
	}
	bool b = res == string("abacabadabacabae");
	cout << b;
	return res;
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
	convert(conv.data, 4);
	return conv.value;
}

double TiffReader::toDouble(uchar* bytes)
{
	toduble conv;
	memcpy(conv.data, bytes, 8);
	convert(conv.data, 8);
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

void TiffReader::convert(uchar* bytes, int size)
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
int TiffReader::getTagIntValue(int offOrValue, int count, char format)
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
	OUT << "----Tag data----";
	ushort tag = toShort(buffer);
	ushort type = toShort(buffer + 2);
	int count = toInt(buffer + 4);
	int value = toInt(buffer + 8);

	bool print = true;
	switch ((Tags)tag)
	{
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
	case Tags::StripOffsets:
		this->tiff.StripOffsets = value;
		OUT << "StripOffsets";
		break;

	case Tags::StripByteCounts:
		this->tiff.StripByteCounts = value;
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

	default:
		print = false;
		break;
	}
	if (print)
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
void TiffReader::read(uchar* buffer, int offset, int len)
{
	fseek(pFile, offset, SEEK_SET);
	if (!fread(buffer, 1, len, pFile))
		OUT << "ERROR?";
	if (feof(pFile))
		OUT << "ERROR2?";
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



void *TiffReader::getRowData(int y)
{
	uchar buffer[4];
	read(buffer, tiff.StripOffsets + y * 4, 4);
	int off = toInt(buffer);

	read(buffer, tiff.StripByteCounts + y * 4, 4);
	int count = toInt(buffer);

	uchar *buff = new uchar[count];
	read(buff, off, count);

	string st = ""; // dectode(buff, count);
	vector<uchar> ret(count);
	decorder decod;
	decod.decompress(buff, count, ret);

	int len = widght();
	void *data;
	switch (getType())
	{
	case ImageType::float32:
		data = setData<float>(ret.data(), len);
	}
	ret.clear();
	return data;
}

void TiffReader::printIFD(int offset)
{
	OUT << "----IDF----";
	uchar* buffer;
	uchar temp[2];
	fseek(pFile, offset, SEEK_SET);
	fgets((char*)temp, 2, pFile);
	read((uchar*)temp, offset, 2);


	ushort tagNums = toShort(temp);
	OUT << "Number of tags in IFD:" << tagNums << endl;

	buffer = new uchar[12 * tagNums + 6];
	read(buffer, offset, 12 * tagNums + 6);

	for (int i = 0; i < tagNums; ++i)
	{
		printTag(buffer + 2 + 12 * i);
	}
	int oofset = toInt(buffer + tagNums * 12 + 2);

	delete[] buffer;

	if (oofset != 0)
		printIFD(oofset);
}
/* string TiffReader::dectode(uchar* bf, int len)
{
	size_t realLen = len * 8 / 9;
	ushort* buffer = new ushort[len / 2];
	int t = 0;
	for (size_t i = 0; i < realLen; i += 9)
	{
		//9 bit
		int k = i / 8;
		int l = i % 8;
		ushort r = ushort(bf[k] << 8 | bf[k + 1]);
		int ds = 16 - 9 - l;
		r = r << l;
		r = r >> ds + l;
		buffer[t++] = r;
	}
	buffer = buffer + 1;
	len = realLen;
	//uchar* buffer = bf;
	dict codes;
	for (short i = 0; i < 256; i++)
	{
		string st = " ";
		st[0] = i;
		codes.insert(ppair(i, st));
	}

	string full = "";
	string res = "";
	bytesCode codestrim;
	int lastI = 256;
	string chast = "";


	for (size_t i = 0; i < len; i++)//TIFF ROWSPERSTRIP * IMAGEWIDTH * BITSPERSAMPLE / 8,
	{
		string out = codes[buffer[i]];
		res += out;
		full = chast + out[0];
		bool found = false;
		for (auto& val : codes)
		{
			if (val.second == full)
			{
				found = true;
				break;
			}
		}
		if (!found)
		{
			codes.insert(ppair(lastI++, full));
			//full = chast;
		}
		chast = out;
	}
	buffer -= 1;
	delete[] buffer;
	return res;
}
ushort get_bits_in_stream(uchar* byteBuffer, int byteBufferSize, int bitStarts, int bitsLen)
{
	//assert(bitStarts != 2363 - 9);
	if (bitStarts == 2363 - 9)
		OUT;
	int BYTE_SIZE = 8;
	int byteOffset = bitStarts % 8;
	int byteStart = bitStarts / 8;
	size_t bitsCount = byteBufferSize * 8;

	ushort r = ushort(byteBuffer[byteStart] << 8 | byteBuffer[byteStart + 1]);
	//  {offset}**{bitsLen}**{rightBitsCount} = 16

	int rightBitsCount = 16 - bitsLen - byteOffset;

	r = r << byteOffset;
	//  {bitsLen}**{rightBitsCount}**{zeros==byteOffset}

	r = r >> rightBitsCount + byteOffset;
	//  {zeros==rightBitsCount}**{zeros==byteOffset}**{bitsLen==actialy data}
	return r;
}
 
void lzw_dict_init(string& d, wstring dindex)
{
	d.clear();
	for (short i = 0; i < 256; i++)
	{
		string st = " ";
		st[0] = i;
		dindex += {4096};
		//d.insert(ppair(i, st));
		d += st;
	}
	d += {0, 0};
	dindex += {4096, 4096};


}
*/
/*void tiff_lzw_calc_maxcode(int bpc, int* mc)
{
	*mc = 12;
}

void tiff_lzw_calc_bpc(int new_inc, int* bpc, int* max)
{
	int mask = int(1) << 30;// << *max
	int nb = *bpc;
	while ((mask & (new_inc << nb++)) == 0);
	*bpc = 32 - nb;
}

void lzw_add_to_cdict(string& cdict, int& cdict_as, int new_index, int idono, string buff, int& bpc)
{
	cdict += buff;
}
void tiff_lzw_decode(uchar* coded, int buffLen, string& dec)
{
	string word = { 0 }, outstring = { 0 };
	size_t coded_pos;   // position in bits
	int i, new_index, code, maxcode, bpc;

	string cdict = { 0 };
	wstring dictionaryIndex = { 0 };

	bpc = 9;            // starts with 9 bits per code, increases later
	tiff_lzw_calc_maxcode(bpc, &maxcode);
	new_index = 258;        // index at which new cdict entries begin
	coded_pos = 0;          // bit position

	lzw_dict_init(cdict, dictionaryIndex);
	const int CLEAR_CODE = 256;
	while ((code = get_bits_in_stream(coded, buffLen, coded_pos, bpc)) != 257)   // while ((Code = GetNextCode()) != EoiCode)
	{
		coded_pos += bpc;

		if (code >= new_index)
			printf("Out of range code %d (new_index %d)\n", code, new_index);

		if (code == 256)                        // if (Code == ClearCode)
		{
			lzw_dict_init(cdict, dictionaryIndex);             // InitializeTable();
			bpc = 9;
			tiff_lzw_calc_maxcode(bpc, &maxcode);
			new_index = 258;

			while (code == CLEAR_CODE)
			{
				code = get_bits_in_stream(coded, buffLen, coded_pos, bpc);   // Code = GetNextCode();
				coded_pos += bpc;
				if (code == 257)                    // if (Code == EoiCode)
					break;
			}
			if (code == 257)                    // if (Code == EoiCode)
				break;

			// revece?
			dec += cdict[code];               // WriteString(StringFromCode(Code));

			word = cdict[code];             // OldCode = Code;
		}
		else if (code > 4096)
			throw exception("corrupted code at scanline ");


		if (cdict.length() > code)                 // if (IsInTable(Code))
		{
			dec += cdict[code];           // WriteString(StringFromCode(Code));

			lzw_add_to_cdict(cdict, i, new_index, 0, word, bpc);
			lzw_add_to_cdict(cdict, i, new_index, 1, "" + cdict[code], bpc);    // AddStringToTable
			new_index++;
			tiff_lzw_calc_bpc(new_index, &bpc, &maxcode);

			word = cdict[code];         // OldCode = Code;
		}
		else
		{
			outstring = word + word[0];      // OutString = StringFromCode(OldCode) + FirstChar(StringFromCode(OldCode));

			dec += outstring;            // WriteString(OutString);

			lzw_add_to_cdict(cdict, i, new_index, 0, outstring, bpc); // AddStringToTable
			new_index++;
			tiff_lzw_calc_bpc(new_index, &bpc, &maxcode);

			word = code;         // OldCode = Code;
		}

	}

	word.clear();
	outstring.clear();
	cdict.clear();
}
*/



void TiffReader::printValue(int x, int y)
{

	// get Tile (contans offsects for each row
	//Count		N = StripsPerImage for PlanarConfiguration equal to 1; N = SamplesPerPixel * StripsPerImage for PlanarConfiguration equal to 2
//	Default		None

	uchar buffer[4];
	read(buffer, tiff.StripOffsets + y * 4, 4);
	int off = toInt(buffer);

	read(buffer, tiff.StripByteCounts + y * 4, 4);
	int count = toInt(buffer);

	uchar* buff = new uchar[count];
	read(buff, off, count);

	string st = "";// dectode(buff, count);
	vector<uchar> ret;
	decorder decod;
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
//
//static int
//DGifDecompressLine(uchar* Line, int LineLen)
//{
//	int i = 0;
//	int j, CrntCode, EOFCode, ClearCode, CrntPrefix, LastCode, StackPtr;
//	GifByteType* Stack, * Suffix;
//	GifPrefixType* Prefix;
//	GifFilePrivateType* Private = (GifFilePrivateType*)GifFile->Private;
//
//	StackPtr = Private->StackPtr;
//	Prefix = Private->Prefix;
//	Suffix = Private->Suffix;
//	Stack = Private->Stack;
//	EOFCode = Private->EOFCode;
//	ClearCode = Private->ClearCode;
//	LastCode = Private->LastCode;
//
//	if (StackPtr > LZ_MAX_CODE) {
//		return GIF_ERROR;
//	}
//
//	if (StackPtr != 0) {
//		/* Let pop the stack off before continueing to read the GIF file: */
//		while (StackPtr != 0 && i < LineLen)
//			Line[i++] = Stack[--StackPtr];
//	}
//
//	while (i < LineLen) {    /* Decode LineLen items. */
//		if (DGifDecompressInput(GifFile, &CrntCode) == GIF_ERROR)
//			return GIF_ERROR;
//
//		if (CrntCode == EOFCode) {
//			/* Note however that usually we will not be here as we will stop
//			 * decoding as soon as we got all the pixel, or EOF code will
//			 * not be read at all, and DGifGetLine/Pixel clean everything.  */
//			GifFile->Error = D_GIF_ERR_EOF_TOO_SOON;
//			return GIF_ERROR;
//		}
//		else if (CrntCode == ClearCode) {
//			/* We need to start over again: */
//			for (j = 0; j <= LZ_MAX_CODE; j++)
//				Prefix[j] = NO_SUCH_CODE;
//			Private->RunningCode = Private->EOFCode + 1;
//			Private->RunningBits = Private->BitsPerPixel + 1;
//			Private->MaxCode1 = 1 << Private->RunningBits;
//			LastCode = Private->LastCode = NO_SUCH_CODE;
//		}
//		else {
//			/* Its regular code - if in pixel range simply add it to output
//			 * stream, otherwise trace to codes linked list until the prefix
//			 * is in pixel range: */
//			if (CrntCode < ClearCode) {
//				/* This is simple - its pixel scalar, so add it to output: */
//				Line[i++] = CrntCode;
//			}
//			else {
//				/* Its a code to needed to be traced: trace the linked list
//				 * until the prefix is a pixel, while pushing the suffix
//				 * pixels on our stack. If we done, pop the stack in reverse
//				 * (thats what stack is good for!) order to output.  */
//				if (Prefix[CrntCode] == NO_SUCH_CODE) {
//					/* Only allowed if CrntCode is exactly the running code:
//					 * In that case CrntCode = XXXCode, CrntCode or the
//					 * prefix code is last code and the suffix char is
//					 * exactly the prefix of last code! */
//					if (CrntCode == Private->RunningCode - 2) {
//						CrntPrefix = LastCode;
//						Suffix[Private->RunningCode - 2] =
//							Stack[StackPtr++] = DGifGetPrefixChar(Prefix,
//								LastCode,
//								ClearCode);
//					}
//					else {
//						GifFile->Error = D_GIF_ERR_IMAGE_DEFECT;
//						return GIF_ERROR;
//					}
//				}
//				else
//					CrntPrefix = CrntCode;
//
//				/* Now (if image is O.K.) we should not get a NO_SUCH_CODE
//				 * during the trace. As we might loop forever, in case of
//				 * defective image, we use StackPtr as loop counter and stop
//				 * before overflowing Stack[]. */
//				while (StackPtr < LZ_MAX_CODE &&
//					CrntPrefix > ClearCode && CrntPrefix <= LZ_MAX_CODE) {
//					Stack[StackPtr++] = Suffix[CrntPrefix];
//					CrntPrefix = Prefix[CrntPrefix];
//				}
//				if (StackPtr >= LZ_MAX_CODE || CrntPrefix > LZ_MAX_CODE) {
//					GifFile->Error = D_GIF_ERR_IMAGE_DEFECT;
//					return GIF_ERROR;
//				}
//				/* Push the last character on stack: */
//				Stack[StackPtr++] = CrntPrefix;
//
//				/* Now lets pop all the stack into output: */
//				while (StackPtr != 0 && i < LineLen)
//					Line[i++] = Stack[--StackPtr];
//			}
//			if (LastCode != NO_SUCH_CODE) {
//				Prefix[Private->RunningCode - 2] = LastCode;
//
//				if (CrntCode == Private->RunningCode - 2) {
//					/* Only allowed if CrntCode is exactly the running code:
//					 * In that case CrntCode = XXXCode, CrntCode or the
//					 * prefix code is last code and the suffix char is
//					 * exactly the prefix of last code! */
//					Suffix[Private->RunningCode - 2] =
//						DGifGetPrefixChar(Prefix, LastCode, ClearCode);
//				}
//				else {
//					Suffix[Private->RunningCode - 2] =
//						DGifGetPrefixChar(Prefix, CrntCode, ClearCode);
//				}
//			}
//			LastCode = CrntCode;
//		}
//	}
//
//	Private->LastCode = LastCode;
//	Private->StackPtr = StackPtr;
//
//	return GIF_OK;
//}

bool TiffReader::open(const char* path)
{
	uchar buffer[16];

	fopen_s(&pFile, path, "rb");
	if (pFile == NULL)
	{
		perror("Error opening file");
		return false;
	}
	else
	{
		read(buffer, 0, 8);

		//feof(pFile))
		// fputs(buffer, stdout); !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//		if (buffer[0] == 'l' && buffer[1] == 'l')
		imgByteOrder = buffer[0];
		printHeader(buffer);
		uint idfOffset = toInt(buffer + 4);
		printIFD(idfOffset);

		//		fseek(pFile, idfOffset, SEEK_SET);

		//		fgets((char *) buffer, 2, pFile);


//		this->printValue(0, 0);
//		this->printValue(3500, 6000);
		//		close();
	}
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
}
/*

*/

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

/*
#include <string>
#include <map>

// Compress a string to a list of output symbols.
// The result will be written to the output iterator
// starting at "result"; the final iterator is returned.
template <typename Iterator>
Iterator compress(const std::string &uncompressed, Iterator result) {
  // Build the dictionary.
  int dictSize = 256;
  std::map<std::string,int> dictionary;
  for (int i = 0; i < 256; i++)
	dictionary[std::string(1, i)] = i;

  std::string w;
  for (std::string::const_iterator it = uncompressed.begin();
	   it != uncompressed.end(); ++it) {
	char c = *it;
	std::string wc = w + c;
	if (dictionary.count(wc))
	  w = wc;
	else {
	  *result++ = dictionary[w];
	  // Add wc to the dictionary.
	  dictionary[wc] = dictSize++;
	  w = std::string(1, c);
	}
  }

  // Output the code for w.
  if (!w.empty())
	*result++ = dictionary[w];
  return result;
}

// Decompress a list of output ks to a string.
// "begin" and "end" must form a valid range of ints
template <typename Iterator>
std::string decompress(Iterator begin, Iterator end) {
  // Build the dictionary.
  int dictSize = 256;
  std::map<int,std::string> dictionary;
  for (int i = 0; i < 256; i++)
	dictionary[i] = std::string(1, i);

  std::string w(1, *begin++);
  std::string result = w;
  std::string entry;
  for ( ; begin != end; begin++) {
	int k = *begin;
	if (dictionary.count(k))
	  entry = dictionary[k];
	else if (k == dictSize)
	  entry = w + w[0];
	else
	  throw "Bad compressed k";

	result += entry;

	// Add w+entry[0] to the dictionary.
	dictionary[dictSize++] = w + entry[0];

	w = entry;
  }
  return result;
}

#include <iostream>
#include <iterator>
#include <vector>

int main() {
  std::vector<int> compressed;
  compress("TOBEORNOTTOBEORTOBEORNOT", std::back_inserter(compressed));
  copy(compressed.begin(), compressed.end(), std::ostream_iterator<int>(std::cout, ", "));
  std::cout << std::endl;
  std::string decompressed = decompress(compressed.begin(), compressed.end());
  std::cout << decompressed << std::endl;

  return 0;
}*/
