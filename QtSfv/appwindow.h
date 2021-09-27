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

#include "sfvthread.h"
#include "settingsdialog.h"

class QtSfvWindow : public QMainWindow
{
	Q_OBJECT

public slots:
	void OnActionOpen();
	void OnActionClose();

	void OnAppendCrc(int TID, int item, uint32_t crc);
	void OnFileOpenFail(int TID, int item);

	void OnThreadJobDone(int TID);

	void OnSettingsWindowRequested();
	void OnUpdateFilePerThread(int val);

signals:
	void UpdateDialogSpinValue(int val);

public:
	QtSfvWindow();
//	void closeEvent(QCloseEvent* closeEvent);

	bool ParseLine(QByteArray& bytearray);

	std::vector<QString> slookup;
	std::vector<std::string> crclookup;

	QString SfvPath;
	QTreeWidget* treeWidget;
	QList<QTreeWidgetItem*> items;
	std::vector<SfvThread*> ThreadPool;
	QLabel label;
	int FinishedThreadCount;
	SettingsDialog* settingsdiag;

	std::chrono::high_resolution_clock perfclock;
	std::chrono::high_resolution_clock::time_point beginclock;
	std::chrono::high_resolution_clock::time_point endclock;

	int ThreadCount;

	void CreateAWorkerThread(int ThreadID, int beg, int partcount);
	void ClearThreadPool();

};