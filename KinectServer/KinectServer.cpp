#include "KinectServer.h"
#include "../KSLogService/KSLogService.h"
#include "../KSService/KSService.h"
#include "../KSUtils/SysConfig.h"
#include <QMouseEvent>
#include <QDesktopWidget>
#include <QTextEdit>
#include <QTextCodec>
#include <QScrollbar>
#include <QDateTime>
#include <QListWidget>
#include <QListWidgetItem>
#include <QApplication>
#include <QPainter>
#include <QPen>
#include <QSplitter>
#include <QPropertyAnimation>

KinectServer::KinectServer(QWidget *parent)
	: QMainWindow(parent)
	, m_ServicePtr(KSService::GetInstance())
	, m_SysTray(this)
	// m_ServicePtr是第一个shared_ptr，保证服务在start时share_from_this不出错
{
	ui.setupUi(this);

	this->InitWidget();
	this->InitStyle();
	this->InitConnect();


	SysConfig::InitConfig();
	KSLogService::GetInstance()->RegisterClient(this);
	//m_ServicePtr->RegisterClient(this);
	m_ServicePtr->Start();
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
		//QRect deskRect = QApplication::desktop()->availableGeometry();
		//QRect rect = this->geometry();
		//this->move(rect.left(), deskRect.height() - rect.height());
	}
}

void KinectServer::paintEvent(QPaintEvent *)
{
	if (m_pvec.size() > 0)
	{
		QPainter painter(this);
		QPen pen[2] =
			{ QPen(QColor(255,0,0), 5) , QPen(QColor(0,0,255), 5) };
		
		
		std::vector<POINT> ps = m_pvec.front();
		for (int k = 0; k < ps.size() / 24; ++k)
		{
			painter.setPen(pen[k]);
			QPointF pos[5];
			for (int i = k * 24 + 0; i < k * 24 + 4; ++i)
			{
				pos[i].setX(ps[i].x);
				pos[i].setY(ps[i].y);
			}
			painter.drawPolyline(pos, 4);
			for (int i = k * 24 + 0; i < k * 24 + 5; ++i)
			{
				pos[i].setX(ps[i + 4].x);
				pos[i].setY(ps[i + 4].y);
			}
			painter.drawPolyline(pos, 5);
			for (int i = k * 24 + 0; i < k * 24 + 5; ++i)
			{
				pos[i].setX(ps[i + 9].x);
				pos[i].setY(ps[i + 9].y);
			}
			painter.drawPolyline(pos, 5);
			for (int i = k * 24 + 0; i < k * 24 + 5; ++i)
			{
				pos[i].setX(ps[i + 14].x);
				pos[i].setY(ps[i + 14].y);
			}
			painter.drawPolyline(pos, 5);

			for (int i = k * 24 + 0; i < k * 24 + 5; ++i)
			{
				pos[i].setX(ps[i + 19].x);
				pos[i].setY(ps[i + 19].y);
			}
			painter.drawPolyline(pos, 5);
		}
		
		m_pvec.pop_front();
	}
}

void KinectServer::InitWidget()
{
	this->setAttribute(Qt::WA_TranslucentBackground);
	this->setWindowFlags(this->windowFlags() | Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint/* | Qt::WindowStaysOnTopHint*/);
	
	qRegisterMetaType<std::string>("std::string");
	qRegisterMetaType<std::vector<POINT>>("std::vector<POINT>");

	//QRect deskRect = QApplication::desktop()->availableGeometry();
	//QRect rect = this->geometry();
	//this->move((deskRect.width() - rect.width()) / 2, deskRect.height() - rect.height());

}

void KinectServer::InitStyle()
{
	ui.listWidget->verticalScrollBar()
		->setStyleSheet("\
		QScrollBar:vertical{\
			border-right: 1px solid #ffffff;\
			background: #32CC99;\
			width: 5px;\
			margin: 5px 0 5px 0;\
			background:rgb(255,255,255);\
		}\
		QScrollBar::handle:vertical{\
			background-color: qlineargradient(spread:reflect, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(255, 255, 255, 255),\
				 stop:0.502825 rgba(0, 170, 255, 255), stop:1 rgba(255, 255, 255, 255));\
			min-height: 10px;\
		}\
		QScrollBar::add-line:vertical{\
			height:10px;\
			background: rgb(255,255,255);\
			subcontrol-position: bottom;\
			subcontrol-origin: margin;\
		}\
		QScrollBar::sub-line:vertical{\
			height:10px;\
			background: rgb(255,255,255);\
			subcontrol-position: top;\
			subcontrol-origin: margin;\
		}\
		QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical{\
			width: 0px;\
			height: 0px;\
			background: white;\
		}\
		QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical{\
			background: none;\
		}\
		");
	ui.listWidget_2->verticalScrollBar()
		->setStyleSheet("\
		QScrollBar:vertical{\
			border-right: 1px solid #ffffff;\
			background: #32CC99;\
			width: 5px;\
			margin: 5px 0 5px 0;\
			background:rgb(255,255,255);\
		}\
		QScrollBar::handle:vertical{\
			background-color: qlineargradient(spread:reflect, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(255, 255, 255, 255),\
				 stop:0.502825 rgba(0, 170, 255, 255), stop:1 rgba(255, 255, 255, 255));\
			min-height: 10px;\
		}\
		QScrollBar::add-line:vertical{\
			height:10px;\
			background: rgb(255,255,255);\
			subcontrol-position: bottom;\
			subcontrol-origin: margin;\
		}\
		QScrollBar::sub-line:vertical{\
			height:10px;\
			background: rgb(255,255,255);\
			subcontrol-position: top;\
			subcontrol-origin: margin;\
		}\
		QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical{\
			width: 0px;\
			height: 0px;\
			background: white;\
		}\
		QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical{\
			background: none;\
		}\
		");

	ui.listWidget_2->horizontalScrollBar()
		->setStyleSheet("\
		QScrollBar:horizontal{\
			border-bottom: 1px solid #ffffff;\
			background: white;\
			height: 5px;\
			margin: 0px 5px 0 5px;\
		}\
		QScrollBar::handle:horizontal{\
			background-color:qlineargradient(spread:reflect, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(255, 255, 255, 255),\
				 stop:0.502825 rgba(0, 170, 255, 255), stop:1 rgba(255, 255, 255, 255));\
			min-width: 10px;\
		}\
		QScrollBar::add-line:horizontal{\
			background: white;\
			width:10px;\
			subcontrol-position: right;\
			subcontrol-origin: margin;\
		}\
			\
		QScrollBar::sub-line:horizontal{\
			background: white;\
			width:10px;\
			subcontrol-position: left;\
			subcontrol-origin: margin;\
		}");

	ui.listWidget->horizontalScrollBar()
		->setStyleSheet("\
		QScrollBar:horizontal{\
			border-bottom: 1px solid #ffffff;\
			background: white;\
			height: 5px;\
			margin: 0px 5px 0 5px;\
		}\
		QScrollBar::handle:horizontal{\
			background-color:qlineargradient(spread:reflect, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(255, 255, 255, 255),\
				 stop:0.502825 rgba(0, 170, 255, 255), stop:1 rgba(255, 255, 255, 255));\
			min-width: 10px;\
		}\
		QScrollBar::add-line:horizontal{\
			background: white;\
			width:10px;\
			subcontrol-position: right;\
			subcontrol-origin: margin;\
		}\
			\
		QScrollBar::sub-line:horizontal{\
			background: white;\
			width:10px;\
			subcontrol-position: left;\
			subcontrol-origin: margin;\
		}");
	
	ui.textEdit->verticalScrollBar()
		->setStyleSheet("\
		QScrollBar:vertical{\
			border-right: 1px solid #ffffff;\
			background: #32CC99;\
			width: 5px;\
			margin: 5px 0 5px 0;\
			background:rgb(255,255,255);\
		}\
		QScrollBar::handle:vertical{\
			background-color: qlineargradient(spread:reflect, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(255, 255, 255, 255),\
				 stop:0.502825 rgba(0, 170, 255, 255), stop:1 rgba(255, 255, 255, 255));\
			min-height: 10px;\
		}\
		QScrollBar::add-line:vertical{\
			height:10px;\
			background: rgb(255,255,255);\
			subcontrol-position: bottom;\
			subcontrol-origin: margin;\
		}\
		QScrollBar::sub-line:vertical{\
			height:10px;\
			background: rgb(255,255,255);\
			subcontrol-position: top;\
			subcontrol-origin: margin;\
		}\
		QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical{\
			width: 0px;\
			height: 0px;\
			background: white;\
		}\
		QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical{\
			background: none;\
		}\
		");
}

void KinectServer::InitConnect()
{
	QObject::connect(this, SIGNAL(DrawLine(std::vector<POINT>)), 
		this, SLOT(slot_DrawLine(std::vector<POINT>)));

	QObject::connect(this, SIGNAL(OutputString(const char*)), this, SLOT(slot_OutputString(const char*)));
	QObject::connect(this, SIGNAL(OutputString(const std::string&)), this, SLOT(slot_OutputString(const std::string&)));
	QObject::connect(this, SIGNAL(OutputDevice(const std::string&, bool)), this, SLOT(slot_OutputDevice(const std::string&, bool)));
	QObject::connect(this, SIGNAL(OutputClient(const std::string&, bool)), this, SLOT(slot_OutputClient(const std::string&, bool)));
}

void KinectServer::slot_DrawLine(std::vector<POINT> points)
{
	m_pvec.push_back(points);

	update();
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
	static QPropertyAnimation *animation = NULL;
	if (animation) delete animation;
	animation = new QPropertyAnimation(this, "geometry");
	animation->setDuration(400);
	w = this->width();
	h = this->height();

	QRect deskRect = QApplication::desktop()->availableGeometry();

	animation->setStartValue(QRect(this->pos().x(), this->pos().y(), this->width(), this->height()));

	animation->setKeyValueAt(0.3, QRect(this->pos().x()*0.7, this->pos().y()*0.7, this->width()*0.7, this->height()*0.7));
	
	animation->setKeyValueAt(0.6, QRect(this->pos().x(), this->pos().y(), this->width()*0.4, this->height()*0.4));

	animation->setKeyValueAt(0.9,
		QRect(deskRect.width() - this->pos().x()/2,
			deskRect.height() - this->pos().y()/2, this->width()*0.1, this->height()*0.1));

	animation->setEndValue(QRect(deskRect.width(), deskRect.height(),0,0));
	
	QObject::connect(animation, SIGNAL(finished()), this, SLOT(slot_anim_finish()));
	animation->start();
}

void KinectServer::slot_anim_finish()
{
	this->hide();
	this->resize(w, h);
	QRect deskRect = QApplication::desktop()->availableGeometry();
	QRect rect = this->geometry();
	this->move((deskRect.width() - rect.width()) / 2, (deskRect.height() - rect.height())/2);
}