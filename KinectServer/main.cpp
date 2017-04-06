#include "KinectServer.h"
#include <QtWidgets/QApplication>
#include <mutex>

#pragma data_seg("Shared")
std::mutex m_onMutex;
int on = 0;
#pragma data_seg()

#pragma comment(linker,"/SECTION:Shared,RWS")

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	{
		std::lock_guard<std::mutex> lock(m_onMutex);
		if (on != 0)
			return 0;
		on = 1;
	}

	KinectServer w;
	w.show();

	int ret = a.exec();

	{
		std::lock_guard<std::mutex> lock(m_onMutex);
		on = 0;
	}

	return ret;
}