#include <iostream>

#include <QMainWindow>
#include <QMenuBar>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QTreeWidget>
#include <QFileDialog>
#include <QProgressDialog>
#include <QThread>
#include <QApplication>
#include <thread>


class QtSfvWindow : public QMainWindow
{
	Q_OBJECT

public slots:
	void OnActionOpen();
	void OnActionClose();

	void OnInsertCrc(int TID, int item, uint32_t crc);


public:
	QtSfvWindow();
	void closeEvent(QCloseEvent* closeEvent);

	bool ParseLine(QByteArray& bytearray);

	std::vector<QString> slookup;
	QString SfvPath;
	QFile* file;
	QTreeWidget* treeWidget;
	QList<QTreeWidgetItem*> items;


};