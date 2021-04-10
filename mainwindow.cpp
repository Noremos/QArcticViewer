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
	QString ret = QFileDialog::getOpenFileName(0, "Импорт ЦМР", QString(), tr("TIFF (*.tif *.tiff)"));
//	fileDialog.setLabelText("Выберите файл, пожалуйста");
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
													"/home/jana/untitled.png",
													tr("ЦМР (*.tif *.tiff )"));

}


MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	  , ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	ui->glWidget->fpsLabel = ui->fpsLabel;
	ui->glWidget->activateWindow();
	setMinMaxSpin(ui->dminSB, ui->dmaxDB);
	setMinMaxSpin(ui->minSizeHeiSB, ui->maxSizeHeiSB);
}

MainWindow::~MainWindow()
{
	delete ui;
}

