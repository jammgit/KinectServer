#include "KinectServer.h"
#include "../KSLogService/KSLogService.h"
#include <QMouseEvent>
#include <QDesktopWidget>
#include <QTextEdit>
#include <QTextCodec>
#include <QScrollbar>
#include <QDateTime>
#include <QListWidget>
#include <QListWidgetItem>

KinectServer::KinectServer(QWidget *parent)
	: QMainWindow(parent)
	, m_servicePtr(KSService::GetInstance())
{
	ui.setupUi(this);

	this->InitWidget();
	this->InitConnect();

	// sPtr是第一个shared_ptr，保证服务在start时share_from_this不出错
	m_servicePtr->Thread::Start();
}

KinectServer::~KinectServer()
{
}

void KinectServer::mouseMoveEvent(QMouseEvent *event)
{
	if (event->buttons() & Qt::LeftButton)
	{
		move(event->globalPos() - m_position);
		event->accept();
	}
}

void KinectServer::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		m_position = event->globalPos() - frameGeometry().topLeft();
		event->accept();
	}
}

void KinectServer::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->type() == QEvent::MouseButtonRelease)
	{
		QRect deskRect = QApplication::desktop()->availableGeometry();
		QRect rect = this->geometry();
		this->move(rect.left(), deskRect.height() - rect.height());
	}
}

void KinectServer::InitWidget()
{
	this->setAttribute(Qt::WA_TranslucentBackground);
	this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);

	KSLogService::GetInstance()->RegisterClient(this);
	qRegisterMetaType<std::string>("std::string");

	ui.textEdit->setAttribute(Qt::WA_TranslucentBackground);

	QRect deskRect = QApplication::desktop()->availableGeometry();
	QRect rect = this->geometry();
	this->move((deskRect.width() - rect.width()) / 2, deskRect.height() - rect.height());
}

void KinectServer::InitConnect()
{
	QObject::connect(this, SIGNAL(OutputString(const char*)), this, SLOT(slot_OutputString(const char*)));
	QObject::connect(this, SIGNAL(OutputString(const std::string&)), this, SLOT(slot_OutputString(const std::string&)));
	QObject::connect(this, SIGNAL(OutputDevice(const std::string&, bool)), this, SLOT(slot_OutputDevice(const std::string&, bool)));
	QObject::connect(this, SIGNAL(OutputClient(const std::string&, bool)), this, SLOT(slot_OutputClient(const std::string&, bool)));
}

void KinectServer::slot_OutputString(const char* msg)
{
	ui.textEdit->setText(
		ui.textEdit->toPlainText()
		+ QString("[") + QDateTime::currentDateTime().toString() + "]"
		+ QString::fromLocal8Bit(msg));

	ui.textEdit->moveCursor(QTextCursor::End);
}

void KinectServer::slot_OutputString(const std::string& msg)
{
	this->slot_OutputString(msg.c_str());
}

void KinectServer::slot_OutputDevice(const std::string& dev, bool connect)
{
	if (connect)
	{
		ui.listWidget->addItem(QString::fromStdString(dev));
	}
	else
	{
		for (int i = 0; i < ui.listWidget->count(); ++i)
		{
			QListWidgetItem * item = ui.listWidget->item(i);
			if (item->text().compare(QString::fromStdString(dev)) == 0)
			{
				ui.listWidget->removeItemWidget(item);
				delete item;
				break;
			}
		}
	}
}

void KinectServer::slot_OutputClient(const std::string& addr, bool connect)
{
	if (connect)
	{
		ui.listWidget_2->addItem(QString::fromStdString(addr));
	}
	else
	{
		for (int i = 0; i < ui.listWidget_2->count(); ++i)
		{
			QListWidgetItem * item = ui.listWidget_2->item(i);
			if (item->text().compare(QString::fromStdString(addr)) == 0)
			{
				ui.listWidget_2->removeItemWidget(item);
				delete item;
				break;
			}
		}
	}
}

void KinectServer::slot_close()
{
	exit(0);
}