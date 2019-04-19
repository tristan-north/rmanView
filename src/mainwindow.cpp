/*
 * mainwindow.cpp
 *
 *  Created on: May 17, 2016
 *      Author: ten
 */

#include <QWidget>
#include <QTimer>

#include "mainwindow.h"
#include "common.h"
#include "rman.h"
#include "timer.h"

MainWindow::MainWindow() {
	setWindowTitle("rmanView");
	resize(WIDTH, HEIGHT);

	setCentralWidget(&viewport);
}


