#include "sfvthread.h"

#include <QFile>

#define CheckForInterrupt if (this->isInterruptionRequested()) return



void SfvThread::run()
{
	uint32_t crc = 0;
	uint64_t filesize;
	uint64_t counter;
	QByteArray buffer;

	uint32_t ic = 0;
	for (auto iter : list)
	{
		CheckForInterrupt;

		QFile file(iter);
		bool bFileOpened = file.open(QIODevice::ReadOnly);

		if (bFileOpened != true)
		{
			emit AcFileOpenFail(TID, beg + ic);
		}

		filesize = file.size();
		if (bFileOpened == true)
		{
			if (filesize < this->ChunkSize)
			{
				CheckForInterrupt;

				buffer = file.readAll();
				crc = CRC32::Calculate(buffer.constData(), buffer.size());
				buffer.clear();
				buffer.shrink_to_fit();
				emit AcAppendCRC(TID, beg + ic, crc);

			}
			else
			{
				CheckForInterrupt;

				uint64_t counter = filesize;
				crc = 0;
				while (counter > 0)
				{
					CheckForInterrupt;

					if (counter > this->ChunkSize)
					{
						buffer = file.read(this->ChunkSize);
						counter -= this->ChunkSize;
						crc = CRC32::Calculate(buffer.constData(), buffer.size(), crc);
						buffer.clear();
						buffer.shrink_to_fit();
					}
					else
					{
						buffer = file.read(counter);
						crc = CRC32::Calculate(buffer.constData(), buffer.size(), crc);
						buffer.clear();
						buffer.shrink_to_fit();
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
