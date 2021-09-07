#include "appwindow.h"

std::vector<int> MessageQueue;

void SfvWorkerThread(void* ptr);
bool TerminateThread = false;

enum ThreadMessages : int
{
	play,
	pause,
	reset
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
	auto name = QFileDialog::getOpenFileName(this, "Open Image", "", "SFV Files (*.sfv)");

	QFile file(name);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return;
	
	items.clear();
	treeWidget->clear();

	while (!file.atEnd()) 
	{
		QByteArray line = file.readLine();
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

	while (!TerminateThread)
	{
		if (MessageQueue.size() > 0)
		{
			std::cout << "Messages at queue" << "\n";

			for (int i = 0; i < MessageQueue.size(); i++)
			{
				std::cout << "Message[" << i << "]" << "=" << MessageQueue[i] << "\n";
			}

			// Play
			if (MessageQueue[0] == ThreadMessages::play)
			{
				go = true;
			}

			// Pause
			if (MessageQueue[0] == ThreadMessages::pause)
			{
				go = false;
			}

			// Reset
			if (MessageQueue[0] == ThreadMessages::reset)
			{
				countto = ((QtSfvWindow*)ptr)->items.count();
				go = false;
				itemcounter = 0;
			}

			MessageQueue.erase(MessageQueue.begin());
		
		}

		if (go == true)
		{
			if (itemcounter == countto)
			{
				go = false;
				break;

			}

			std::cout << itemcounter << "\n";
			std::cout << ((QtSfvWindow*)ptr)->items[itemcounter]->text(0).toStdString() << "\n";
			((QtSfvWindow*)ptr)->items[itemcounter]->setText(3, "Yikes!");
			itemcounter++;
			_sleep(1000);

			// for (; itemcounter < ((QtSfvWindow*)ptr)->items.count(); itemcounter++)
			// {
			// 	if (MessageQueue.size() > 0)
			// 	{
			// 		if (MessageQueue[0] == ThreadMessages::pause || MessageQueue[0] == ThreadMessages::reset)
			// 			break;
			// 
			// 		MessageQueue.pop_back();
			// 	}
			// 
			// 	std::cout << ((QtSfvWindow*)ptr)->items[itemcounter]->text(0).toStdString() << "\n";
			// 	((QtSfvWindow*)ptr)->items[itemcounter]->setText(3, "Yikes!");
//			// 	_sleep(1000);
			// }
			// if (MessageQueue[0] != ThreadMessages::pause)
			// {
			// 	go = false;
			// 	itemcounter = 0;
			// }

		}
	}

}
