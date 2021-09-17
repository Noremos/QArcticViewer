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
    
    QVector<defs> defens;
    QMap<QString nds, def> defMap;
    
    void loadShader(QString& vert, QString& frag);
    
    QString getVertex();
    QString getFragment();
    
private:
    char* preprocess(char* src);
};

struct defs
{
    QString templace;
    QString mobile;
    QString desc;
    /* data */
};

ShaderBuilder::ShaderBuilder(glVersion ver)
{
    QFile out("replaces.data");
    if (out.exists())
        out.remove();

    if (!out.open(QFile::WriteOnly | QFile::Truncate))
        return;
    
    QString line;
    defs def;
    
    while (stream.readLineInto(&line))
    {
        char fst  = line[0].toStdChar();
        switch (fst)
        {
        case '@':
            if (!def.templace.empty())
                defMap[def.templace] = def;
                defens.append(def);
            /* code */
            stream.readLineInto(&def.template)
            break;
        case '#'
            QString ref& = line[1] == 'd' def.desc : def.mobile;
            while (stream.readLineInto(&line) && line[0] != '#' && line[0] != '@')
            {
                ref.append(line);
            }
            break;

        default:
            break;
        }
    }
}

ShaderBuilder::~ShaderBuilder()
{
}
struct replJob
{
    QString holder;//Что заменить
    QString text;// На что заменить
};
QString& strip(QString& imput, QString& beg)
{
    for (size_t i = 0; i < imput.size(); ++i)
    {
        QChar c = imput[i];
        if (c = ' ' || c = '\t')
            beg.append(c);
        else
        {
            imput = imput.sub(i);
            break;
        }
    }

    return imput;
}
void processFile(QString path)
{
    Uparser parser;

    QFile out(path);
    if (out.exists())
        out.remove();

    if (!out.open(QFile::WriteOnly | QFile::Truncate))
        return;
    
    QTextStream begin;
    begin.writeLine("#verion 330");

    QTextStream main;

    QTextStream end;
    // begin.writeLine("}");// main() {



    QTextStream res;
    begin.writeLine("@BEG");// main() {
    begin.writeLine("@MAIN");// main() {
    begin.writeLine("@END");// main() {
        
    QTextStream stream(&out);
    QTextStream& curStream = begin;
    while (stream.readLineInto(&line))
    {
        curStream.append(line);
        if (line.containt("void main"))
        {
            stream.readLineInto(&line);//skip {
            main.append(line);
                
            stream.readLineInto(&line);
            main.append(line);
            curStream = main;
            main.write(imports)
            break;
        }
        QString beg;
        if (strip(line, beg).startsWith("+import"))
        {
            QStringList list = line.split(' ');
            QString sls = "layout(location = %s) in %s %s;".ags(line[0].toString()).ags(line[1].toString()).ags(line[2].toString())
            imports.append(beg + "sls");
        }
    }

    while (stream.readLineInto(&line))
    {
        if (line.constains('}'))//&& curStream == main)
        {
            curStream.append(ends);
        }
        QString beg;
        if (def.has(strip(line, beg)))
        {
            
        }
    }
    //-------------------------------------------

    parser.addSkipWordToken('(', ')');
    parser.addSkipWordToken('{', '}');

    parser.addWordToken("uniform");
    parser.addWordToken("import");
    parser.addWordToken("#version");
    parser.addSingleToken(';');

    parser.addWordToken("main");
    parser.addWordToken("void");

    parser.addGetStatementSignature(, "void main()");
    parser.addGetStatementSignature(, "uniform r_MPV;");
    parser.addSkipWordToken('\'', '\'', 0, true);
    parser.addGetStatementSignature(, "+import .@1 .@2 \\[putToEnd 'out_{@2} = @2;'\\]");
    parser.addGetStatementSignature(, "+import .@1 .@2 \\[initOut 'out_{@2};'\\]");

    QTextStream stream(&out);
    parser.processText(stream.readAllText());
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

    parser.processText()




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
