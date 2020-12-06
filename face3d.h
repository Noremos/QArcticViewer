#ifndef FACE3D_H
#define FACE3D_H
#include <QObject>
#include <QString>
#include <QVector>

typedef long long objoff;

class Face3d
{
	QVector<objoff> nums;
public:
	Face3d();
	Face3d(objoff *faces, int len) { add(faces, len); }
	void add(objoff* faces, int len)
	{
		for (int i = 0; i < len; ++i)
			nums.append(faces[i]);
	}
	void clear()
	{
		nums.clear();
	}
	void add(int f)
	{
		nums.append(f);
	}

	QString buildStr(objoff offset=0)
	{
		QString st = "f ";
		for(auto v : nums)
			st += QString::number(v + offset) + " ";

		return st.remove(st.length() - 1, 1);
	}

	~Face3d()
	{
		clear();
	}
};

#endif // FACE3D_H
