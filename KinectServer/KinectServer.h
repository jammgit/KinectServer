#ifndef KINECTSERVER_H
#define KINECTSERVER_H

#include <QtWidgets/QMainWindow>
#include "ui_KinectServer.h"

class KinectServer : public QMainWindow
{
	Q_OBJECT

public:
	KinectServer(QWidget *parent = 0);
	~KinectServer();

private:
	Ui::KinectServerClass ui;
};

#endif // KINECTSERVER_H
