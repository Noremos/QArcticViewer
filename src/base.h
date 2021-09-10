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
private:
	T* m_buffer = nullptr;
	uint m_size = 0;


public:
	staticArray() {}
	staticArray(const staticArray &other) /*: s(other.s)*/
	{
		allocate(other.m_size);
		std::copy(other.m_buffer, other.m_buffer + other.m_size, m_buffer);
	}
 /*std::cout << "move failed!\n";*/
	 staticArray(staticArray&& other) /*: s(std::move(o.s))*/
	 {
		 m_buffer = std::exchange(other.m_buffer, nullptr); // leave other in valid state
		 m_size = std::exchange(other.m_size, 0);
	 }

	T *data()
	{
		return m_buffer;
	}

	T* extract()
	{
		T* temp = m_buffer;
		m_size = 0;

		m_buffer = nullptr;
		return temp;
	}

	void allocate(uint nsize)
	{
//		assert(size != 0);

		release();
		this->m_size = nsize;
		m_buffer = new T[nsize];
	}

	uint size() { return m_size; }

	void setData(T *newData, size_t size)
	{
		release();
		m_size = size;
		m_buffer = newData;
	}

	void setToZero()
	{
		memset(m_buffer, 0, m_size * sizeof(T));
	}

	T& operator[](std::size_t idx)
	{
		assert(idx < m_size);
		return m_buffer[idx];
	}

	// copy assignment
	staticArray &operator=(const staticArray &other)
	{
		// Guard self assignment
		if (this == &other)
			return *this;

		allocate(other.m_size);

		std::copy(other.m_buffer, other.m_buffer + other.m_size, m_buffer);
		return *this;
	}

	// move assignment
	staticArray& operator=(staticArray&& other) noexcept
	{
		// Guard self assignment
		if (this == &other)
			return *this; // delete[]/size=0 would also be ok

		m_buffer = std::exchange(other.m_buffer, nullptr); // leave other in valid state
		m_size = std::exchange(other.m_size, 0);
		return *this;
	}

	void release()
	{
		if (m_buffer)
		{
			delete[] m_buffer;
			m_buffer = nullptr;
		}
		m_size = 0;
	}

	~staticArray()
	{
		release();
	}
};

using vbuffer = staticArray<uchar>;

#endif // BASE_H
