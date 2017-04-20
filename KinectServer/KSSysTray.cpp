#include "KSSysTray.h"
#include "KinectServer.h"
#include <QWidget>
#include <QAction>
#include <QMenu>

KSSysTray::KSSysTray(KinectServer *parent)
	: QSystemTrayIcon(parent)
	, m_pMainWindow(parent)
{
	this->InitWidget();
}

KSSysTray::~KSSysTray()
{
}

void KSSysTray::InitWidget()
{
	QAction *show = new QAction(QString::fromLocal8Bit("显示窗口"));
	QAction *hide = new QAction(QString::fromLocal8Bit("隐藏窗口"));
	QAction *quit = new QAction(QString::fromLocal8Bit("退出"));
	show->setIcon(QIcon(":/KinectServer/Resources/maxmize.png"));
	hide->setIcon(QIcon(":/KinectServer/Resources/minimize.png"));
	quit->setIcon(QIcon(":/KinectServer/Resources/close_2.png"));

	QObject::connect(show, SIGNAL(triggered(bool)), this, SLOT(Slot_Show()));
	QObject::connect(hide, SIGNAL(triggered(bool)), this, SLOT(Slot_Hide()));
	QObject::connect(quit, SIGNAL(triggered(bool)), this, SLOT(Slot_Quit()));
	QObject::connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
		this, SLOT(Slot_Active(QSystemTrayIcon::ActivationReason)));

	QMenu *menu = new QMenu();
	menu->addAction(show);
	menu->addAction(hide);
	menu->addAction(quit);

	this->setContextMenu(menu);
	this->setIcon(QIcon(":/KinectServer/Resources/icon.png"));
	this->setToolTip("KinectServer");
	this->show();
}


void KSSysTray::Slot_Show()
{
	m_pMainWindow->show();
}

void KSSysTray::Slot_Hide()
{
	m_pMainWindow->hide();
}

void KSSysTray::Slot_Quit()
{
	exit(0);
}

void KSSysTray::Slot_Active(QSystemTrayIcon::ActivationReason reason)
{
	if (reason == QSystemTrayIcon::Trigger
		|| reason == QSystemTrayIcon::DoubleClick)
		m_pMainWindow->show();
}
