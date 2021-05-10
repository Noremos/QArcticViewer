#ifndef OBJECT3D_H
#define OBJECT3D_H

#include <QString>
#include <QVector>

#include <src/base.h>

struct vertex
{
	vertex(float _x=0, float _y=0, float _z=0) : x(_x),y(_y),z(_z),texX(_x),texY(_y)
	{}

	float x, y, z;
	float texX, texY;
};
struct face
{
	unsigned int v1, v2, v3;
	face(unsigned int _v1=0, int _v2=0, int _v3=0) : v1(_v1),v2(_v2),v3(_v3)
	{

	}
};

class Object3d
{
public:
	Object3d();
	QVector<vertex> vetexes;
	QVector<face> faces;

	void readFile(QString path);
	void readFastfile(const PrjgBarCallback &pbCallback, QString path);
};

#endif // OBJECT3D_H
