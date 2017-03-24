#include "KinectServer.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{

	QApplication a(argc, argv);
	KinectServer w;
	w.show();

	return a.exec();
}