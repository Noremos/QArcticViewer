#ifndef BASE_H
#define BASE_H
#define LOG
#include <vector>


//#define RELEASE_AV
//#define USE_OPENCV

#define ENABLE_MARKERS
#define ENABLE_SHAPE

typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned short ushort;

//typedef unsigned long long offu64;
typedef unsigned int offu64;

typedef unsigned int objoff;

//2 метра на пиксель
constexpr const int resol = 2;
namespace
{
#ifdef _WIN32
#define nl "\r\n"
#elif defined macintosh // OS 9
#define nl "\r"
#else
#define nl "\n" // linux | Mac OS X uses \n
#endif
}

#ifndef MAX
#define MAX(A, B) ((A) > (B) ? (A) : (B))
#endif

#ifndef MIN
#define MIN(A, B) ((A) < (B) ? (A) : (B))
#endif


#include <assert.h>
#include  <functional>

struct PrjgBarCallback
{
	std::function<void(int)> cbIncrValue;
	std::function<void(int)> cbSetMax;
	volatile bool &stopAction;
	PrjgBarCallback(volatile bool &stopAction):stopAction(stopAction)
	{}
};

#include <string.h>

template<class T>
struct staticArray
{
	T* buffer = nullptr;
	uint _size = 0;

	uchar *data()
	{
		return buffer;
	}

	T* extract()
	{
		T* temp = buffer;
		_size = 0;

		buffer = nullptr;
		return temp;
	}

	void allocate(uint size)
	{
//		assert(size != 0);

		release();
		this->_size = size;
		buffer = new T[size];
	}

	uint size()
	{
		return _size;
	}

	void setToZero()
	{
		memset(buffer, 0, _size * sizeof(T));
	}

	T& operator[](std::size_t idx)
	{
		assert(idx < _size);
		return buffer[idx];
	}

	void release()
	{
		if (buffer)
		{
			delete[] buffer;
			buffer = nullptr;
		}
		_size = 0;
	}

	~staticArray()
	{
		release();
	}
};

using vbuffer = staticArray<uchar>;

#endif // BASE_H
