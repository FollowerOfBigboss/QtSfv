#include <iostream>

#include <QMainWindow>
#include <QMenuBar>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QTreeWidget>
#include <QFileDialog>
#include <thread>


class QtSfvWindow : public QMainWindow
{
	Q_OBJECT

private slots:
	void OnActionOpen();
	void OnActionClose();

public:
	QtSfvWindow();

	QTreeWidget* treeWidget;
	QList<QTreeWidgetItem*> items;
	std::thread* workerthread = nullptr;
};