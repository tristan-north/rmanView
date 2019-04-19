#include <QApplication>
#include <stdio.h>
#include <sys/stat.h>

#include "mainwindow.h"
#include "rman.h"
#include "common.h"

MainWindow *g_mainWindow = 0;

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	// Deal with arguments
	if( app.arguments().length() <= 1 ) {
		printf("Specify path to alembic as first argument.\n");
		return 0;
	}
	QByteArray alembicPath = app.arguments()[1].toAscii();
	struct stat sb;
	if( stat(alembicPath.data(), &sb) == -1 ) {
		printf("Couldn't read \"%s\".\n", alembicPath.data());
		return -1;
	}


	g_mainWindow = new MainWindow();
	g_mainWindow->show();

	g_mainWindow->viewport.startRender(alembicPath.data());

	app.exec();

	rman::shutdown();
	return 0;
}




/*  TODO:
 - Figure out why not getting any indirect in rerendering, although it works when rendering the rib through "prman scene.rib"
 - Set tesselation to happen in world space so subd geo doesn't look weird when rotating around it
 - OCIO support
 - Try telling rman to not render the lowest level LOD

 */
