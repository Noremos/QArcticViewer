#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAbstractSpinBox>
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

private slots:
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

private:
	Ui::MainWindow *ui;
	Project* proj;

	bool opened = false;
};
#endif // MAINWINDOW_H
