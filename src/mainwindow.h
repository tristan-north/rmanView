/*
 * mainwindow.h
 *
 *  Created on: May 17, 2016
 *      Author: ten
 */

#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include <QMainWindow>
#include <QPushButton>
#include "viewport.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

    Viewport viewport;
};

#endif /* MAINWINDOW_H_ */
