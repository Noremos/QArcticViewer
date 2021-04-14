#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAbstractSpinBox>
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

	void findByParamsAsync();

	QFutureWatcher<void> *watcher;
	QFuture<void> future1;


signals:
	void signalProgressMaxChanged(int);
	void signalProgressValueChawnged(int);

private slots:
	//Progress

	void slotSetProgressMax(int maxVal);
	void slotSetProgressValue(int incr);

	void setPorogBarMax(int maxVal);
	void incementProgBarVal(int incr = 1);

	void findByParamsAsyncEnd();
	void on_mattype_currentIndexChanged(int index);

	void on_textureLoder_clicked();

	void importDTM();
	void openProject();
	void openProject(QString path);
	void findByParams();
	void saveSettings();

	void on_pbOpenProject_clicked();

	void on_pbOpenDTM_clicked();

	void on_pbCreateBars_clicked();

	void on_pbFindByParams_clicked();

	void on_pbSave_clicked();

	void on_heightSpin_valueChanged(int arg1);

	void on_chShowFinded_stateChanged(int arg1);

	void on_pbStopButton_clicked();

private:
	Ui::MainWindow *ui;
	Project* proj;

	bool opened = false;
};
#endif // MAINWINDOW_H
