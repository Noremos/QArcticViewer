#ifndef BASE_H
#define BASE_H
#define LOG
#include <vector>


#define USE_OPENCV
typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned short ushort;

typedef std::vector<uchar> buffer;

//typedef unsigned long long offu64;
typedef unsigned int offu64;

typedef unsigned int objoff;


namespace
{
#ifdef _WIN32
#define nl "\r\n"
#elif defined macintosh // OS 9
#define nl = "\r"
#elif
#define nl = "\n" // Mac OS X uses \n
#endif
}

#endif // BASE_H
