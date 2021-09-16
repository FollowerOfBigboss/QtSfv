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
	int end;

	void run()
	{
		uint32_t crc = 0;
		uint64_t filesize;
		uint64_t counter;
		QByteArray buffer;

		int ic = 0;
		for (auto iter : list)
		{
			QFile file(iter);
			if (!file.open(QIODevice::ReadOnly))
			{
				emit AcFileOpenFail(TID, beg + ic);
			}

			filesize = file.size();

			if (filesize < MB(50))
			{
				buffer = file.readAll();
				crc = CRC::Calculate(buffer.constData(), buffer.size(), CRC::CRC_32());
				buffer.clear();
				emit AcAppendCRC(TID, beg + ic, crc);

			}
			ic++;
		}
		
		emit AcJobDone(TID);
	}

signals:
	void AcAppendCRC(int TID, int item, uint32_t crc);
	void AcFileOpenFail(int TID, int item);
	void AcJobDone(int TID);

};

#endif