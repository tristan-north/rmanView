/*
 * rman.cpp
 *
 *  Created on: May 18, 2016
 *      Author: ten
 */

#include <iostream>
#include <cstring>
#include <stdlib.h>

#include "ndspy.h"
#include "ri.h"
#include "Ric.h"

#include "rman.h"
#include "common.h"
#include "mainwindow.h"

extern MainWindow *g_mainWindow;

namespace rman {

// This gets set for each image LOD in DspyImageOpen and passed in to the other
// Dspy functions by rman.
struct ImageInfo {
	char name[256];
	int width;
	int height;
	int lod;
};

void createWorld(char *alembicPath, xform camXform) {
	// Set up procedural search path
	char *rmantree = getenv("RMANTREE");
	if(!rmantree) {fprintf(stderr, "Error: RMANTREE env var not defined.\n"); return;}
	char procSearchPath[strlen(rmantree)+10];
	strcpy(procSearchPath, rmantree);
	strcat(procSearchPath, "/etc");
	RtToken procSearchPathToken = procSearchPath;
	RiOption("searchpath", "string procedural", &procSearchPathToken, RI_NULL);

	RiFormat(WIDTH, HEIGHT, 1);
	RtFloat CameraFOV = float(26.9915);
	RiProjection(RI_PERSPECTIVE, RI_FOV, &CameraFOV, RI_NULL);
//	RiCropWindow(.3, .7, .25, .75);
//	RiShutter(0, 1);
//	RiShadingRate(1);
	RiDisplay("Output", "multires", "rgba", RI_NULL);
	RtToken path = "path";
	RiHider("raytrace", "string integrationmode", &path, RI_NULL);
//	Integrator "PxrPathTracer" "PxrPathTracer" "int maxPathLength" [10] "string sampleMode" ["bxdf"] "int numLightSamples" [8] "int numBxdfSamples" [8] "int numIndirectSamples" [1]
	RtInt maxPathLength = 3;
	RtToken sampleMode = "bxdf";
	RtInt numLightSamples = 4;
	RtInt numBxdfSamples = 1;
	RtInt numIndirectSamples = 1;
	RiIntegrator("PxrPathTracer", "handle", "int maxPathLength", &maxPathLength, "string sampleMode", &sampleMode, "int numLightSamples", &numLightSamples, "int numBxdfSamples",  &numBxdfSamples, "int numIndirectSamples", &numIndirectSamples, RI_NULL);

	// Initial cam transform
	RiTranslate(camXform.tx, camXform.ty, camXform.tz);
	RiRotate(camXform.rx, 1, 0, 0);
	RiRotate(camXform.ry, 0, 1, 0);
	RiRotate(camXform.rz, 0, 0, 1);
	RiWorldBegin();
		// Directional light
		RiAttributeBegin();
			RiRotate(45, 0, 1, 0);
			RiRotate(-45, 1, 0, 0);
			RiBxdf("PxrLightEmission", "emission", RI_NULL);
			RtToken distant = "distant";
			RtFloat exposure = 10.0f;
			RiAreaLightSource("PxrStdAreaLight", "float exposure", &exposure, "string rman__Shape", &distant, RI_NULL);
			RtInt zero = 0;
			RiAttribute("visibility", "int camera", &zero, "int indirect", &zero, "int transmission", &zero, RI_NULL);
			RtFloat lightAngle = 3.0f;
			RiGeometry("distantlight", "constant float anglesubtended", &lightAngle, RI_NULL);
		RiAttributeEnd();

		// Env light
		RiAttributeBegin();
			RiShadingRate(10);
			RiBxdf("PxrLightEmission", "emissionEnvLgt", RI_NULL);
			exposure = 6.0f;
//			RiAreaLightSource("PxrStdEnvMapLight", "float exposure", &exposure, RI_NULL);
			RiAttribute("visibility", "int camera", &zero, "int indirect", &zero, "int transmission", &zero, RI_NULL);
			RtFloat resolution[] = {512, 256};
			RiGeometry("envsphere", "constant float[2] resolution", resolution, RI_NULL);
		RiAttributeEnd();

		RiBxdf("PxrDiffuse", "default", RI_NULL);  // Default material

		char filenameArg[strlen(alembicPath) + 11];
		strcpy(filenameArg, "-filename ");
		strcat(filenameArg, alembicPath);
		printf("***** %s ****\n", filenameArg);
		RtString procArgs[] = { "AlembicProcPrim.so", filenameArg };
		RtBound procBounds = {-100000, 100000, -100000, 100000, -100000, 100000};
		RiProcedural((RtPointer)procArgs, procBounds, RiProcDynamicLoad, NULL );

	RiWorldEnd();
}

PtDspyError DspyImageOpen( PtDspyImageHandle *ppvImage, const char *drivername, const char *filename, int width, int height, int paramCount, const UserParameter *parameters, int formatCount, PtDspyDevFormat *format, PtFlagStuff *flagstuff ) {
    fprintf(stderr, "DspyImageOpen(): \"%s\" %d %d\n", filename, width, height);

    ImageInfo *imageInfo = new ImageInfo;
    strncpy(imageInfo->name, filename, 255);
    imageInfo->width = width;
    imageInfo->height = height;

    // Figure out which lod this is from the filename
    char lastCharOfName = filename[(strlen(filename)-1)];
    if (isdigit(lastCharOfName)) {
    	imageInfo->lod = lastCharOfName - '0'; // Convert from char to int
    } else {
    	imageInfo->lod = 0;
    }

    *ppvImage = static_cast<PtDspyImageHandle>(imageInfo);

    // Set each channel to float format
    for (int i = 0; i < formatCount; i++) format[i].type = PkDspyFloat32;

	return PkDspyErrorNone;
}

PtDspyError DspyImageQuery( PtDspyImageHandle pvImage, PtDspyQueryType querytype, int datalen, void *data ) {
	PtDspyError ret = PkDspyErrorNone;
	if (datalen > 0 && NULL != data)
	{
		switch (querytype) {
			case PkOverwriteQuery:
			{
				printf("PXR CALLBACK - DSPYIMAGEQUERY - Overwrite query\n");

				PtDspyOverwriteInfo* overwriteInfo = (PtDspyOverwriteInfo*) data;
				if (datalen > sizeof(*overwriteInfo))
				datalen = sizeof(*overwriteInfo);
				overwriteInfo->overwrite = 0;
				overwriteInfo->interactive = 0;
				break;
			}
			case PkRedrawQuery:
			{
				printf("PXR CALLBACK - DSPYIMAGEQUERY - Redraw query\n");

				PtDspyRedrawInfo*  redrawInfo = (PtDspyRedrawInfo*) data;
				if (datalen > sizeof(*redrawInfo))
				datalen = sizeof(*redrawInfo);
				redrawInfo->redraw = 1;

				break;
			}
			case PkSizeQuery:
			{
				printf("PXR CALLBACK - DSPYIMAGEQUERY - Size query\n");

				PtDspySizeInfo* sizeInfo = (PtDspySizeInfo*) data;
				if (datalen > sizeof(*sizeInfo))
				datalen = sizeof(*sizeInfo);
	//				MRImage *image = static_cast<MRImage*>(pvImage);
	//				sizeInfo->width = image->cropWidth;
	//				sizeInfo->height = image->cropHeight;
	//				sizeInfo->aspectRatio = 1.0f;

				break;
			}
			case PkMultiResolutionQuery:
			{
				// From this thread: https://renderman.pixar.com/forum/showthread.php?s=&threadid=26498
				// "Just before 19 shipped we found a race condition with the multires query, and rather
				// than fix the underlying issue, the decision was made to assume that any display driver
				// used with re-rendering was multires capable. But! Please continue to code as though the
				// query were happening. We will hopefully address the underlying issue soon."
				printf("PXR CALLBACK - DSPYIMAGEQUERY - MultiResolution query\n");

				PtDspyMultiResolutionQuery* multiresInfo = (PtDspyMultiResolutionQuery*) data;
				if (datalen > sizeof(*multiresInfo))
					datalen = sizeof(*multiresInfo);

				multiresInfo->supportsMultiResolution = 1;
				break;
			}
			case PkNextDataQuery:
			{
				// From this thread: https://renderman.pixar.com/forum/showthread.php?s=&threadid=26498
				// "The PkNextDataQuery is ignored by all of our display drivers, so it is safe for yours to ignore it as well.
				// This looks to be a bit of legacy code that needs to be excised."
				ret = PkDspyErrorUnsupported;
				break;
			}
			default:
			{
				printf("PXR CALLBACK - DSPYIMAGEQUERY - Unsupported query: %d\n", querytype);
				ret = PkDspyErrorUnsupported;
			}
			break;
		}
	}
	else
	{
		printf("PXR CALLBACK - DSPYIMAGEQUERY - Bad params query\n");
		ret = PkDspyErrorBadParams;
	}
	return ret;
}

PtDspyError DspyImageData( PtDspyImageHandle pvImage, int xmin, int xmax_plusone,  int ymin, int ymax_plusone, int entrysize, const unsigned char *data ) {
	ImageInfo *imageInfo = static_cast<ImageInfo*>(pvImage);

//	if(imageInfo->lod != 1) return PkDspyErrorNone;

	// How many full res pixels does each recieved pixel represent in x and y
	int lodScaleX = WIDTH / imageInfo->width;
	int lodScaleY = HEIGHT / imageInfo->height;

	// For each pixel recieved from rman
	for (int y = ymin; y < ymax_plusone; y++) {
		for (int x = xmin; x < xmax_plusone; x++) {
			float r = *((float*)data);
			float g = *((float*)data + 1);
			float b = *((float*)data + 2);

			// Clamp to white
			r = r > 255.f ? 255.f : r;
			g = g > 255.f ? 255.f : g;
			b = b > 255.f ? 255.f : b;

			// For each full res pixel represented by recieved pixel
			for(int j = 0; j < lodScaleY; j++) {
				ucharPixel *scanline = g_mainWindow->viewport.getScanlineStart(y*lodScaleY+j);
				for(int i = 0; i < lodScaleX; i++) {
					scanline[x*lodScaleX+i].r = r;
					scanline[x*lodScaleX+i].g = g;
					scanline[x*lodScaleX+i].b = b;

					// Color coded based on LOD
//					float r = 0;
//					float g = 0;
//					float b = 0;
//					if( imageInfo->lod == 3 ) r = 255;
//					if( imageInfo->lod == 2 ) g = 255;
//					if( imageInfo->lod == 1 ) b = 255;
//					if( imageInfo->lod == 0 ) r = g = b = 255;
//					scanline[x*lodScaleX+i].r = r;
//					scanline[x*lodScaleX+i].g = g;
//					scanline[x*lodScaleX+i].b = b;
				}
			}

			data += entrysize;
		}
	}

	//	printf("DspyImageData() xmin: %d  xmax_plus_1: %d  ymin: %d   ymax_plus_1: %d  entrysize: %d\n", xmin, xmax_plusone, ymin, ymax_plusone, entrysize);

	return PkDspyErrorNone;
}

PtDspyError DspyImageActiveRegion( PtDspyImageHandle pvImage, int xmin, int xmax_plusone, int ymin, int ymax_plusone) {  // exclusive bound
//	printf("DspyImageActiveRegion(): xmin: %d  xmax_plusone: %d  ymin: %d  ymax_plusone: %d\n");
//	for(int i = 0; i < WIDTH*HEIGHT*3; i++) g_renderPixels[i] = 0;

	return PkDspyErrorNone;
}

PtDspyError DspyImageClose( PtDspyImageHandle pvImage ) {
	ImageInfo *imageInfo = static_cast<ImageInfo*>(pvImage);
//	printf("DspyImageClose(): %s\n", imageInfo->name);

	return PkDspyErrorNone;
}

void startRender(char *alembicPath, xform camXform) {
	PtDspyDriverFunctionTable dt;
	dt.Version = k_PtDriverCurrentVersion;
	dt.pOpen  = DspyImageOpen;
	dt.pWrite = DspyImageData;
	dt.pClose = DspyImageClose;
	dt.pQuery = DspyImageQuery;
	dt.pActiveRegion = DspyImageActiveRegion;
	PtDspyError err = DspyRegisterDriverTable("direct", &dt); // register the direct-linked display driver

	PRManBegin(0, 0); // This is command line arguments argc and argv

	// Write out an initial rib file
	RiBegin("scene.rib");
	createWorld(alembicPath, camXform);
	RiEnd();

	RiBegin("launch:prman? -ctrl $ctrlin $ctrlout -dspy $dspyin $dspyout");
	RiDisplay("Output", "direct", "rgb", RI_NULL);
	RtToken path = "path";
	RtInt one = 1;
	RiHider("raytrace", "string integrationmode", &path, "int incremental", &one, RI_NULL);
	RtToken rerenderer = "raytrace";
	RiEditWorldBegin("scene.rib", "string rerenderer", &rerenderer, RI_NULL);
}

void cameraEdit(xform camXform) {
	static int counter = 0;

	char marker[20];
	sprintf(marker, "%s%d", "moveMarker", counter);
	RiArchiveRecord(RI_STRUCTURE,"%s%s", RI_STREAMMARKER, marker, RI_NULL);
	RicFlush(marker, 0, RI_SUSPENDRENDERING);

	RiEditBegin("option", RI_NULL);
//	RiTransformBegin();
//	RiIdentity();
	RiTranslate(camXform.tx, camXform.ty, camXform.tz);
	RiRotate(camXform.rx, 1, 0, 0);
	RiRotate(camXform.ry, 0, 1, 0);
	RiRotate(camXform.rz, 0, 0, 1);
	RiCamera("world", RI_NULL);
//	RiTransformEnd();
	RiEditEnd();

	counter++;
}

void processCallbacks() {
	RicProcessCallbacks();
}

int getProgress() {
	return RicGetProgress();
}

void shutdown() {
	char endMarker[] = "end";
	RiArchiveRecord(RI_STRUCTURE,"%s%s", RI_STREAMMARKER, endMarker, RI_NULL);
	RicFlush(endMarker, 0, RI_SUSPENDRENDERING);

	RiEditWorldEnd();
	RiEnd();
	PRManEnd();
}

}
