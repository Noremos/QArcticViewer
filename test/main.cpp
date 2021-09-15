//#include <QtTest/QtTest>

#include "../src/core/tiffreader.h"

class TestTiffReader
	//: public QObject
{
//    Q_OBJECT
public:
	void readAllWithCache();
	void checkBadRow();
	void compireDecoders();
	virtual ~TestTiffReader() {};
};

void TestTiffReader::readAllWithCache()
{
	TiffReader tiffr;
	tiffr.open("D:/Learning/BAR/Moscow/50_59_1_2_2m_v3.0/50_59_1_2_2m_v3.0_reg_dem.tif");

	tiffr.setRowsCacheSize(100);
	int l = tiffr.height();
	for (int i = 161; i < tiffr.height(); i++)
	{
		auto ds1 = tiffr.getRow(i);
	}
	//	QCOMPARE(true, true);
}

void TestTiffReader::checkBadRow()
{
	TiffReader tiffr;
	tiffr.open("D:/Learning/BAR/Moscow/50_59_1_2_2m_v3.0/50_59_1_2_2m_v3.0_reg_dem.tif");

	tiffr.setRowsCacheSize(100);
	//	int l = tiffr.height();
	int i = 161;
	//	for (int i = 161; i < tiffr.height(); i++)
	{
		auto ds1 = tiffr.getRow(i);
		for (int k = 1; k < 25010; ++k)
		{
			float kvP = ds1[k - 1];
			float kvC = ds1[k];
			if (abs(kvP - kvC) > 1)
				qDebug() << "Wrn:" << kvC;
		}
	}
	//	QCOMPARE(true, true);
}

void TestTiffReader::compireDecoders()
{
	TiffReader tiffr;
	tiffr.open("D:/Learning/BAR/Moscow/50_59_1_2_2m_v3.0/50_59_1_2_2m_v3.0_reg_dem.tif");

	for (int i = 0; i < tiffr.height(); i++)
	{
		tiffr.MODE = 0;
		rowptr ds1 = tiffr.getRowData(i);
		tiffr.MODE = 1;
		rowptr ds2 = tiffr.getRowData(i);
		int r = memcmp(ds1, ds2, tiffr.widght());
		if (r != 0)
			qDebug() << "line: " << i << " - " << r;
		delete [] ds1;
		delete [] ds2;
	}
}


int main()
{
	TestTiffReader reader;
	reader.readAllWithCache();
//	reader.compireDecoders();
	return 0;
}

//QTEST_MAIN(TestTiffReader)
//#include "main.moc"
