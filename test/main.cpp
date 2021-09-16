//#include <QtTest/QtTest>
#define DISABLE_GUI

#include "../src/core/tiffreader.h"
#include "../mainwidget.h"
#include "../src/core/project.h"

#include <QObject>
#include <QPixmap>

//#define QT_NO_DEBUG_OUTPUT

class TestTiffReader//: public QObject
{
//	Q_OBJECT
	TiffReader tiffr;
	Project *proj;
public:
	TestTiffReader()
	{
		tiffr.open("D:/Learning/BAR/Moscow/50_59_1_2_2m_v3.0/50_59_1_2_2m_v3.0_reg_dem.tif");
		proj = Project::getProject();
		proj->badZones = new SpotZones();
		proj->spotZones = new SpotZones();
	}
	void readAllWithCache();
	void checkBadRow();
	void compireDecoders();
	void openAndFind();
	void openAndProcess();
	virtual ~TestTiffReader() {};
};

void TestTiffReader::readAllWithCache()
{

	tiffr.setRowsCacheSize(100);
	for (int i = 161; i < tiffr.height(); i++)
	{
		tiffr.getRow(i);
	}
	//	QCOMPARE(true, true);
}

void TestTiffReader::checkBadRow()
{
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


void TestTiffReader::openAndFind()
{
	//	MainWidget *glWidget = new MainWidget();
	//	ui->glWidget->fpsLabel = ui->fpsLabel;
	proj->loadProject("D:/Programs/Barcode/_bar/arctic/proj.qwr");
	volatile bool stopAction = false;

	using std::placeholders::_1;
	PrjgBarCallback callback(stopAction);
//	callback.cbSetMax = std::bind(&MainWindow::bindSetPorogBarMax, this, _1);
//	callback.cbIncrValue =  std::bind(&MainWindow::bindIncementProgBarVal, this, _1);
	proj->findROIsOnHiemap(callback, 1,  100001);
	//	delete glWidget;
}

void TestTiffReader::openAndProcess()
{
	proj->loadProject("D:/Programs/Barcode/_bar/arctic/proj.qwr");
	volatile bool stopAction = false;

	using std::placeholders::_1;
	PrjgBarCallback callback(stopAction);
	proj->filterROIs(callback, true, false, 0.5, false, 0.5, 0, 25000);
//	proj->filterROIs(callback, true, false, 0.5, false, 0.5, 0, 25000);
//	proj->filterROIs(callback, true, false, 0.5, false, 0.5, 0, 25000);
//	proj->filterROIs(callback, true, false, 0.5, false, 0.5, 0, 25000);
}

void checkTime()
{
	using std::chrono::high_resolution_clock;
	using std::chrono::duration_cast;
	using std::chrono::duration;
	using std::chrono::milliseconds;

	TestTiffReader reader;
		auto t1 = high_resolution_clock::now();
		reader.readAllWithCache();
		auto t2 = high_resolution_clock::now();
		auto ms_int = duration_cast<milliseconds>(t2 - t1);
		std::cout << "time:" << (double) (ms_int.count()) / 1000;

		t1 = high_resolution_clock::now();
		reader.readAllWithCache();
		t2 = high_resolution_clock::now();
		ms_int = duration_cast<milliseconds>(t2 - t1);
		std::cout << "time:" << (double) (ms_int.count()) / 1000;

		t1 = high_resolution_clock::now();
		reader.readAllWithCache();
		t2 = high_resolution_clock::now();
		ms_int = duration_cast<milliseconds>(t2 - t1);
		std::cout << "time:" << (double) (ms_int.count()) / 1000;

}
int main(int argc, char *argv[])
{
	system("pause");
	std::cout << "started" << std::endl;
	TestTiffReader reader;

	// find ROIS
//	reader.openAndFind();

	// FILTER
	reader.openAndProcess();

//	system("pause");
	return 0;
}

//QTEST_MAIN(TestTiffReader)
//#include "main.moc"
