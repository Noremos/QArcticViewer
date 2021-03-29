#ifndef FACE3D_H
#define FACE3D_H
#include <QObject>
#include <QString>
#include <QVector>
#include "base.h"

class Face3d
{
	QVector<objoff> nums;
public:
    Face3d()
    {

    }
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
	void add(int f) { nums.append(f); }

	static void createVstr(objoff *faces, objoff len, QString &out)
	{
		out = "f ";
		for (objoff i = 0; i < len - 1; ++i)
			out += QString::number(faces[i]) + " ";

		out += QString::number(faces[len - 1]) + nl;
	}
	static void createVstr(objoff *faces, objoff *texts, objoff len, QString &out)
	{
		out = "f ";
		for (objoff i = 0; i < len - 1; ++i)
			out += QString::number(faces[i]) +"/"+  QString::number(texts[i]) + " ";

		out += QString::number(faces[len - 1]) +"/"+  QString::number(texts[len-1]) + nl;
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
