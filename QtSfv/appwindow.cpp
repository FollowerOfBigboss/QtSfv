#include "appwindow.h"
#include "sfvthread.h"


QtSfvWindow::QtSfvWindow()
{

	this->setWindowIcon(QIcon(R"(QtSfv2.png)"));

	QMenu* filemenu = menuBar()->addMenu("&File");
	QAction* openaction = filemenu->addAction("Open");
		
	QAction* closeaction = filemenu->addAction("Close");
	filemenu->addSeparator();
	QAction* exitaction = filemenu->addAction("Exit");

	QMenu* settings = menuBar()->addMenu("&Settings");
	QAction* settingsaction = settings->addAction("Settings");

	QMenu* helpmenu = menuBar()->addMenu("&Help");
	QAction* aboutaction = helpmenu->addAction("About");
	QAction* aboutqtaction = helpmenu->addAction("About Qt");

	settingsdiag = new SettingsDialog(this);
	ThreadCount = 5;
	ChunkSize = MB(1);


	connect(openaction, &QAction::triggered, this, &QtSfvWindow::OnActionOpen);
	connect(closeaction, &QAction::triggered, this, &QtSfvWindow::OnActionClose);
	connect(aboutaction, &QAction::triggered, this, [&] { QMessageBox
		::information(this, "About", "This program written by FollowerOfBigboss", QMessageBox::StandardButton::Ok, QMessageBox::StandardButton::Ok);});
	
	connect(aboutqtaction, &QAction::triggered, this, [&] { QMessageBox::aboutQt(this); });
	connect(exitaction, &QAction::triggered, this, [&] { this->close(); });

	connect(settingsaction, &QAction::triggered, this, &QtSfvWindow::OnSettingsWindowRequested);

	connect(this, &QtSfvWindow::UpdateDialogSpinValue, settingsdiag, &SettingsDialog::OnUpdateSpinValue);
	connect(settingsdiag, &SettingsDialog::UpdateThreadCountForJob, this, &QtSfvWindow::OnUpdateThreadCountForJob);

	connect(this, &QtSfvWindow::UpdateDialogChunkValue, settingsdiag, &SettingsDialog::OnUpdateChunkValue);
	connect(settingsdiag, &SettingsDialog::UpdateChunkSize, this, &QtSfvWindow::OnUpdateChunkValue);

	treeWidget = new QTreeWidget(this);
	treeWidget->setRootIsDecorated(false);
	treeWidget->setAllColumnsShowFocus(true);
	treeWidget->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);
	treeWidget->setColumnCount(4);

	QStringList headers = {"File Name", "CRC", "Calculated CRC", "Status"};
	treeWidget->setHeaderLabels(headers);

	this->setCentralWidget(treeWidget);
	treeWidget->expandAll();
	
	label.setText("Ready for an action!");

	label2.setText("Idling...");

	progressBar = new QProgressBar(this);
	statusBar()->setLayoutDirection(Qt::LayoutDirection::LeftToRight);
	statusBar()->addPermanentWidget(&label);
	statusBar()->addPermanentWidget(progressBar, 1);
	statusBar()->addPermanentWidget(&label2);

	this->resize(600, 400);
	
	connect(&timer, &QTimer::timeout, this, &QtSfvWindow::UpdateTimer);
}

bool QtSfvWindow::ParseLine(QByteArray& line)
{
	if (line.startsWith(';') == true)
		return false;

	int findfirst =  line.indexOf(' ');
	QByteArray s0 = line.mid(0, findfirst);
	QByteArray s1 = line.mid(findfirst);
	slookup.push_back(s0);
	crclookup.push_back(s1.toStdString());

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

void QtSfvWindow::CreateAWorkerThread(uint32_t ThreadID, uint32_t beg, uint32_t partcount)
{
	ThreadPool.push_back(new SfvThread);
	ThreadPool[ThreadID]->TID = ThreadID;
	ThreadPool[ThreadID]->beg = beg;
	ThreadPool[ThreadID]->ChunkSize = this->ChunkSize;

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

	for (int i = 0; i < items.size(); i++)
	{
		delete items[i];
	}
	
	items.clear();
	items.shrink_to_fit();

	crclookup.clear();
	crclookup.shrink_to_fit();

	ThreadPool.clear();
	ThreadPool.shrink_to_fit();
}

void QtSfvWindow::OnActionOpen()
{
	QString filename = QFileDialog::getOpenFileName(this, "Open Image", "", "SFV Files (*.sfv)");
	if (filename.isEmpty())
	{
		return;
	}


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

	FinishedThreadCount = 0;
	FileCount = 0;
	while (!file.atEnd()) 
	{
		QByteArray line = file.readLine();
		if (this->ParseLine(line) == true) FileCount++;
	}


	treeWidget->insertTopLevelItems(0, items);

	int PartsPerThread = FileCount / ThreadCount;
	int remain = FileCount % ThreadCount;

	int first = 1;
	label.setText("Job is still in progress... Please be patient");
	timer.start(980);
	progressBar->setRange(0, FileCount);
	progressBar->setValue(0);
	progressBar->setFormat(QString("%%p - %v/%m"));
	for (int i = 0; i < ThreadCount; i++)
	{
		CreateAWorkerThread(i, i * PartsPerThread, PartsPerThread);
		if (first == 1)
		{
			beginclock = perfclock.now();
			first = 0;
		}
	}

	// If there are remaining parts then create a thread for them
	if (remain != 0)
	{
		CreateAWorkerThread(ThreadCount, ThreadCount * PartsPerThread, remain);
	}

	slookup.clear();
	slookup.shrink_to_fit();
}

void QtSfvWindow::OnActionClose()
{
	ClearThreadPool();
	timer.stop();
	label2.setText("");
	progressBar->reset();
}

void QtSfvWindow::OnAppendCrc(uint32_t TID, uint32_t item, uint32_t crc)
{
	items[item]->setText(2, QString::number(crc, 16));	
	QString str = crclookup[item].c_str();	
	uint32_t crctocompare = str.toUInt(0, 16);

	if (crctocompare == crc)
	{
		items[item]->setText(3, "File OK");
	}
	else
	{
		items[item]->setText(3, "File Corrupted!");
	}
	progressBar->setValue(progressBar->value() + 1);
}

void QtSfvWindow::OnFileOpenFail(uint32_t TID, uint32_t item)
{
	items[item]->setText(3, "Failed to open file!");
	progressBar->setValue(progressBar->value() + 1);
}

void QtSfvWindow::OnThreadJobDone(uint32_t TID)
{
	FinishedThreadCount++;
	if (FinishedThreadCount == ThreadPool.size())
	{
		endclock =  perfclock.now();
		auto militime = std::chrono::duration_cast<std::chrono::milliseconds>(endclock - beginclock).count();
		auto sectime = std::chrono::duration_cast<std::chrono::seconds>(endclock - beginclock).count();

		label.setText("Job finished!");
		timer.stop();
	}
}

void QtSfvWindow::OnSettingsWindowRequested()
{
	emit UpdateDialogSpinValue(ThreadCount);
	emit UpdateDialogChunkValue(ChunkSize);
	settingsdiag->exec();
}

void QtSfvWindow::OnUpdateThreadCountForJob(uint32_t val)
{
	ThreadCount = val;
}

void QtSfvWindow::OnUpdateChunkValue(uint32_t val)
{
	ChunkSize = MB(val);
}

void QtSfvWindow::UpdateTimer()
{
	endclock = perfclock.now();
	auto sectime = std::chrono::duration_cast<std::chrono::seconds>(endclock - beginclock).count();
	label2.setText(QVariant(sectime).toString());
}
