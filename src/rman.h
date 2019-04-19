/*
 * rman.h
 *
 *  Created on: May 18, 2016
 *      Author: ten
 */

#ifndef RMAN_H_
#define RMAN_H_

#include "common.h"

namespace rman {

void startRender(char *alembicPath, xform camXform);
void shutdown();
void processCallbacks();
int getProgress();
void cameraEdit(xform camXform);

}

#endif /* RMAN_H_ */
