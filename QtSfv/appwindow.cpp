#include "appwindow.h"
#include "sfvthread.h"


QtSfvWindow::QtSfvWindow()
{
	QMenu* filemenu = menuBar()->addMenu("&File");
	QAction* openaction = filemenu->addAction("Open");
		
	QAction* closeaction = filemenu->addAction("Close");
	filemenu->addSeparator();
	QAction* exitaction = filemenu->addAction("Exit");

	QMenu* helpmenu = menuBar()->addMenu("&Help");
	QAction* aboutaction = helpmenu->addAction("About");
	QAction* aboutqtaction = helpmenu->addAction("About Qt");

	connect(openaction, &QAction::triggered, this, &QtSfvWindow::OnActionOpen);
	connect(closeaction, &QAction::triggered, this, &QtSfvWindow::OnActionClose);
	connect(aboutaction, &QAction::triggered, this, [&] { QMessageBox
		::information(this, "About", "This program written by FollowerOfBigboss", QMessageBox::StandardButton::Ok, QMessageBox::StandardButton::Ok);});
	
	connect(aboutqtaction, &QAction::triggered, this, [&] { QMessageBox::aboutQt(this); });
	connect(exitaction, &QAction::triggered, this, [&] { this->close(); });

	treeWidget = new QTreeWidget(this);
	treeWidget->setRootIsDecorated(false);
	treeWidget->setAllColumnsShowFocus(true);
	treeWidget->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);
	treeWidget->setColumnCount(4);

	QStringList headers = {"File Name","CRC","Calculated CRC","Status"};
	treeWidget->setHeaderLabels(headers);

	this->setCentralWidget(treeWidget);
	treeWidget->expandAll();

}

void QtSfvWindow::closeEvent(QCloseEvent* closeEvent)
{

}

bool QtSfvWindow::ParseLine(QByteArray& line)
{
	if (line.startsWith(';') == true)
		return false;

	QByteArrayList sline = line.split(' ');
	slookup.push_back(sline[0]);

	// Remove newline character
	sline[1].remove(sline[1].indexOf('\n'), 1);

	QTreeWidgetItem* item = new QTreeWidgetItem();
	item->setText(0, sline[0]);
	item->setText(1, sline[1]);
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
	if (CountOfThreadsInThreadPool > 0)
	{
		CountOfThreadsInThreadPool--;
		return;
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

	treeWidget->clear();
	items.clear();

	int linecount = 0;
	while (!file.atEnd()) 
	{
		QByteArray line = file.readLine();
		if (this->ParseLine(line) == true) linecount++;
	}

	treeWidget->insertTopLevelItems(0, items);

	int parts = linecount / 5;
	int remains = 0;
	int isitremains = linecount % 5;
	if (isitremains > 0)
		remains = isitremains;

	int last = 0;
	int TIDCounter = 0;

	if (remains != 0)
	{		
		for (int i = 0; i < parts; i++)
		{
			CreateAWorkerThread(TIDCounter, last, 5);
			last += 5;
			TIDCounter++;
		}


		CreateAWorkerThread(TIDCounter, last, remains);
		CountOfThreadsInThreadPool = TIDCounter;
	}
	else
	{
		for (int i = 0; i < parts; i++)
		{
			CreateAWorkerThread(TIDCounter, last, 5);
			last += 5;
			TIDCounter++;
		}
		CountOfThreadsInThreadPool = TIDCounter - 1;
	}

	slookup.clear();
	slookup.shrink_to_fit();
}

void QtSfvWindow::OnActionClose()
{

}

void QtSfvWindow::OnAppendCrc(int TID, int item, uint32_t crc)
{
	items[item]->setText(2, QString::number(crc, 16));
}

void QtSfvWindow::OnFileOpenFail(int TID, int item)
{
	items[item]->setText(3, "Failed to open file!");
}

void QtSfvWindow::OnThreadJobDone(int TID)
{
	ThreadPool[TID]->exit();
	
	if (ThreadPool[TID]->isFinished() != true)
	{
		bool wait = ThreadPool[TID]->wait();
		if (wait == true)
		{
			delete ThreadPool[TID];
		}
	}
	else
	{
		delete ThreadPool[TID];
	}

	ClearThreadPool();
}
