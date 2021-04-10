#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QObject>

void MainWindow::setMinMaxSpin(QSpinBox *boxMin, QSpinBox *boxMax)
{
	connect(boxMin, qOverload<int>(&QSpinBox::valueChanged), boxMax, &QSpinBox::setMinimum);
//	connect(boxMax, qOverload<int>(&QSpinBox::valueChanged), boxMin, &QSpinBox::setMaximum);
}


void MainWindow::setMinMaxSpin(QDoubleSpinBox *boxMin, QDoubleSpinBox *boxMax)
{
	connect(boxMin, qOverload<double>(&QDoubleSpinBox::valueChanged), boxMax, &QDoubleSpinBox::setMinimum);
}

void MainWindow::importDTM()
{
	QFileDialog fileDialog;

	QString defaultPath ="D:/";
	QString fileName = QFileDialog::getOpenFileName(0, "Импорт ЦМР", QString(), tr("TIFF (*.tif *.tiff)"));
//	fileDialog.setLabelText("Выберите файл, пожалуйста");
}


#define connectSettsD(filed, signl) \
								 connect(filed, qOverload<double>(&QDoubleSpinBox::valueChanged),\
										 &proj->searchSetts, signl);\

#define connectSettsI(filed, signl) \
								 connect(filed, qOverload<int>(&QSpinBox::valueChanged),\
										 &proj->searchSetts, signl);\

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent),
	  ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	ui->glWidget->fpsLabel = ui->fpsLabel;
	ui->glWidget->activateWindow();
	setMinMaxSpin(ui->dminSB, ui->dmaxDB);
	setMinMaxSpin(ui->minSizeHeiSB, ui->maxSizeHeiSB);

//	connect(ui->coofSB, qOverload<double>(&QDoubleSpinBox::valueChanged), sets, &SeachingSettings::setCoof);
//	connect(ui->dminSB, qOverload<int>(&QSpinBox::valueChanged), sets, &SeachingSettings::setDiametrMin);
//	connect(ui->dmaxDB, qOverload<int>(&QSpinBox::valueChanged), sets, &SeachingSettings::setDiametrMax);
//	connect(ui->minSizeHeiSB, qOverload<double>(&QDoubleSpinBox::valueChanged), sets, &SeachingSettings::setHeightMin);
//	connect(ui->maxSizeHeiSB, qOverload<double>(&QDoubleSpinBox::valueChanged), sets, &SeachingSettings::setHeightMax);

//	connect(ui->bottomLenSB, qOverload<int>(&QSpinBox::valueChanged), sets, &SeachingSettings::setBottomProc);

	connectSettsD(ui->coofSB, &SeachingSettings::setCoof);

	connectSettsI(ui->dminSB, &SeachingSettings::setDiametrMin);
	connectSettsI(ui->dmaxDB, &SeachingSettings::setDiametrMax);

	connectSettsD(ui->minSizeHeiSB, &SeachingSettings::setHeightMin);
	connectSettsD(ui->maxSizeHeiSB, &SeachingSettings::setHeightMax);

	connectSettsI(ui->bottomLenSB, &SeachingSettings::setBottomProc);

	proj = new Project();
}

MainWindow::~MainWindow()
{
	delete ui;
}

