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
	if (fileName.length()==0)
		return;
	if (!opened)
	{
		QString projName = QFileDialog::getSaveFileName(0, "Куда сохранить проект", QString(), tr("AW prject (*.qwr)"));
		if (projName.length()==0)
			return;
		proj->setProjectPath(projName);
		opened = true;
	}

	proj->loadImage(fileName, ui->startProc->value(), ui->endProc->value());
	proj->saveProject();

	ui->glWidget->drawTerra = true;



	//	fileDialog.setLabelText("Выберите файл, пожалуйста");
}

void MainWindow::openProject()
{
	QString projName = QFileDialog::getOpenFileName(0, "Открыть проект", QString(), tr("AW prject (*.qwr)"));

	if (projName.length() == 0)
		return;
	proj->loadProject(projName);

	ui->coofSB->setValue(proj->searchSetts.coof);
	ui->dminSB->setValue(proj->searchSetts.diametrMin());
	ui->dmaxDB->setValue(proj->searchSetts.diametrMax());
	ui->minSizeHeiSB->setValue(proj->searchSetts.heightMax());
	ui->maxSizeHeiSB->setValue(proj->searchSetts.heightMin());
	ui->bottomLenSB->setValue(proj->searchSetts.bottomProc);

	ui->glWidget->makeCurrent();
	ui->glWidget->terra->clearTextures();
	ui->glWidget->terra->readfile(proj->getPath(BackPath::object));

	ui->glWidget->terra->setTexture(0, proj->getPath(BackPath::texture1));
	ui->glWidget->terra->setTexture(1, proj->getPath(BackPath::texture2));
	ui->glWidget->drawTerra = true;
	ui->glWidget->doneCurrent();


//	proj->searchSetts.coof = ui->coofSB->value();
//	proj->searchSetts.setDiametrMin(ui->dminSB->value());
//	proj->searchSetts.setDiametrMax(ui->dmaxDB->value());
//	proj->searchSetts.setHeightMin(ui->minSizeHeiSB->value());
//	proj->searchSetts.setHeightMax(ui->maxSizeHeiSB->value());
//	proj->searchSetts.bottomProc = ui->bottomLenSB->value();
}

void MainWindow::findByParams()
{
	proj->findByParams();
}

void MainWindow::saveSettings()
{
	proj->saveProject();
}


#define connectSettsD(filed, signl) \
								 connect(filed, qOverload<double>(&QDoubleSpinBox::valueChanged),\
										 &proj->searchSetts, signl);

#define connectSettsI(filed, signl) \
								 connect(filed, qOverload<int>(&QSpinBox::valueChanged),\
										 &proj->searchSetts, signl);

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

	opened = false;
	proj = Project::getProject();
	// side menu
	connectSettsD(ui->coofSB, &SeachingSettings::setCoof);

	connectSettsI(ui->dminSB, &SeachingSettings::setDiametrMin);
	connectSettsI(ui->dmaxDB, &SeachingSettings::setDiametrMax);

	connectSettsD(ui->minSizeHeiSB, &SeachingSettings::setHeightMin);
	connectSettsD(ui->maxSizeHeiSB, &SeachingSettings::setHeightMax);

	connectSettsI(ui->bottomLenSB, &SeachingSettings::setBottomProc);

//	connect(ui->pbFindByParams, &QPushButton::click, proj, .)

}

MainWindow::~MainWindow()
{
	delete ui;
}


void MainWindow::on_mattype_currentIndexChanged(int index)
{
	ui->textureLoder->setEnabled(index >= 2);
	switch (index)
	{
	case 0: //Выстоа
		ui->glWidget->terra->displayHeimap(-100,200);
		break;
	case 1: //Материал
		ui->glWidget->terra->displayObject();
		break;
	default:
		ui->glWidget->terra->displayTexture(index - 2);
		break;
	}
}

void MainWindow::on_textureLoder_clicked()
{
	QString fileName = QFileDialog::getOpenFileName(0, "Открыть текстуру", QString(), tr("Image (*.tif *.tiff *.png *.jpg *.bmp)"));
	if (fileName.length() == 0)
		return;

	ui->glWidget->terra->setTexture(ui->mattype->currentIndex()-2, fileName);
}

void MainWindow::on_pbOpenProject_clicked()
{
	openProject();
}

void MainWindow::on_pbOpenDTM_clicked()
{
	importDTM();
}

void MainWindow::on_pbCreateBars_clicked()
{
	proj->processHiemap(ui->startProc->value(), ui->endProc->value());
}

void MainWindow::on_pbFindByParams_clicked()
{
	findByParams();
}

void MainWindow::on_pbSave_clicked()
{
	saveSettings();
}

void MainWindow::on_heightSpin_valueChanged(int arg1)
{
	ui->glWidget->terra->factor = arg1;
	qDebug() << ui->glWidget->terra->factor;
}
