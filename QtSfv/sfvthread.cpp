#include "sfvthread.h"

#include <QFile>

void SfvThread::run()
{
	uint32_t crc = 0;
	uint64_t filesize;
	uint64_t counter;
	QByteArray buffer;

	int ic = 0;
	for (auto iter : list)
	{
		if (this->isInterruptionRequested())
			return;

		QFile file(iter);
		bool bFileOpened = file.open(QIODevice::ReadOnly);

		if (bFileOpened != true)
		{
			emit AcFileOpenFail(TID, beg + ic);
		}

		filesize = file.size();
		if (bFileOpened == true)
		{
			if (filesize < MB(10))
			{
				if (this->isInterruptionRequested())
					return;

				buffer = file.readAll();
				crc = CRC::Calculate(buffer.constData(), buffer.size(), CRC::CRC_32());
				buffer.clear();
				emit AcAppendCRC(TID, beg + ic, crc);

			}
			else
			{
				if (this->isInterruptionRequested())
					return;

				int counter = filesize;
				crc = 0;
				while (counter > 0)
				{
					if (this->isInterruptionRequested())
						return;

					if (counter > MB(1))
					{
						buffer = file.read(MB(1));
						counter -= MB(1);
						crc = CRC::Calculate(buffer.constData(), buffer.size(), CRC::CRC_32(), crc);
						buffer.clear();
					}
					else
					{
						buffer = file.read(counter);
						crc = CRC::Calculate(buffer.constData(), buffer.size(), CRC::CRC_32(), crc);
						buffer.clear();
						counter -= counter;
					}
				}
				emit AcAppendCRC(TID, beg + ic, crc);
			}
		}
		ic++;
	}

	emit AcJobDone(TID);
}
