#include "imagesearcher.h"

#include <opencv2/opencv.hpp>
#include "opencv2/core/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#ifdef USE_OPENCV
using namespace cv;
#endif


float ImageSearcher::findHistAVG(Img tile)
{
	map<float, int> hist;
	return 0;
}
float ImageSearcher::findSumdivAVG(Img tile)
{
	float sum = 0;
	for (int var = 0; var < tile.getTotal(); ++var)
	{
		float val = tile.data[var];
		if (val != -9999)
			sum += abs(tile.data[var]);
	}
	return sum / tile.getTotal();
}

RetImg ImageSearcher::process(Img tile, AvgType avgType, ProcessType funType, uchar porog)
{
	float avg;
	switch (avgType) {
	case AvgType::Hist:
		avg =  findHistAVG(tile);
		break;
	case AvgType::AvgNsum:
		avg = findSumdivAVG(tile);
		break;
	case AvgType::MaxMinusMin:
		avg = reader->max - reader->min;
		break;
	case AvgType::Manuly:
	default:
		break;
	}
		qDebug() << avg;
#ifdef USE_OPENCV
	Mat newImg(tile.hei, tile.wid, CV_8UC1);
#else
	Img newImg(tile.hei, tile.wid, new uc;
#endif
	switch (funType)
	{
	case ProcessType::AvgSdvig:
		for (int j = 2; j < tile.hei - 2; ++j) //hei
		{
			for (int i = 2; i < tile.wid - 2; ++i) //wid
			{
				auto p = abs(tile.get(i, j));
				//p = abs(p-imax);
				if (p > avg)
					p = p - avg;
				else
					p = avg - p;

#ifdef USE_OPENCV
				newImg.at<uchar>(j, i) = (p < porog) ? 0 : 255;
#else
				newImg.set(i, j, (p < porog) ? 0 : 255);
#endif
			}
//#ifdef USE_OPENCV
//			newImg.at<uchar>(j, 0) = 0;
//			newImg.at<uchar>(j,1) = 0;
//			newImg.at<uchar>(j, tile.wid - 2) = 0;
//			newImg.at<uchar>(j, tile.wid - 1) = 0;
//#else
//			newImg.set(0, j, 0);
//			newImg.set(1, j, 0);
//			newImg.set(wid - 2, j, 0);
//			newImg.set(wid - 1, j, 0);
//#endif
		}
//#ifndef USE_OPENCV
//		//zeroing first 2 rows
//		memset(gray.data, 0, 2 * wid * sizeof(float));
//		//and last 2 rows
//		memset(gray.data + wid * (hei - 2), 0, 2 * wid * sizeof(float));
//#else
//		for (int i = 0; i < tile.wid; ++i)
//		{
//			newImg.at<uchar>(0, i) = 0;
//			newImg.at<uchar>(1, i) = 0;
//			newImg.at<uchar>(tile.hei - 2, i) = 0;
//			newImg.at<uchar>(tile.hei - 1, i) = 0;
//		}
//#endif

		break;

	case ProcessType::NaburPorog:

#ifdef USE_OPENCV

#else
		newImg = gray.zeroClone();

#endif
		for (int j = 1; j < tile.hei; ++j) //hei
		{
			for (int i = 1; i < tile.wid; ++i) //wid
			{
				float p = tile.get(i, j);
				float pm1 = tile.get(i, j - 1);
				float pm2 = tile.get(i - 1, j);
				//p = abs(p-imax);
#ifndef USE_OPENCV

				if (abs(p - pm1) > porog || abs(p - pm2) > porog)
					newImg.set(i, j, 255);
				else
					newImg.set(i, j, 0);
#else
				if (abs(p - pm1) > porog || abs(p - pm2) > porog)
					newImg.at<uchar>(j, i) = 255;
				else
					newImg.at<uchar>(j, i) = 0;
#endif
			}
		}
		break;
	}
	return newImg;
}

void scanRow(const Img &img, int stX, int row, boundy &d)
{
	Img zeros = img.clone();
	int minX = stX, maxX = stX;
	while (minX >= 0 && zeros.get(minX, row) == 255)
	{
		zeros.set(minX, row, 0);
		--minX;
	};
	++minX;
	d.setXifLess(minX);

	while (maxX < img.wid - 1 && img.get(maxX, row) == 255)
	{
		zeros.set(minX, row, 0);
		++minX;
	}
	--maxX;
	d.setEndXifMore(maxX);

	++row;
	d.setEndYifMore(row);

	bool has0 = false;
	for (int i = minX; i <= maxX; ++i)
	{
		if (img.get(i, row) == 255)
		{
			if (!has0)
			{
				scanRow(img, i, row, d);
				has0 = false;
			}
		}
		else
			has0 = true;
	}
}

void ImageSearcher::binFindObjects(const Img &img, vector<boundy>& objects)
{
	for (int j = 0; j < img.hei; ++j)
	{
		for (int i = 0; i < img.wid - 1; ++i)
		{
			if (img.get(i, j) == 0 && img.get(i + 1, j) == 255)
			{
				boundy b(i + 1, j, 1, 1);
				scanRow(img, i + 1, j, b);
				objects.push_back(b);
			}
		}
	}
}

ImageSearcher::ImageSearcher(TiffReader *reader): reader(reader)
{

}

constexpr int getTid(int tx, int ty, int wid)
{
	return ty* wid + tx;
}

//Img ImageSearcher::getTile(int tx, int ty)
Img ImageSearcher::getTile(int index)
{
	int tx = index % tilesInWid;
	int ty = index / tilesInWid;

	if (tx<0 || tx>=tilesInWid || ty<0 || ty>= tilesInHei)
		return nullptr;

	if (reader->tiff.TileWidth != 0)
	{
		char offs[4][2] = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
		Img img(tileWid + diffset, tileHei + diffset);


		for (int i = 0; i < 4; ++i)
		{
			int tcurX = tx + offs[i][0];
			int tcurY = ty + offs[i][1];
			if (tcurX < 0 || tcurY < 0 || tcurX >= tilesInWid || tcurY >= tilesInHei)
				continue;

			// in img
			int istX = 0, istY = 0;

			int lenX = tileWid,
				lenY = tileHei;

			if (offs[i][0] == 1)
			{
				istX = tileWid;
				lenX = diffset;
			}
			if (offs[i][1] == 1)
			{
				istY = tileHei;
				lenX = diffset;
			}

			for (int i = 0; i < tileWid * tileHei; ++i)
				img.data[i] = -9999;


			uchar *tile = reader->getTile(ty * tilesInWid + tx);

			float *data = static_cast<float *>(reader->processData(tile));

			for (int j = 0; j < lenY; ++j)
			{
				int fromdataOff = j * tileWid;
				img.setInRow(istY + j, istX, data + fromdataOff, lenX);
			}
		}
//load 9
// * * *
// * * *
// * * *
		return img;
//		reader->getRowData()
	}

	Img img(tileWid + diffset, tileHei + diffset);
	for (int i = 0; i < tileWid * tileHei; ++i)
		img.data[i] = -9999;

	// Pixel    "st" is a column number is source image
	// *  *  *
	// *  st *
	// *  *  *
	int st = tileWid * tx;
	int len = (tx == tilesInWid - 1 ? tileWid : tileWid + diffset);

	// Pixel  "i" is a row number in source image
	// * * *
	// i * *
	// * * *
	for (int i = ty * tileHei, total = min((ty + 1) * tileHei + diffset, reader->height()), k = 0; i < total; ++i, ++k)
	{
		img.setInRow(k, 0, static_cast<float*>(reader->getRow(i)) + st, len);
	}
	return img;
}

int ImageSearcher::getMaxTiles()
{
	return reader->widght() * reader->height() / (tileWid * tileHei);
}

void ImageSearcher::findZones(vector<boundy> &bounds)
{
	if (reader->tiff.TileWidth != 0)
	{
		tileWid = reader->tiff.TileWidth;
		tileHei = reader->tiff.TileLength;
		reader->setTitleCacheSize(9);
	}
	else
	{
		tileWid = 1500;
		tileHei = 1500;
		reader->setRowsCacheSize(tileHei);
	}
	tilesInWid = reader->widght() / tileWid;
	tilesInHei = reader->height() / tileHei;

	image = new float[(tileWid + diffset) * (tileHei + diffset)];

	for (int i = 0, totalTiles = 10 + 0 *tilesInWid * tilesInHei; i < totalTiles; ++i)
	{
		Img img = getTile(i);
//		int add = bounds.size();
		Mat ret = process(img, AvgType::AvgNsum, ProcessType::AvgSdvig, 5);
		cv::imshow("ret", ret);
		cv::waitKey(1);

		vector<vector<Point>> conturs;
		vector<cv::Vec4i> hierarchy;
		findContours(ret, conturs, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE);
		//binFindObjects(img, bounds);

		int totalX = tileWid * (i % tilesInWid);
		int totalY = tileHei * (i / tilesInWid);
		for (int i = 0, total = conturs.size(); i < total; ++i)
		{
			Rect r = boundingRect(conturs[i]);
			boundy b(r.x, r.y, r.x + r.width, r.y + r.height);
			float m0 = img.get(r.x, r.y);
			float m1 = img.get(r.x + r.width - 1, r.y);
			float m2 = img.get(r.x, r.y + r.height - 1);
			float m3 = img.get(r.x + r.width - 1, r.y + r.height - 1);

			if (m1 > m0)
				m0 = m1;
			if (m2 > m0)
				m0 = m1;
			if (m3 > m0)
				m0 = m1;

			b.setMax(m0);
			b.addXoffset(totalX);
			b.addYoffset(totalY);
			bounds.push_back(b);
		}
		qDebug() << "Processed tiles: " << i<< "/" << totalTiles;
	}
//	for (int var = 0; var < total; ++var) {

//	}

}
