#ifndef BASE_H
#define BASE_H
#define LOG
#include <vector>


//#define USE_OPENCV
typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned short ushort;

typedef std::vector<uchar> buffer;

//typedef unsigned long long offu64;
typedef unsigned int offu64;

typedef unsigned int objoff;

//2 метра на пиксель
constexpr const uint resol = 2;
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

#ifndef MAX
#define MAX(A, B) ((A) > (B) ? (A) : (B))
#endif

#ifndef MIN
#define MIN(A, B) ((A) < (B) ? (A) : (B))
#endif

#endif // BASE_H
