#include "../base.h"
#include <QFile>
#include <QMap>
#include <QString>
#include <QTextStream>
#include <QVector>

#include "parser.h"

enum class glVersion
{
    opngl3_3,
    opngl4_2,
    opngl_es
};

class ShaderBuilder
{
private:
    /* data */
public:
    ShaderBuilder(glVersion ver/* args */);
    ~ShaderBuilder();
    
    
    void loadShader(QString& vert, QString& frag);
    
    QString getVertex();
    QString getFragment();
    
private:
    char* preprocess(char* src);
};

ShaderBuilder::ShaderBuilder(glVersion ver)
{
}

ShaderBuilder::~ShaderBuilder()
{
}
struct replJob
{
    QString holder;//Что заменить
    QString text;// На что заменить
};

void processFile(QString path)
{
    Uparser parser;

    QFile out(path);
    if (out.exists())
        out.remove();

    if (!out.open(QFile::WriteOnly | QFile::Truncate))
        return;

    parser.addSkipWordToken('(', ')');

    parser.addWordToken("uniform");
    parser.addWordToken("import");
    parser.addWordToken("#version");
    parser.addSingleToken(';');


    QTextStream stream(&out);
}

/*
#version 330 core
layout (location = 0) in vec3 aPos;
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
    //    gl_Position = vec4(aPos + offset, 0.0, 1.0);
    // coords of char location


//    int index = int(round(instanceMatrix[3][3]));
//    float xstart = float(index%8) / 63.0;
//    float ystart = float(index/8) / 63.0;

//    // offset/ tex.x max is 1. So 1/8 = max char len
//    xstart += a_texcoord.x/8;
//    ystart += a_texcoord.y/8;

    mat4 rightMatrix = instanceMatrix;
//    rightMatrix[3][3] = 1.0;

    gl_Position = projection * view * rightMatrix  * vec4(aPos, 1.0);

    //    v_texcoord = vec2(xstart, ystart);
    v_texcoord = a_texcoord;
}


 */

void ShaderBuilder::loadShader(QString &vertPath, QString &frag)
{

    QMap<QString, QString> simpleTemplate;
    QVector<replJob> repljobs;
    QVector<replJob> insertAtEndJobs;
    QVector<replJob> insertAtFileBeginJobs;
    QVector<replJob> insertAtMainBeginJobs;
    QVector<replJob> insertFuncsJobs;

    parser.processLine()




    QString funcClccl = "";
    /*
     mat4 rmat = model;
    vec3 vp = a_position;
    v_pos = vp;
//    vp.y *= factor;
//    rmat[1][1] *= factor;
    vp.y = minHei + (vp.y - minHei) * factor;
    gl_Position = projection * view * rmat  * vec4(vp, 1.0);
     */
}
