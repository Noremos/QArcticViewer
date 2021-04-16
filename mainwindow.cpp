#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QFuture>
#include <QObject>
#include <QThread>
#include <QtConcurrent/QtConcurrent>



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

	connect(this, &MainWindow::signalProgressValueChawnged, this, &MainWindow::slotSetProgressValue);
	connect(this, &MainWindow::signalProgressMaxChanged, this, &MainWindow::slotSetProgressMax);

	watcher = nullptr;
	//	connect(ui->pbFindByParams, &QPushButton::click, proj, .)

}

MainWindow::~MainWindow()
{
	future1.cancel();
	while (!future1.isCanceled());

	delete ui;
	if (watcher)
		delete watcher;
}



void MainWindow::on_pbOpenProject_clicked()
{
	openProject("D:\\Programs\\Barcode\\_bar\\p2\\proj.qwr");
	//	openProject();
}

void MainWindow::openProject()
{
	QString projName = QFileDialog::getOpenFileName(0, "Открыть проект", QString(), tr("AW prject (*.qwr)"));
	openProject(projName);
}

void MainWindow::openProject(QString projName)
{

	if (projName.length() == 0)
		return;
	proj->loadProject(projName);

	ui->coofSB->setValue(proj->searchSetts.coof);
	ui->dminSB->setValue(proj->searchSetts.diametrMin());
	ui->dmaxDB->setValue(proj->searchSetts.diametrMax());
	ui->minSizeHeiSB->setValue(proj->searchSetts.heightMin());
	ui->maxSizeHeiSB->setValue(proj->searchSetts.heightMax());
	ui->bottomLenSB->setValue(proj->searchSetts.bottomProc);

	ui->glWidget->makeCurrent();
	ui->glWidget->terra->clearTextures();
	ui->glWidget->terra->readfile(proj->getPath(BackPath::object));

	ui->glWidget->terra->setTexture(0, proj->getPath(BackPath::texture1));
	ui->glWidget->terra->setTexture(1, proj->getPath(BackPath::texture2));
	ui->glWidget->terra->displayTexture(0);
	ui->glWidget->drawTerra = true;
	ui->glWidget->doneCurrent();

	ui->glWidget->update();


	//	proj->searchSetts.coof = ui->coofSB->value();
	//	proj->searchSetts.setDiametrMin(ui->dminSB->value());
	//	proj->searchSetts.setDiametrMax(ui->dmaxDB->value());
	//	proj->searchSetts.setHeightMin(ui->minSizeHeiSB->value());
	//	proj->searchSetts.setHeightMax(ui->maxSizeHeiSB->value());
	//	proj->searchSetts.bottomProc = ui->bottomLenSB->value();
}




void MainWindow::setMinMaxSpin(QSpinBox *boxMin, QSpinBox *boxMax)
{
	connect(boxMin, qOverload<int>(&QSpinBox::valueChanged), boxMax, &QSpinBox::setMinimum);
//	connect(boxMax, qOverload<int>(&QSpinBox::valueChanged), boxMin, &QSpinBox::setMaximum);
}

void MainWindow::setMinMaxSpin(QDoubleSpinBox *boxMin, QDoubleSpinBox *boxMax)
{
	connect(boxMin, qOverload<double>(&QDoubleSpinBox::valueChanged), boxMax, &QDoubleSpinBox::setMinimum);
}

// FIND ROI AND BARS
void MainWindow::findROIs()
{
	ui->progressBar->reset();

//	ui->pbCreateBars->setEnabled(false);
	ui->leftMenu->setEnabled(false);
	ui->topMenu->setEnabled(false);
	ui->pbStopButton->setEnabled(true);

	stopAction = false;

	future1 = QtConcurrent::run(this, &MainWindow::findROIsAsync); // Thread 1

	if (watcher)
		delete watcher;
	watcher = new QFutureWatcher<void>(this);
	watcher->setFuture(future1);
	connect(watcher, SIGNAL(finished()), this, SLOT(findROIsAsyncEnd()));
}

void MainWindow::findROIsAsyncEnd()
{
	qDebug() << "ROI done";

//	ui->pbCreateBars->setEnabled(true);

	ui->topMenu->setEnabled(true);
	ui->leftMenu->setEnabled(true);
	ui->pbStopButton->setEnabled(false);

	delete watcher;
	watcher = nullptr;
}

void MainWindow::findROIsAsync()
{
	using std::placeholders::_1;
	PrjgBarCallback callback(stopAction);
	callback.cbSetMax = std::bind(&MainWindow::bindSetPorogBarMax, this, _1);
	callback.cbIncrValue =  std::bind(&MainWindow::bindIncementProgBarVal, this, _1);

	proj->findROIsOnHiemap(callback, ui->startProc->value(), ui->endProc->value());
}

//#####################




// FIND BURS IN ROI
void MainWindow::findByParams()
{
	ui->progressBar->reset();
//	ui->pbFindByParams->setEnabled(false);
	ui->leftMenu->setEnabled(false);
	ui->topMenu->setEnabled(false);
	ui->pbStopButton->setEnabled(true);

	stopAction = false;

	future1 = QtConcurrent::run(this, &MainWindow::findByParamsAsync); // Thread 1

	if (watcher)
		delete watcher;
	watcher = new QFutureWatcher<void>(this);
	watcher->setFuture(future1);
	connect(watcher, SIGNAL(finished()), this, SLOT(findByParamsAsyncEnd()));

	// delete the watcher when finished too
	//	connect(watcher, SIGNAL(finished()), watcher, SLOT(deleteLater()));

}

void MainWindow::findByParamsAsync()
{
	using std::placeholders::_1;
	PrjgBarCallback callback(stopAction);
	callback.cbSetMax = std::bind(&MainWindow::bindSetPorogBarMax, this, _1);
	callback.cbIncrValue =  std::bind(&MainWindow::bindIncementProgBarVal, this, _1);

	proj->filterROIs(callback);
}

void MainWindow::findByParamsAsyncEnd()
{
	qDebug() << "Update beffers";
	ui->glWidget->makeCurrent();

	proj->spotZones->updateBuffer();
	proj->text->updateBuffer();
	ui->glWidget->drawZones = true;

	ui->glWidget->doneCurrent();


	finded = true;
//	ui->pbFindByParams->setEnabled(true);
	ui->pbStopButton->setEnabled(false);
	ui->topMenu->setEnabled(true);
	ui->leftMenu->setEnabled(true);

	ui->glWidget->update();

	delete watcher;
	watcher = nullptr;
}
//#####################



void MainWindow::importDTM()
{
	QFileDialog fileDialog;

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

	proj->loadImage(fileName, ui->simpithithion->value(), 0);
	proj->saveProject();

	ui->glWidget->drawTerra = true;
	//	fileDialog.setLabelText("Выберите файл, пожалуйста");
}


void MainWindow::bindIncementProgBarVal(int incr)
{
	emit signalProgressValueChawnged(incr);
}

void MainWindow::bindSetPorogBarMax(int maxVal)
{
	emit signalProgressMaxChanged(maxVal);
}


void MainWindow::slotSetProgressMax(int maxVal)
{
	ui->progressBar->setMaximum(maxVal);
}

void MainWindow::slotSetProgressValue(int incr)
{
	int val = ui->progressBar->value();
	int maxval = ui->progressBar->maximum();
	if (val + incr > maxval)
	{
		ui->progressBar->setValue(maxval);
	}
	else
		ui->progressBar->setValue(val + incr);
}

void MainWindow::saveSettings()
{
	proj->saveProject();
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
	QString fileName = QFileDialog::getOpenFileName(0, "Открыть текстуру", QString(),
													tr("Image (*.tif *.tiff *.png *.jpg *.bmp)"));
	if (fileName.length() == 0)
		return;

	ui->glWidget->terra->setTexture(ui->mattype->currentIndex() - 2, fileName);

	if (ui->mattype->currentIndex() - 2 == 0)
		proj->texturePath = fileName;
	if (ui->mattype->currentIndex() - 2 == 1)
		proj->texture2Path = fileName;

	saveSettings();
}


void MainWindow::on_pbOpenDTM_clicked()
{
	importDTM();
}

void MainWindow::on_pbCreateBars_clicked()
{
	findROIs();
}

void MainWindow::on_pbFindByParams_clicked()
{
//	qDebug() << "MainThread: " << QThread::currentThread();
	findByParams();
}

void MainWindow::on_pbSave_clicked()
{
	saveSettings();
}

void MainWindow::on_heightSpin_valueChanged(int arg1)
{
	ui->glWidget->terra->factor = arg1;
	ui->glWidget->zones->factor = arg1;
	qDebug() << ui->glWidget->zones->factor;
}

void MainWindow::on_chShowFinded_stateChanged(int arg1)
{
	if (finded)
	{
		ui->glWidget->drawZones = ui->chShowFinded->checkState() == Qt::CheckState::Checked;
		ui->glWidget->update();
	}

}

void MainWindow::on_pbStopButton_clicked()
{
	stopAction = true;
}
