#ifndef SUBCLASSES_H
#define SUBCLASSES_H


#include "../base.h"
#include "tiffreader.h"

#include <QFile>
#include <QObject>
#include <vector>

#include <src/types/types.h>
#include <src/types/img.h>

#include "fstream"


struct Size2
{
	int wid, hei;
	Size2(int _wid, int _hei)
	{
		wid = _wid;
		hei = _hei;
	}
};

#include <QDir>
struct FileBuffer
{
private:
	QString buffer;
	QFile outfile;
	QTextStream *stream = nullptr;
	int maxBufferSize = 10000;
public:
	bool openFileStream(QString path, int maxBufferSize = 10000, QFile::OpenMode openFlags = QFile::WriteOnly | QFile::Truncate)
	{
		outfile.setFileName(path);

		//		if (outfile.exists())
		//		{
		//			QFile::rename(path,  path.replace("bds.lst", "bds old.lst"));
		//			//			outfile.setFileName(path);
		//		}

		if (!outfile.open(openFlags))
			return false;

		if (stream != nullptr)
			delete stream;
		stream = new QTextStream(&outfile);
		this->maxBufferSize = maxBufferSize;

		return true;
	}

	void write(const QString &part)
	{
		if (stream == nullptr)
			return;

		buffer.append(part);
		if (buffer.size() > maxBufferSize)
		{
			dowrite();
		}
	}

	void writeLine(const QString &part = "")
	{
		if (stream == nullptr)
			return;

		buffer.append(part);
		buffer.append(nl);
		if (buffer.size() > maxBufferSize)
		{
			dowrite();
		}
	}

	void dowrite()
	{
		stream->operator<<(buffer);
		buffer.clear();
	}
	void close()
	{
		if (stream != nullptr && outfile.isOpen())
		{
			dowrite();
			outfile.close();
			delete stream;
			stream = nullptr;
		}
	}
	~FileBuffer()
	{
		close();
	}
};


#endif // SUBCLASSES_H
