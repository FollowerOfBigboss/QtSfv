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
	
	QVBoxLayout* vbox;

	QLabel* label;
	QHBoxLayout* hbox;
	QSpinBox* threadSpinbox;
	QDialogButtonBox* buttonbox;

	QHBoxLayout* hbox2;
	QLabel* label2;
	QSpinBox* spinbox2;

public slots:
	void OnUpdateSpinValue(uint32_t val);
	void OnActionSaveSettings();
	void OnUpdateChunkValue(uint32_t val);

signals:
	void UpdateThreadCountForJob(uint32_t val);
	void UpdateChunkSize(uint32_t val);
};

#endif