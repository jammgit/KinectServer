#ifndef KINECTSERVER_H
#define KINECTSERVER_H

#include <QtWidgets/QMainWindow>
#include <QPoint>
#include "ui_KinectServer.h"
#include "../KSService/KSService.h"
#include "../KSLogService/IKSLogClient.h"

class QMouseEvent;

class KinectServer
	: public QMainWindow
	, public IKSLogClient
{
	Q_OBJECT

public:
	KinectServer(QWidget *parent = 0);
	~KinectServer();

	void mouseMoveEvent(QMouseEvent *event) override;
	void mousePressEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;

protected:
	void InitWidget();
	void InitConnect();

signals:
	void OutputString(const char* msg) override;
	void OutputString(const std::string& msg) override;
	void OutputDevice(const std::string& dev, bool connect) override;
	void OutputClient(const std::string& addr, bool connect) override;
	public slots:
	void slot_OutputString(const char* msg);
	void slot_OutputString(const std::string& msg);
	void slot_OutputDevice(const std::string& dev, bool connect);
	void slot_OutputClient(const std::string& addr, bool connect);

	private slots:
	void slot_close();

private:
	Ui::KinectServerClass ui;

	KSServicePtr m_servicePtr;
	QPoint m_position;
};

#endif // KINECTSERVER_H
