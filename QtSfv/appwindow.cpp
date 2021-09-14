#include "appwindow.h"
#include "sfvthread.h"


QtSfvWindow::QtSfvWindow()
{
	QMenu* filemenu = menuBar()->addMenu("&File");
	QAction* openaction = filemenu->addAction("Open");
	
	QAction* playaction = filemenu->addAction("Play");
	QAction* pauseaction = filemenu->addAction("Pause");
	
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

void QtSfvWindow::OnActionOpen()
{
	QString filename = QFileDialog::getOpenFileName(this, "Open Image", "", "SFV Files (*.sfv)");

	qDebug() << filename;
	file = new QFile(filename);
	if (!file->open(QIODevice::ReadOnly))
	{
		QMessageBox::critical(this, "Error", "File couldn't opened!");
		return;
	}

	QDir abDir = QFileInfo(filename).absoluteDir();
	QString abPath = abDir.absolutePath();
	SfvPath = abPath;

	qDebug() << abPath;


	treeWidget->clear();
	items.clear();

	int linecount = 0;
	while (!file->atEnd()) 
	{
		QByteArray line = file->readLine();
		if (this->ParseLine(line) == true) linecount++;
	}

	treeWidget->insertTopLevelItems(0, items);

	int parts = linecount / 5;
	int remains = 0;
	int isitremains = linecount % 5;
	if (isitremains > 0)
		remains = isitremains;

	SfvThread* sthread;

	int last = 0;
	int TIDCounter = 0;

	if (remains != 0)
	{
		sthread = new SfvThread[parts + 1];
		
		for (int i = 0; i < parts; i++)
		{
			sthread[i].TID = i;
			sthread[i].beg = last;
			for (int x = 0; x < 5; x++)
			{
					sthread[i].list.append(QDir::cleanPath(SfvPath + QDir::separator() + slookup[last]));
					last++;
			}
#ifdef _DEBUG
			std::cout << "Thread " << TIDCounter << "\n";
			for (auto titer : sthread[i].list)
			{
				std::cout << titer.toStdString() << "\n";
			}
			std::cout << "Thread " << TIDCounter << "End\n";
#endif

			sthread[i].end = last;
			connect(&sthread[i], &SfvThread::InsertCRC, this, &QtSfvWindow::OnInsertCrc);
			sthread[i].start();
			TIDCounter++;
		}

		sthread[parts].TID = TIDCounter;
		sthread[parts].beg = last;
		for (int i = 0; i < remains; i++)
		{
			sthread[parts].list.append(slookup[last]);
			last++;
		}
		sthread[parts].end = last;
		connect(&sthread[parts], &SfvThread::InsertCRC, this, &QtSfvWindow::OnInsertCrc);
		sthread[parts].start();

#ifdef _DEBUG
		std::cout << "Thread " << TIDCounter << "\n";
		for (auto titer : sthread[parts].list)
		{
			std::cout << titer.toStdString() << "\n";
		}
		std::cout << "Thread " << TIDCounter << "End\n";
#endif

//			for (int x = 0; x < parts; x++)
//			{
//				qDebug() << "Part " << x << " Begin";
//
//				for (int a = 0; a < 5; a++)
//				{
//					qDebug() << slookup[last];
//					sthread[i].list.append(slookup[last]);
//					last++;
//				}
//
//				qDebug() << "Part " << x << " End\n";
//
//			}
//
//			for (int x = 0; x < remains; x++)
//			{
//				sthread[i].list.append(slookup[last]);
//				last++;
//			}
//			

	// 		for (auto q: sthread->list)
	// 		{
	// 			qDebug() << q;
	// 		}

	}
	else
	{
		sthread = new SfvThread[parts];
		for (int i = 0; i < parts; i++)
		{
			sthread[i].TID = TIDCounter;
			sthread[i].beg = last;
			for (int x = 0; x < 5; x++)
			{
				sthread[i].list.append(QDir::cleanPath(SfvPath + QDir::separator() +  slookup[last]));
				last++;
			}
			sthread[i].end = last;

#ifdef _DEBUG
			std::cout << "Thread " << TIDCounter << "\n";
			for (auto titer : sthread[i].list)
			{
				std::cout << titer.toStdString() << "\n";
			}
			std::cout << "Thread " << TIDCounter << "End\n";
#endif

			connect(&sthread[i], &SfvThread::InsertCRC, this, &QtSfvWindow::OnInsertCrc);
			sthread[i].start();
			TIDCounter++;
		}

	}
// 	connect(sthread, &SfvThread::InsertCRC, this, &QtSfvWindow::OnInsertCrc);
//	sthread->start();

	slookup.clear();
	delete file;
}

void QtSfvWindow::OnActionClose()
{

}

void QtSfvWindow::OnInsertCrc(int TID, int item, uint32_t crc)
{
	items[item]->setText(2, QString::number(crc, 16));

	// 	std::cout << "TID is " << TID << "\n";
	// 	std::cout << "item is " << item << " crc is " << crc << "\n";
}
