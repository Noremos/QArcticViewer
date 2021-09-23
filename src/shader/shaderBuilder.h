#include "../base.h"
#include <QFile>
#include <QMap>
#include <QString>
#include <QTextStream>
#include <QVector>

#include "parser.h"
#include <QDebug>
#include <QRegularExpression>

enum class glVersion
{
    opngl3_3,
    opngl4_2,
    opngl_es
};


class ShaderBuilder
{

private:
	struct DefTempl
	{
		QString templname;
		//	QString mobile;
		QString desc;
		QString preprocess(QString imput, QString beg) const
		{
			QStringList st = imput.split(' ');
			QString dsret = beg + desc;
			for (int i = 1; i < st.size(); ++i)
			{
				dsret = dsret.replace("@" + QString::number(i), st[i]);
				//			mobile = mobile.replace("@" + QString::number(i), st[i]);
			}

			QString nbeg = nl + beg;
			dsret = dsret.replace(QRegularExpression(nl), nbeg); // + nl;
			if (dsret.endsWith(nbeg))
			{
				dsret = dsret.remove(dsret.length() - beg.length(), beg.length());
			}
			return dsret;
		}

		QString finalize()
		{
			QStringList st = templname.split(' ');
			return st[0];
		}
		/* data */
	};
    /* data */
public:
	static unique_ptr<ShaderBuilder> shaderBuilder;

	ShaderBuilder(glVersion ver/* args */);
    ~ShaderBuilder();
    
	QVector<DefTempl> defens;
	QMap<QString, DefTempl> defMap;
    
    void loadShader(QString& vert, QString& frag);
    
    // QString getVertex();
	// QString getFragment();

	QString processFile(QString path);
private:
    char* preprocess(char* src);
};


/*
#version 330 core
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec2 a_texcoord;
layout (location = 2) in mat4 instanceMatrix;

//uniform int minHei;
uniform mat4 projection;
uniform mat4 view;

out vec2 v_texcoord;
out vec2 offset;


void main()
{
//    vec2 offset = offsets[gl_InstanceID];
    //    gl_Position = vec4(a_position + offset, 0.0, 1.0);
    // coords of char location


//    int index = int(round(instanceMatrix[3][3]));
//    float xstart = float(index%8) / 63.0;
//    float ystart = float(index/8) / 63.0;

//    // offset/ tex.x max is 1. So 1/8 = max char len
//    xstart += a_texcoord.x/8;
//    ystart += a_texcoord.y/8;

    mat4 rightMatrix = instanceMatrix;
//    rightMatrix[3][3] = 1.0;

    gl_Position = projection * view * rightMatrix  * vec4(a_position, 1.0);

    //    v_texcoord = vec2(xstart, ystart);
    v_texcoord = a_texcoord;
}


 */


