#include "appwindow.h"

#include "crc32/CRC.h"

std::vector<int> MessageQueue;
QString filedirectory;

void SfvWorkerThread(void* ptr);
const bool TerminateThread = true;

enum ThreadMessages : int
{
	play,
	pause,
	reset,
	load,
	die
};

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

	connect(playaction, &QAction::triggered, this, [&] { MessageQueue.push_back(ThreadMessages::play); });
	connect(pauseaction, &QAction::triggered, this, [&] { MessageQueue.push_back(ThreadMessages::pause); });

	treeWidget = new QTreeWidget(this);
	treeWidget->setRootIsDecorated(false);
	treeWidget->setAllColumnsShowFocus(true);
	treeWidget->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);
	treeWidget->setColumnCount(4);

	QStringList headers = {"File Name","CRC","Calculated CRC","Status"};
	treeWidget->setHeaderLabels(headers);


	this->setCentralWidget(treeWidget);
	treeWidget->expandAll();

	workerthread = new std::thread(SfvWorkerThread, this);
}


void QtSfvWindow::OnActionOpen()
{
	QString name = QFileDialog::getOpenFileName(this, "Open Image", "", "SFV Files (*.sfv)");
	QDir d = QFileInfo(name).absoluteDir();
	QString absolute = d.absolutePath();

	
	QFile file(name);
	if (!file.open(QIODevice::ReadOnly))
	{
		QMessageBox::critical(this, "Error", "File couldn't opened!");
		return;
	}

	MessageQueue.push_back(ThreadMessages::reset);
	filedirectory = absolute;

	for (int i = 0; i < items.count(); i++)
	{
		delete items[i];
	}

	items.clear();
	treeWidget->clear();


	while (!file.atEnd()) 
	{
		QByteArray line = file.readLine();
		
		if (line.startsWith(';') == true)
			continue;

		QByteArrayList sline =  line.split(' ');

		// Remove newline character
		sline[1].remove(sline[1].indexOf('\n'), 1);


		QTreeWidgetItem* item = new QTreeWidgetItem();
	 	item->setText(0, sline[0]);
		item->setText(1, sline[1]);
		items.append(item);
	}
	treeWidget->insertTopLevelItems(0, items);

	MessageQueue.push_back(ThreadMessages::reset);
	MessageQueue.push_back(ThreadMessages::load);
	MessageQueue.push_back(ThreadMessages::play);
}

void QtSfvWindow::OnActionClose()
{

}

void SfvWorkerThread(void* ptr)
{
static int itemcounter = 0;
static bool go = false;
static int countto = 0;


	while (TerminateThread)
	{
		if (MessageQueue.size() > 0)
		{
			// Play
			if (MessageQueue[0] == ThreadMessages::play)
			{
				go = true;
				printf("Play message operation will continue!\n");
			}

			// Pause
			else if (MessageQueue[0] == ThreadMessages::pause)
			{
				go = false;
				printf("Pause message operation will stop!\n");
			}

			// Reset
			else if (MessageQueue[0] == ThreadMessages::reset)
			{
				go = false;
				countto = 0;
				itemcounter = 0;
				printf("Reset message received all statics are zeroed!\n");
			}

			// load
			else if (MessageQueue[0] == ThreadMessages::load)
			{
				countto = ((QtSfvWindow*)ptr)->items.count();
				printf("Load message received countto set!\n");

			}
			else
			{
				printf("Unknown message %i\n", MessageQueue[0]);
			}

			MessageQueue.erase(MessageQueue.begin());
		}
	
		if (go == true)
		{
			if (itemcounter == countto)
			{
				go = false;
				itemcounter = 0;
				countto = 0;
			}

			if (itemcounter < countto)
			{
				QString filename = QDir::cleanPath(filedirectory + QDir::separator() + ((QtSfvWindow*)ptr)->items[itemcounter]->text(0));
				QFile file(filename);
				if (file.open(QIODevice::ReadOnly) != true)
				{
					((QtSfvWindow*)ptr)->items[itemcounter]->setText(3, "File couldn't open!");
				}

				else
				{
					QByteArray content = file.readAll();
					uint32_t crc = CRC::Calculate(content.constData(), content.size(), CRC::CRC_32());

					((QtSfvWindow*)ptr)->items[itemcounter]->setText(2, QString::number(crc, 16));
					((QtSfvWindow*)ptr)->items[itemcounter]->setText(3, "SUCCESS");
				}

				itemcounter++;
			}
		}
	}
}
