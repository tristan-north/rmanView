/*
 * viewport.cpp
 *
 *  Created on: May 19, 2016
 *      Author: ten
 */

#include <QtGui>

#include "viewport.h"
#include "common.h"
#include "rman.h"


Viewport::Viewport() : activeMouseButton(Qt::NoButton) {
	camXform.tx = 0.0f;
	camXform.ty = 0.0f;
	camXform.tz = 4.0f;
	camXform.rx = camXform.ry = camXform.rz = 0.0f;

	image = new QImage(WIDTH, HEIGHT, QImage::Format_RGB888);
	image->fill(0x333333);
	setPixmap(QPixmap::fromImage(*image));

	QTimer::singleShot(50, this, SLOT(idle()));
}

void Viewport::idle() {
	rman::processCallbacks();
//	int percentDone = rman::getProgress();
//	printf("%d%%\n", percentDone);

	updatePixmap();

//	if (percentDone >= 99) {
//		printf("Done render\n");
//	} else {
//		QTimer::singleShot(50, this, SLOT(idle()));
//	}

	QTimer::singleShot(20, this, SLOT(idle()));
}

void Viewport::mousePressEvent(QMouseEvent *event){
	// Don't do anything if its not left middle or right button
	if (! ((event->button() == Qt::LeftButton) || (event->button() == Qt::MidButton) || (event->button() == Qt::RightButton)))
		return;

	// If another button was already pressed first just return
	if (activeMouseButton != Qt::NoButton) return;

	activeMouseButton = event->button();

	mouseLastPosX = event->x();
	mouseLastPosY = event->y();
}


void Viewport::mouseReleaseEvent(QMouseEvent *event){
	if (event->button() == activeMouseButton)
		activeMouseButton = Qt::NoButton;
}


void Viewport::mouseMoveEvent(QMouseEvent *event){
	int posDifferenceX = event->x() - mouseLastPosX;
	int posDifferenceY = event->y() - mouseLastPosY;

    if( (event->buttons() & Qt::LeftButton) && (activeMouseButton == Qt::LeftButton) ) {
    	camXform.rx -= posDifferenceY * CAMROTATESPEED;
    	camXform.ry -= posDifferenceX * CAMROTATESPEED;
    	rman::cameraEdit(camXform);
    }
    else if( (event->buttons() & Qt::MidButton) && (activeMouseButton == Qt::MidButton) ) {
    	camXform.tx += posDifferenceX*CAMPANSPEED;
    	camXform.ty -= posDifferenceY*CAMPANSPEED;
    	rman::cameraEdit(camXform);
    }
    else if( (event->buttons() & Qt::RightButton) && (activeMouseButton == Qt::RightButton) ) {
    	camXform.tz -= (posDifferenceX + posDifferenceY) * CAMDOLLYSPEED;
    	rman::cameraEdit(camXform);
    }

    mouseLastPosX = event->x();
    mouseLastPosY = event->y();
}


void Viewport::updatePixmap() {
	setPixmap(QPixmap::fromImage(*image));
}


void Viewport::startRender(char *alembicPath) {
	rman::startRender(alembicPath, camXform);
}


ucharPixel* Viewport::getScanlineStart(int lineNum) {
	return (ucharPixel*)image->scanLine(lineNum);
}
