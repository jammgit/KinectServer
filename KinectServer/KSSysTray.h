#pragma once

#include <QSystemTrayIcon>
#include <QMainWindow>

class KinectServer;

class KSSysTray : public QSystemTrayIcon
{
	Q_OBJECT

public:
	KSSysTray(KinectServer *parent);
	~KSSysTray();

private:
	void InitWidget();

	private slots:
	void Slot_Show();
	void Slot_Hide();
	void Slot_Quit();
	void Slot_Active(QSystemTrayIcon::ActivationReason);

private:
	KinectServer *m_pMainWindow;
};
