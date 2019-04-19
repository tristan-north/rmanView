/*
 * viewport.h
 *
 *  Created on: May 19, 2016
 *      Author: ten
 */

#ifndef VIEWPORT_H_
#define VIEWPORT_H_

#include <QLabel>
#include <QVector3D>

#include "common.h"

class Viewport : public QLabel
{
	Q_OBJECT

public:
	Viewport();
	void startRender(char *alembicPath);
	void updatePixmap();
	ucharPixel* getScanlineStart(int lineNum);

protected:
	virtual void mouseMoveEvent(QMouseEvent *event);
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);

private slots:
	void idle();

private:
	QImage *image;
	xform camXform;
	int mouseLastPosX, mouseLastPosY;
	int activeMouseButton; // This gets set to the first button pressed and is used to avoid conflicts when multiple buttons are pressed and released.
};

#endif /* VIEWPORT_H_ */
