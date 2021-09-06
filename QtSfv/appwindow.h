#include <QMainWindow>
#include <QMenuBar>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QTreeWidget>


class QtSfvWindow : public QMainWindow
{
	Q_OBJECT

private slots:
	void OnActionOpen();
	void OnActionClose();

public:
	QtSfvWindow();

private:
	QTreeWidget* treeWidget;
	QList<QTreeWidgetItem*> items;
};