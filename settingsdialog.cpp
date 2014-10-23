#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "settings.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    readSettings();
}

void SettingsDialog::readSettings() {
    ui->cacheSlider->setValue(
                globalSettings->s.value("cacheSize",64).toInt());
    ui->cacheLabel2->setNum(ui->cacheSlider->value());
}

void SettingsDialog::writeSettings() {
    globalSettings->s.setValue("cacheSize", ui->cacheSlider->value());
    emit settingsChanged();
    this->close();
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}
