#include "appwindow.h"
#include "sfvthread.h"


QtSfvWindow::QtSfvWindow()
{
	settingsdiag = nullptr;
	FilePerThread = 5;
	settingsdiag = new SettingsDialog(this);

	this->setWindowIcon(QIcon(R"(QtSfv2.png)"));

	QMenu* filemenu = menuBar()->addMenu("&File");
	QAction* openaction = filemenu->addAction("Open");
		
	QAction* closeaction = filemenu->addAction("Close");
	filemenu->addSeparator();
	QAction* exitaction = filemenu->addAction("Exit");

	QMenu* helpmenu = menuBar()->addMenu("&Help");
	QAction* aboutaction = helpmenu->addAction("About");
	QAction* aboutqtaction = helpmenu->addAction("About Qt");

	QMenu* settings = menuBar()->addMenu("&Settings");
	QAction* settingsaction = settings->addAction("Settings");


	connect(openaction, &QAction::triggered, this, &QtSfvWindow::OnActionOpen);
	connect(closeaction, &QAction::triggered, this, &QtSfvWindow::OnActionClose);
	connect(aboutaction, &QAction::triggered, this, [&] { QMessageBox
		::information(this, "About", "This program written by FollowerOfBigboss", QMessageBox::StandardButton::Ok, QMessageBox::StandardButton::Ok);});
	
	connect(aboutqtaction, &QAction::triggered, this, [&] { QMessageBox::aboutQt(this); });
	connect(exitaction, &QAction::triggered, this, [&] { this->close(); });

	connect(settingsaction, &QAction::triggered, this, &QtSfvWindow::OnSettingsWindowRequested);

	connect(this, &QtSfvWindow::UpdateDialogSpinValue, settingsdiag, &SettingsDialog::OnUpdateSpinValue);
	connect(settingsdiag, &SettingsDialog::UpdateFilePerThread, this, &QtSfvWindow::OnUpdateFilePerThread);

	treeWidget = new QTreeWidget(this);
	treeWidget->setRootIsDecorated(false);
	treeWidget->setAllColumnsShowFocus(true);
	treeWidget->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);
	treeWidget->setColumnCount(4);

	QStringList headers = {"File Name","CRC","Calculated CRC","Status"};
	treeWidget->setHeaderLabels(headers);

	this->setCentralWidget(treeWidget);
	treeWidget->expandAll();
	
	label.setText("Ready!");
	statusBar()->addWidget(&label);
	this->resize(600, 400);
}


bool QtSfvWindow::ParseLine(QByteArray& line)
{
	if (line.startsWith(';') == true)
		return false;

	int findfirst =  line.indexOf(' ');
	QByteArray s0 = line.mid(0, findfirst);
	QByteArray s1 = line.mid(findfirst);
	slookup.push_back(s0);

	int chsize = s1.size();
	for (int i = 0; i < chsize; i++)
	{
		if ( s1[i] == ' ' || s1[i] == '\x00' || s1[i] == '\n' || s1[i] == '\r' )
		{
			s1.remove(i, 1);
			chsize = s1.size();
			i = -1;
		}
	}

	QTreeWidgetItem* item = new QTreeWidgetItem();
	item->setText(0, s0);
	item->setText(1, s1);
	items.append(item);

	return true;
}

void QtSfvWindow::CreateAWorkerThread(int ThreadID, int beg, int partcount)
{
	ThreadPool.push_back(new SfvThread);
	ThreadPool[ThreadID]->TID = ThreadID;
	ThreadPool[ThreadID]->beg = beg;

	for (int x = 0; x < partcount; x++)
	{
		ThreadPool[ThreadID]->list.append(QDir::cleanPath(SfvPath + QDir::separator() + slookup[beg + x]));
	}

	connect(ThreadPool[ThreadID], &SfvThread::AcAppendCRC, this, &QtSfvWindow::OnAppendCrc);
	connect(ThreadPool[ThreadID], &SfvThread::AcFileOpenFail, this, &QtSfvWindow::OnFileOpenFail);
	connect(ThreadPool[ThreadID], &SfvThread::AcJobDone, this, &QtSfvWindow::OnThreadJobDone);

	ThreadPool[ThreadID]->start();
}

void QtSfvWindow::ClearThreadPool()
{
	for (int i = 0; i < ThreadPool.size(); i++)
	{
			ThreadPool[i]->requestInterruption();
			ThreadPool[i]->wait();
			delete ThreadPool[i];
	}

	ThreadPool.clear();
	ThreadPool.shrink_to_fit();
}

void QtSfvWindow::OnActionOpen()
{
	QString filename = QFileDialog::getOpenFileName(this, "Open Image", "", "SFV Files (*.sfv)");

	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly))
	{
		QMessageBox::critical(this, "Error", "File couldn't opened!");
		return;
	}

	QDir abDir = QFileInfo(filename).absoluteDir();
	QString abPath = abDir.absolutePath();
	SfvPath = abPath;

	ClearThreadPool();

	treeWidget->clear();
	items.clear();

	int linecount = 0;
	while (!file.atEnd()) 
	{
		QByteArray line = file.readLine();
		if (this->ParseLine(line) == true) linecount++;
	}

	treeWidget->insertTopLevelItems(0, items);

	int parts = linecount / FilePerThread;
	int remains = 0;
	int isitremains = linecount % FilePerThread;
	if (isitremains > 0)
		remains = isitremains;

	int last = 0;
	int TIDCounter = 0;

	if (remains != 0)
	{		
		for (int i = 0; i < parts; i++)
		{
			CreateAWorkerThread(TIDCounter, last, FilePerThread);
			last += FilePerThread;
			TIDCounter++;
		}


		CreateAWorkerThread(TIDCounter, last, remains);
		CountOfThreadsInThreadPool = TIDCounter;
	}
	else
	{
		for (int i = 0; i < parts; i++)
		{
			CreateAWorkerThread(TIDCounter, last, FilePerThread);
			last += FilePerThread;
			TIDCounter++;
		}
		CountOfThreadsInThreadPool = TIDCounter - 1;
	}

	slookup.clear();
	slookup.shrink_to_fit();
}

void QtSfvWindow::OnActionClose()
{
	ClearThreadPool();
}

void QtSfvWindow::OnAppendCrc(int TID, int item, uint32_t crc)
{
	items[item]->setText(2, QString::number(crc, 16));

	QString str = items[item]->text(1);
	uint32_t crctocompare = str.toUInt(0, 16);

	if (crctocompare == crc)
	{
		items[item]->setText(3, "File OK");
	}
	else
	{
		items[item]->setText(3, "File Corrupted!");
	}

}

void QtSfvWindow::OnFileOpenFail(int TID, int item)
{
	items[item]->setText(3, "Failed to open file!");
}

void QtSfvWindow::OnThreadJobDone(int TID)
{
	printf("Thread %i job is done!\n", TID);
}

void QtSfvWindow::OnSettingsWindowRequested()
{
	emit UpdateDialogSpinValue(FilePerThread);
	settingsdiag->exec();
}

void QtSfvWindow::OnUpdateFilePerThread(int val)
{
	FilePerThread = val;
}
