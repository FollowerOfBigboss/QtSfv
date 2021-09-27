#include "settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget* parent) : QDialog(parent)
{
	this->setWindowTitle("Settings");

	vbox = new QVBoxLayout(this);
	hbox = new QHBoxLayout();

	label = new QLabel();
	label->setText("Thread count for job");
	label->setToolTip("It sets desired thread count for a job.\nIt may decrease calculation speed for multiple files.\nIt depends on your cpu and storage device.");
	threadSpinbox = new QSpinBox();
	threadSpinbox->setRange(1, 100);

	hbox->addWidget(label);
	hbox->addWidget(threadSpinbox);
	vbox->addLayout(hbox);
	vbox->addStretch(1);

	buttonbox = new QDialogButtonBox(QDialogButtonBox::StandardButton::Cancel | QDialogButtonBox::StandardButton::Save);
	buttonbox->setOrientation(Qt::Orientation::Horizontal);
	vbox->addWidget(buttonbox);


	connect(buttonbox, &QDialogButtonBox::rejected, this, [&] { this->close(); });
	connect(buttonbox, &QDialogButtonBox::accepted, this, &SettingsDialog::OnActionSaveSettings);
}



void SettingsDialog::OnUpdateSpinValue(int val)
{
	threadSpinbox->setValue(val);
}

void SettingsDialog::OnActionSaveSettings()
{
	emit UpdateFilePerThread(threadSpinbox->value());
	this->close();
}
