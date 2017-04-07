#ifndef KINECTSERVER_H
#define KINECTSERVER_H

#include <QtWidgets/QMainWindow>
#include <QPoint>
#include "ui_KinectServer.h"
#include "KSSysTray.h"
#include "../KSLogService/IKSLogClient.h"
#include "../KSService/IKSClient.h"
#include "../KSUtils/NetworkMonitor.h"
#include <list>
#include <boost\shared_ptr.hpp>

class IKSService;
typedef boost::shared_ptr<IKSService> IKSServicePtr;

class QMouseEvent;

class KinectServer
	: public QMainWindow
	, public IKSLogClient
	, public IKSClient
	, public INetworkClient
{
	Q_OBJECT

public:
	KinectServer(QWidget *parent = 0);
	~KinectServer();

	void mouseMoveEvent(QMouseEvent *event) override;
	void mousePressEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
	void paintEvent(QPaintEvent *) override;

protected:
	void InitWidget();
	void InitStyle();
	void InitConnect();

signals:
	void NetworkInfo(double quality, std::string ssid, std::string ip) override;
	public slots:
	void slot_NetworkInfo(double quality, std::string ssid, std::string ip);

signals:
	void DrawLine(std::vector<POINT> points) override;
	public slots:
	void slot_DrawLine(std::vector<POINT> points);

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
	void slot_anim_finish();

private:
	Ui::KinectServerClass ui;
	NetworkMonitorPtr m_NetMonitor;
	IKSServicePtr m_ServicePtr;
	QPoint m_position;

	KSSysTray m_SysTray;
	int w, h;
private:
	std::list<std::vector<POINT>> m_pvec;

};

#endif // KINECTSERVER_H
