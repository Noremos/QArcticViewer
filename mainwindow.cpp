#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	  , ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	ui->glWidget->fpsLabel = ui->fpsLabel;
	ui->glWidget->activateWindow();
}

MainWindow::~MainWindow()
{
	delete ui;
}

