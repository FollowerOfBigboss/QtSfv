#include "settingsdialog.h"

#define B2MB(x) x >> 20

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

	hbox2 = new QHBoxLayout();
	label2 = new QLabel();
	label2->setText("Chunk Size (MB)");
	label2->setToolTip("Sets chunk size which will be used for every thread.");
	spinbox2 = new QSpinBox();
	spinbox2->setMinimum(0);


	hbox->addWidget(label);
	hbox->addWidget(threadSpinbox);
	hbox2->addWidget(label2);
	hbox2->addWidget(spinbox2);
	vbox->addLayout(hbox);
	vbox->addLayout(hbox2);


	vbox->addStretch(1);

	buttonbox = new QDialogButtonBox(QDialogButtonBox::StandardButton::Cancel | QDialogButtonBox::StandardButton::Save);
	buttonbox->setOrientation(Qt::Orientation::Horizontal);
	vbox->addWidget(buttonbox);


	connect(buttonbox, &QDialogButtonBox::rejected, this, [&] { this->close(); });
	connect(buttonbox, &QDialogButtonBox::accepted, this, &SettingsDialog::OnActionSaveSettings);
}



void SettingsDialog::OnUpdateSpinValue(uint32_t val)
{
	threadSpinbox->setValue(val);
}

void SettingsDialog::OnActionSaveSettings()
{
	emit UpdateThreadCountForJob(threadSpinbox->value());
	emit UpdateChunkSize(spinbox2->value());
	this->close();
}

void SettingsDialog::OnUpdateChunkValue(uint32_t val)
{
	spinbox2->setValue(B2MB(val));
}
