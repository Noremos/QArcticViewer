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
    connectSettsD(ui->coofSB, &SeachingSettings::setCoof)

    connectSettsI(ui->dminSB, &SeachingSettings::setDiametrMin)
    connectSettsI(ui->dmaxDB, &SeachingSettings::setDiametrMax)

    connectSettsD(ui->minSizeHeiSB, &SeachingSettings::setHeightMin)
    connectSettsD(ui->maxSizeHeiSB, &SeachingSettings::setHeightMax)

    connectSettsI(ui->bottomLenSB, &SeachingSettings::setBottomProc)

	connect(this, &MainWindow::signalProgressValueChawnged, this, &MainWindow::slotSetProgressValue);
	connect(this, &MainWindow::signalProgressMaxChanged, this, &MainWindow::slotSetProgressMax);
	connect(this, &MainWindow::signalProgressMaxChanged, this, &MainWindow::slotSetProgressMax);

	connect(ui->glWidget, &MainWidget::startTimer, this, &MainWindow::StartTheTimer);

	watcher = nullptr;
	//	connect(ui->pbFindByParams, &QPushButton::click, proj, .)

#ifdef RELEASE_AV
	ui->cbUseBarcode->setVisible(false);
	ui->barShoj->setVisible(false);
	ui->cbUseCycle->setVisible(false);
	ui->cirlceShoj->setVisible(false);
	ui->cbUseRegion->setVisible(false);
	ui->cbUseBarcode->setChecked(true);
	ui->cbUseCycle->setChecked(true);
#endif

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
	// openProject("D:\\Progs\\temp\\bar\\proj.qwr");
#ifndef RELEASE_AV
//	openProject("D:\\Programs\\Barcode\\_bar\\bar\\proj.qwr");
	openProject();
#else
	openProject();
#endif
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


// #################### OPEN PROJECT ASYNC ##########################
void MainWindow::openProject()
{
//#ifndef E
	QString projName = QFileDialog::getOpenFileName(0, "Открыть проект", QString(), tr("AW prject (*.qwr)"));
	openProject(projName);
}


void MainWindow::openProject(QString projName)
{

	if (projName.length() == 0)
		return;

//	proj->displayFactor = ui->simpithithion->value();
	proj->loadProject(projName);

	ui->coofSB->setValue(proj->searchSetts.coof);
	ui->dminSB->setValue(proj->searchSetts.diametrMin());
	ui->dmaxDB->setValue(proj->searchSetts.diametrMax());
	ui->minSizeHeiSB->setValue(proj->searchSetts.heightMin());
	ui->maxSizeHeiSB->setValue(proj->searchSetts.heightMax());
	ui->bottomLenSB->setValue(proj->searchSetts.bottomProc);

	ui->glWidget->makeCurrent();
	ui->glWidget->terra->clearTextures();
	ui->glWidget->doneCurrent();


	// ASYNC
	ui->progressBar->reset();
	ui->leftMenu->setEnabled(false);
	ui->topMenu->setEnabled(false);
	ui->pbStopButton->setEnabled(true);

	stopAction = false;
	future1 = QtConcurrent::run(this, &MainWindow::openProjectAsync); // Thread 1

	if (watcher)
		delete watcher;
	watcher = new QFutureWatcher<void>(this);
	connect(watcher, SIGNAL(finished()), this, SLOT(openProjectAsyncEnd()));
	watcher->setFuture(future1);




	//	proj->searchSetts.coof = ui->coofSB->value();
	//	proj->searchSetts.setDiametrMin(ui->dminSB->value());
	//	proj->searchSetts.setDiametrMax(ui->dmaxDB->value());
	//	proj->searchSetts.setHeightMin(ui->minSizeHeiSB->value());
	//	proj->searchSetts.setHeightMax(ui->maxSizeHeiSB->value());
	//	proj->searchSetts.bottomProc = ui->bottomLenSB->value();
}

void MainWindow::openProjectAsync()
{
	using std::placeholders::_1;
	PrjgBarCallback callback(stopAction);
	callback.cbSetMax = std::bind(&MainWindow::bindSetPorogBarMax, this, _1);
	callback.cbIncrValue =  std::bind(&MainWindow::bindIncementProgBarVal, this, _1);

	ui->glWidget->terra->obj.readFastfile(callback, proj->getPath(BackPath::object));
}

void MainWindow::openProjectAsyncEnd()
{
	ui->glWidget->makeCurrent();
	ui->glWidget->terra->initArrays();
	ui->glWidget->terra->setTexture(0, proj->getPath(BackPath::texture1));
	ui->glWidget->terra->setTexture(1, proj->getPath(BackPath::texture2));
//	ui->glWidget->terra->displayTexture(0);

#ifdef ENABLE_MARKERS
	proj->readMarkers();
#endif
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1

#ifdef ENABLE_SHAPE
//	proj->readMyGeo();
//	proj->readGeoshape();
#endif

	ui->glWidget->doneCurrent();

	ui->glWidget->drawTerra = true;

	ui->glWidget->update();

	ui->topMenu->setEnabled(true);
	ui->leftMenu->setEnabled(true);
	ui->pbStopButton->setEnabled(false);

	delete watcher;
	watcher = nullptr;
}

//####################


// ############################ FIND ROI AND BARS ASYNC ##################################
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
	connect(watcher, SIGNAL(finished()), this, SLOT(findROIsAsyncEnd()));
    watcher->setFuture(future1);
}


void MainWindow::findROIsAsync()
{
	using std::placeholders::_1;
	PrjgBarCallback callback(stopAction);
	callback.cbSetMax = std::bind(&MainWindow::bindSetPorogBarMax, this, _1);
	callback.cbIncrValue =  std::bind(&MainWindow::bindIncementProgBarVal, this, _1);

	proj->findROIsOnHiemap(callback, ui->startProc->value(), ui->endProc->value());
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
	connect(watcher, SIGNAL(finished()), this, SLOT(findByParamsAsyncEnd()));
    watcher->setFuture(future1);

	// delete the watcher when finished too
	//	connect(watcher, SIGNAL(finished()), watcher, SLOT(deleteLater()));

}

void MainWindow::findByParamsAsync()
{
	using std::placeholders::_1;
	PrjgBarCallback callback(stopAction);
	callback.cbSetMax = std::bind(&MainWindow::bindSetPorogBarMax, this, _1);
	callback.cbIncrValue =  std::bind(&MainWindow::bindIncementProgBarVal, this, _1);

    proj->filterROIs(callback,
                     ui->cbUseRegion->isChecked(),
                     ui->cbUseBarcode->isChecked(),
                     ui->barShoj->value(),
                     ui->cbUseCycle->isChecked(),
                     ui->cirlceShoj->value(),
					 ui->startProc->value(),
					 ui->endProc->value());
}

void MainWindow::findByParamsAsyncEnd()
{
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

//	ui->searchResult->setText(proj->status);

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
		QString projName = QFileDialog::getExistingDirectory(0, "Куда сохранить проект", QString());
		if (projName.length()==0)
			return;
		ui->progressBar->reset();
		//	ui->pbFindByParams->setEnabled(false);
		ui->leftMenu->setEnabled(false);
		ui->topMenu->setEnabled(false);
		ui->pbStopButton->setEnabled(true);

		stopAction = false;

		proj->setProjectPath(QDir(projName).filePath("project.qwr"));

		future1 = QtConcurrent::run(this, &MainWindow::importDTMAsync, fileName); // Thread 1

		if (watcher)
			delete watcher;
		watcher = new QFutureWatcher<void>(this);
		connect(watcher, SIGNAL(finished()), this, SLOT(importDTMAsyncEnd()));
		watcher->setFuture(future1);

	}

//	openProject(proj->getPath(BackPath::project));
	//	fileDialog.setLabelText("Выберите файл, пожалуйста");
}

void MainWindow::importDTMAsync(QString fileName)
{
	using std::placeholders::_1;
	PrjgBarCallback callback(stopAction);
	callback.cbSetMax = std::bind(&MainWindow::bindSetPorogBarMax, this, _1);
	callback.cbIncrValue =  std::bind(&MainWindow::bindIncementProgBarVal, this, _1);

	proj->loadImage(callback, fileName, ui->simpithithion->value(), 0, ui->startProc->value(), ui->endProc->value());
	if (callback.stopAction)
		return;
	callback.cbSetMax(1);
	ui->glWidget->terra->obj.readFastfile(callback, proj->getPath(BackPath::object));

	opened = true;
}


void MainWindow::importDTMAsyncEnd()
{
	if (opened)
	{
		ui->glWidget->makeCurrent();
		ui->glWidget->terra->initArrays();
		ui->glWidget->doneCurrent();

		proj->saveProject();
		ui->glWidget->drawTerra = true;
	}


	ui->pbStopButton->setEnabled(false);
	ui->topMenu->setEnabled(true);
	ui->leftMenu->setEnabled(true);

	delete watcher;
	watcher = nullptr;
}


void MainWindow::bindIncementProgBarVal(int incr)
{
	emit signalProgressValueChawnged(incr);
}

void MainWindow::StartTheTimer() { timer.start(12, this); }

void MainWindow::bindSetPorogBarMax(int maxVal)
{
	emit signalProgressMaxChanged(maxVal);
}


void MainWindow::slotSetProgressMax(int maxVal)
{
	if (maxVal==0)
	{
		ui->progressBar->setMaximum(1);
		ui->progressBar->setValue(1);
	}
	else
	{
		ui->progressBar->setValue(0);
		ui->progressBar->setMaximum(maxVal);
	}
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
	proj->heiFactor = arg1;
//	ui->glWidget->zones->factor = arg1;
//	qDebug() << ui->glWidget->zones->factor;
}

void MainWindow::on_chShowFinded_stateChanged(int /*arg1*/)
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

void MainWindow::timerEvent(QTimerEvent */*event*/)
{
	ui->glWidget->update();
}


void MainWindow::on_cbUseRegion_stateChanged(int /*arg1*/)
{
    ui->gbZones->setEnabled(ui->chShowFinded->checkState() == Qt::CheckState::Checked);

}

void MainWindow::on_simpithithion_valueChanged(int /*arg1*/)
{
//	proj->displayFactor = arg1;
}

void MainWindow::on_chShowMarker_stateChanged(int /*arg1*/)
{
//	proj->showMarkers = ui->chShowMarker->isChecked();
}
