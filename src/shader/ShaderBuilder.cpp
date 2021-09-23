#include "shaderBuilder.h"


struct replJob
{
	QString holder;//Что заменить
	QString text;// На что заменить
};

QString& strip(QString& imput, QString& beg)
{
	for (int i = 0; i < imput.size(); ++i)
	{
		uchar c = imput.at(i).cell();
		if (c == ' ' || c == '\t')
			beg.append(c);
		else
		{
			imput = imput.mid(i);
			break;
		}
	}

	return imput;
}



ShaderBuilder::~ShaderBuilder()
{
}

QString ShaderBuilder::processFile(QString path)
{
	Uparser parser;

	QFile out(path);

	if (!out.open(QFile::ReadOnly))
	{
		qDebug() << "Unable to onpen file " << path;
		return "";
	}

	QString begin;
	// if (ver == gl3_3
	begin.append("#version 330 core" nl);

//	QString main;
	QString line;
	// begin.writeLine("}");// main() {



	//    QTextStream res;
	//    begin.writeLine("@BEG");// main() {
	//    begin.writeLine("@MAIN");// main() {
	//    begin.writeLine("@END");// main() {

//	QString imports;
	QTextStream stream(&out);
	QString& curStream = begin;
	while (stream.readLineInto(&line))
	{
//		if (line.contains("void main"))
//		{
//			stream.readLineInto(&line); //skip {
//			main.append(line);

//			stream.readLineInto(&line);
//			main.append(line);
//			//			curStream = main;
//			//			main(imports)
//			break;
//		}

		QString beg;
		QString srcLine = line;
		strip(line, beg);
		if (line.startsWith("//"))
			continue;

		QString srcLine2 = line;
		line = line.left(line.indexOf(' '));
		if (defMap.contains(line))
		{
			curStream.append(defMap[line].preprocess(srcLine2, beg));
		}
		else
		{
			curStream.append(srcLine + nl);
		}
	}

	return curStream;
	//    while (stream.readLineInto(&line))
	//    {
	//        if (line.contains('}'))//&& curStream == main)
	//        {
	//            curStream.append(ends);
	//        }
	//        QString beg;

	//    }
	//-------------------------------------------

	//    parser.addSkipWordToken('(', ')');
	//    parser.addSkipWordToken('{', '}');

	//    parser.addWordToken("uniform");
	//    parser.addWordToken("import");
	//    parser.addWordToken("#version");
	//    parser.addSingleToken(';');

	//    parser.addWordToken("main");
	//    parser.addWordToken("void");

	//    parser.addGetStatementSignature(, "void main()");
	//    parser.addGetStatementSignature(, "uniform r_MPV;");
	//    parser.addSkipWordToken('\'', '\'', 0, true);
	//    parser.addGetStatementSignature(, "+import .@1 .@2 \\[putToEnd 'out_{@2} = @2;'\\]");
	//    parser.addGetStatementSignature(, "+import .@1 .@2 \\[initOut 'out_{@2};'\\]");

	//    QTextStream stream(&out);
	//    parser.processText(stream.readAllText());
}
unique_ptr<ShaderBuilder> ShaderBuilder::shaderBuilder = nullptr;

ShaderBuilder::ShaderBuilder(glVersion ver)
{
	QFile out(":/shaders/replaces.data");


	if (!out.open(QFile::ReadOnly))
	{
		return;
	}

	QString line;
	DefTempl def;
	QTextStream stream(&out);
	bool skipRead = false;
	while (true)
	{
		if (!skipRead)
		{
			if (!stream.readLineInto(&line))
			{
				if (def.templname.size() != 0)
				{
					QString deftempl = def.finalize();
					defMap[deftempl] = def;
					def.templname.clear();
					def.desc.clear();
					//				defens.append(def);
				}
				stream.readLineInto(&def.templname);
				break;
			}
		}
		else
		{
			skipRead = false;
		}

		char fst  = line[0].cell();
		switch (fst)
		{
		case '@':
			if (def.templname.size() != 0)
			{
				QString deftempl = def.finalize();
				defMap[deftempl] = def;
				def.templname.clear();
				def.desc.clear();
				//				defens.append(def);
			}
			stream.readLineInto(&def.templname);
			break;
		case '#':
			if (ver == glVersion::opngl3_3 && line[1] != 'd')
				continue;
			if (ver == glVersion::opngl_es && line[1] != 'm')
				continue;

			while (stream.readLineInto(&line) && line[0] != '#' && line[0] != '@')
			{
				def.desc.append(line + nl);
			}
			skipRead = true;
			break;
		}

	}

	return;
}
