#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAbstractSpinBox>
#include <QBasicTimer>
#include <QFutureWatcher>
#include <QMainWindow>
#include <QSpinBox>
#include "src/core/project.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();

	void setMinMaxSpin(QSpinBox *boxMin, QSpinBox *boxMax);
	void setMinMaxSpin(QDoubleSpinBox *boxMin, QDoubleSpinBox *boxMax);
	bool finded = false;
	volatile bool stopAction = false;


	QBasicTimer timer;

	// for async
	QFutureWatcher<void> *watcher;
	QFuture<void> future1;

	// find roi and create barcodes on it
	void findROIs();
	void findROIsAsync();

	// Filer ROIs
	void findByParams();
	void findByParamsAsync();

	// call from another thread
	void bindSetPorogBarMax(int maxVal);
	void bindIncementProgBarVal(int incr = 1);

	void openProject(QString path);
	void openProject();
	void openProjectAsync();

	void importDTM();
	void importDTMAsync(QString fileName);

signals:
	void signalProgressMaxChanged(int);
	void signalProgressValueChawnged(int);

private slots:

	void StartTheTimer();

	void findROIsAsyncEnd();
	void findByParamsAsyncEnd();
	void openProjectAsyncEnd();
	void importDTMAsyncEnd();

	//Progress

	// slot that will be called from another slot
	void slotSetProgressMax(int maxVal);
	void slotSetProgressValue(int incr);



	void on_mattype_currentIndexChanged(int index);

	void on_textureLoder_clicked();


	void saveSettings();

	void on_pbOpenProject_clicked();

	void on_pbOpenDTM_clicked();

	void on_pbCreateBars_clicked();

	void on_pbFindByParams_clicked();

	void on_pbSave_clicked();

	void on_heightSpin_valueChanged(int arg1);

	void on_chShowFinded_stateChanged(int arg1);

	void on_pbStopButton_clicked();

    void on_cbUseRegion_stateChanged(int arg1);

private:
	Ui::MainWindow *ui;
	Project* proj;

	bool opened = false;

	// QPaintDevice interface
public:
	// QObject interface
protected:
	void timerEvent(QTimerEvent *event) override;
};
#endif // MAINWINDOW_H
