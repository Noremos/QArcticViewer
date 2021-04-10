#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAbstractSpinBox>
#include <QMainWindow>
#include <QSpinBox>

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

public slots:
	void importDTM();
private:
	Ui::MainWindow *ui;
	Project proj;



};
#endif // MAINWINDOW_H
