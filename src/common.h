/*
 * common.h
 *
 *  Created on: May 18, 2016
 *      Author: ten
 */

#ifndef COMMON_H_
#define COMMON_H_

// Make this a multiple of 16 so that the lowest lvl lod render fits nicely
#define WIDTH 1489
#define HEIGHT 801

#define CAMROTATESPEED 0.5f
#define CAMPANSPEED 0.004f
#define CAMDOLLYSPEED 0.008f

struct ucharPixel {
	unsigned char r;
	unsigned char g;
	unsigned char b;
};

struct xform {
	float tx, ty, tz;
	float rx, ry, rz;
};

#endif /* COMMON_H_ */
