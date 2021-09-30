#include <iostream>
#include <chrono>

#include <QMainWindow>
#include <QMenuBar>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QTreeWidget>
#include <QFileDialog>
#include <QProgressDialog>
#include <QThread>
#include <QApplication>
#include <QStatusBar>
#include <QLabel>
#include <QTimer>
#include <QProgressBar>


#include "sfvthread.h"
#include "settingsdialog.h"

class QtSfvWindow : public QMainWindow
{
	Q_OBJECT

public slots:
	void OnActionOpen();
	void OnActionClose();

	void OnAppendCrc(uint32_t TID, uint32_t item, uint32_t crc);
	void OnFileOpenFail(uint32_t TID, uint32_t item);

	void OnThreadJobDone(uint32_t TID);

	void OnSettingsWindowRequested();
	void OnUpdateThreadCountForJob(uint32_t val);
	void OnUpdateChunkValue(uint32_t val);

	void UpdateTimer();

signals:
	void UpdateDialogSpinValue(uint32_t val);
	void UpdateDialogChunkValue(uint32_t val);

public:
	QtSfvWindow();
//	void closeEvent(QCloseEvent* closeEvent);

	bool ParseLine(QByteArray& bytearray);

	std::vector<QString> slookup;
	std::vector<std::string> crclookup;
	std::vector<SfvThread*> ThreadPool;

	QString SfvPath;
	QTreeWidget* treeWidget;
	QList<QTreeWidgetItem*> items;
	QLabel label;
	QLabel label2;
	SettingsDialog* settingsdiag;
	QTimer timer;
	QProgressBar* progressBar;


	std::chrono::high_resolution_clock perfclock;
	std::chrono::high_resolution_clock::time_point beginclock;
	std::chrono::high_resolution_clock::time_point endclock;

	uint64_t FileCount;
	uint32_t ThreadCount;
	uint32_t FinishedThreadCount;
	uint32_t ChunkSize;

	void CreateAWorkerThread(uint32_t ThreadID, uint32_t beg, uint32_t partcount);
	void ClearThreadPool();

};