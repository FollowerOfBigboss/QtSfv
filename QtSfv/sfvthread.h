#ifndef _SFV_THREAD
#define _SFV_THREAD

#include <QThread>
#include "crc32/CRC.h"

#define MB(x)   ((size_t) (x) << 20)


class SfvThread : public QThread
{
	Q_OBJECT
public:
	uint32_t TID;
	uint32_t ChunkSize;
	QStringList list;
	uint32_t beg;

	void run();

signals:
	void AcAppendCRC(uint32_t TID, uint32_t item, uint32_t crc);
	void AcFileOpenFail(uint32_t TID, uint32_t item);
	void AcJobDone(uint32_t TID);
};

#endif