#ifndef _DEBUG_DIALOG
#define _DEBUG_DIALOG

#include <QDialog>
#include <QpushButton>

class DebugDialog : public QDialog
{
	Q_OBJECT

public:
	DebugDialog(QWidget* parent = 0);

};

#endif