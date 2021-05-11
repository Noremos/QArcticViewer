#include "object3d.h"
#include "object3d.h"

#include <QFile>
#include <QDebug>


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
//#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include <sstream>
#include <charconv>
#include "side-src/fast_float/fast_float.h"


Object3d::Object3d()
{

}
int parceIntFast(const char *num, int len)
{
	int ret = 0;
	std::from_chars(num, num + len, ret);
	return ret;

	int sum = 0;
	for (int i = 0; i < len; i++)
	{
		sum = (sum * 10) + (num[i] - '0');
	}
	return sum;
}

//static bool getLineFast(std::istream &is, std::string &t)
//{
//	t.clear();

//	std::istream::sentry se(is, true);
//	std::streambuf *sb = is.rdbuf();

//	while (true)
//	{
//		int c = sb->sbumpc();
//		if (c == EOF)
//			return false;
//		if (c == '\n' || c == '\r')
//		{
//			c = sb->sbumpc();
//			if (c == '\n')
//				sb->sbumpc();
//			else if (c == EOF)
//				return false;
//			else
//				sb->sungetc();
//			return true;
//		}
//		t += c;
//	}
//	return true;
//}


inline bool isNotEnd(int c)
{
	return (c != ' ' && c != '\n' && c != '\r');
}

int getWord(const char *base)
{
	const char *ref = base;
	int len = 0;
	while (isNotEnd(*ref))
	{
		++ref;
		++len;
	}
	return len;
}

void parceFace(const char *str, unsigned int &f)
{
	const char *razd = str;
	//	int len = 0;
	while (*razd != '/')
	{
		++razd;
		//		++len;
	}
	std::from_chars(str, razd, f);
	--f;
	//	f = parceIntFast(str, len);
}

#include <filesystem>

void Object3d::readFastfile(const PrjgBarCallback &pbCallback, QString path)
{
	QFile fin(path);
	if (!fin.open(QFile::OpenModeFlag::ReadOnly))
	{
		qDebug() << "File " << path << " not found!";
		return;
	}
	size_t lines = 0;

	faces.clear();
	vetexes.clear();

	std::stringstream errss;
	std::string name;

	char rawtoken[500];
	const char *token;
	size_t readed = 0;
	float min = 9999;
	const size_t ADD_STEP = 10;
	const size_t BYTES_COUNT =  static_cast<size_t>(fin.size()) / ADD_STEP;
	pbCallback.cbSetMax(100);
	while (!fin.atEnd())
	{
		if (readed  >= BYTES_COUNT)
		{
			pbCallback.cbIncrValue(ADD_STEP);
			if (pbCallback.stopAction)
				break;
			readed = 0;
		}
		int size = fin.readLine(rawtoken, 500);
		readed += size;
		token = rawtoken;
		lines++;

		// Skip if empty line.
		if (size==0)
			continue;

		int len;

		if (token[0] == 'v')
		{
			vertex v;

			token += 2;
			len = getWord(token);
			fast_float::from_chars(token, token + len, v.x);
			token += len + 1;

			len = getWord(token);
			fast_float::from_chars(token, token + len, v.y);
			token += len + 1;

			if (v.y < min)
				min = v.y;

			len = getWord(token);
			fast_float::from_chars(token, token + len, v.z);

			readed += fin.readLine(rawtoken, 500);
			token = rawtoken;

			lines++;

			// vt
			assert(token[0] == 'v' && token[1] == 't');

			token += 3;
			len = getWord(token);

			fast_float::from_chars(token, token + len, v.texX);
			int end = size - 3;
			fast_float::from_chars(token + len + 1, token + end, v.texY);


			// token += len + 1;
			// int end = curline.length() - len - 4;
			// fast_float::from_chars(token, token + end, v.texY);

			this->vetexes.push_back(v);

			continue;
		}
		if (token[0] == 'f')
		{
			face fc;

			token += 2;
			len = getWord(token);
			parceFace(token, fc.v1);

			token += len + 1;
			len = getWord(token);
			parceFace(token, fc.v2);

			token += len + 1;
			//			len = getWord(token);
			parceFace(token, fc.v3);

			faces.push_back(fc);
			continue;
		}
	}
	pbCallback.cbIncrValue(ADD_STEP);

	fin.close();
	qDebug() << "Readed " << lines << " lines";
	qDebug() << "Vs: " << vetexes.size();
	qDebug() << "Fs: " << faces.size();
	qDebug() << "Min: " << min;
}


void Object3d::readFile(QString path)
{
	QFile fin(path);
	if (!fin.open(QFile::OpenModeFlag::ReadOnly))
	{
		qDebug() << "File " << path << " not found!";
		return;
	}
	size_t lines = 0;

	faces.clear();
	lvetexes.clear();

	std::stringstream errss;
	std::string name;

	char rawtoken[500];
	const char *token;
	size_t readed = 0;
	while (!fin.atEnd())
	{
		int size = fin.readLine(rawtoken, 500);
		readed += size;
		token = rawtoken;
		lines++;

		// Skip if empty line.
		if (size==0)
			continue;

		int len;

		if (token[0] == 'v')
		{
			lvertex v;

			token += 2;
			len = getWord(token);
			fast_float::from_chars(token, token + len, v.x);
			token += len + 1;

			len = getWord(token);
			fast_float::from_chars(token, token + len, v.y);
			token += len + 1;

			len = getWord(token);
			fast_float::from_chars(token, token + len, v.z);

			this->lvetexes.push_back(v);
			continue;
		}
		if (token[0] == 'f')
		{
			face fc;

			token += 2;

			len = getWord(token);
			parceFace(token, fc.v1);

			token += len + 1;
			len = getWord(token);
			parceFace(token, fc.v2);

			token += len + 1;
			parceFace(token, fc.v3);

			faces.push_back(fc);
			continue;
		}
	}
	fin.close();
	qDebug() << "Readed " << lines << " lines";
	qDebug() << "LVs: " << lvetexes.size();
	qDebug() << "Fs: " << faces.size();

	return;
}
