#include "appwindow.h"

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
	connect(aboutaction, &QAction::triggered, this, [&] { QMessageBox::information(this, "About", "This program written by FollowerOfBigboss", QMessageBox::StandardButton::Ok, QMessageBox::StandardButton::Ok);});
	connect(aboutqtaction, &QAction::triggered, this, [&] { QMessageBox::aboutQt(this); });
	connect(exitaction, &QAction::triggered, this, [&] { this->close(); });

	treeWidget = new QTreeWidget(this);
	treeWidget->setRootIsDecorated(false);
	treeWidget->setAllColumnsShowFocus(true);
	treeWidget->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);
	treeWidget->setColumnCount(4);

	QStringList headers = {"File Name","CRC","Calculated CRC","Status"};
	treeWidget->setHeaderLabels(headers);

	// for (int i = 0; i < 100; ++i)
	// {
	// 	QTreeWidgetItem* item = new QTreeWidgetItem();
	// 	item->setText(0, "F");
	// 	item->setText(1, "U");
	// 	item->setText(2, "C");
	// 	item->setText(3, "K");
	// 	items.append(item);
	// }
	// 
	// treeWidget->insertTopLevelItems(0, items);
	
	this->setCentralWidget(treeWidget);
	treeWidget->expandAll();
}


void QtSfvWindow::OnActionOpen()
{

}

void QtSfvWindow::OnActionClose()
{

}
