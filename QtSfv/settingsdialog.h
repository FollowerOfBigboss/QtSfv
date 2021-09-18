#ifndef _SETTINGS_DIALOG
#define _SETTINGS_DIALOG

#include <QDialog>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QDialogButtonBox>

class SettingsDialog : public QDialog
{
	Q_OBJECT

public:
	SettingsDialog(QWidget* parent = 0);

	QLabel* label;
	QVBoxLayout* vbox;
	QHBoxLayout* hbox;
	QSpinBox* threadSpinbox;
	QPushButton* button;
	QDialogButtonBox* buttonbox;

public slots:
	void OnUpdateSpinValue(int val);
	void OnActionSaveSettings();

signals:
	void UpdateFilePerThread(int val);
};

#endif