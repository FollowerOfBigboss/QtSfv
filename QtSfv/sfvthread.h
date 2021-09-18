#ifndef _SFV_THREAD
#define _SFV_THREAD

#include <QThread>
#include "crc32/CRC.h"

#define MB(x)   ((size_t) (x) << 20)


class SfvThread : public QThread
{
	Q_OBJECT
public:
	int TID;
	QStringList list;
	int beg;

	void run();

signals:
	void AcAppendCRC(int TID, int item, uint32_t crc);
	void AcFileOpenFail(int TID, int item);
	void AcJobDone(int TID);
};

#endif