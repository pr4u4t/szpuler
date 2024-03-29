// Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
// Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <QIntValidator>
#include <QLineEdit>
#include <QCryptographicHash>
#include <QMessageBox>

//#include "DockManager.h"
#include <QCloseEvent>

static const char blankString[] = QT_TRANSLATE_NOOP("SettingsDialog", "N/A");

SettingsDialog::SettingsDialog(QWidget* parent, Loader* loader, const QString& settingsPath)
    : SettingsMdi(parent)
    , m_currentSettings(Settings::get(), settingsPath)
    , m_ui(new Ui::SettingsDialog)
    , m_intValidator(new QIntValidator(0, 4000000, this))
    , m_settingsPath(settingsPath) {
    emit message("SettingsDialog::SettingsDialog");
    m_ui->setupUi(this);

    connect(m_ui->okButton, &QPushButton::clicked,
        this, &SettingsDialog::ok);
    connect(m_ui->applyButton, &QPushButton::clicked,
        this, &SettingsDialog::apply);
    connect(m_ui->cancelButton, &QPushButton::clicked,
        this, &SettingsDialog::cancel);

    fillFromSettings();
}

SettingsDialog::~SettingsDialog() {
    delete m_ui;
}

SettingsDialog::LinSettings SettingsDialog::linSettings() const {
    emit message("SettingsDialog::settings");
    return m_currentSettings;
}

void SettingsDialog::fillFromSettings() {
    emit message("SettingsDialog::fillFromSettings");

    //m_ui->prompt->setText(m_currentSettings.prompt);
    //m_ui->localEcho->setChecked(m_currentSettings.localEcho);
}

void SettingsDialog::updateSettings() {
    emit message("SettingsDialog::updateSettings");

    QString version = m_ui->linVersion->currentText();

    if (version == "1.3") {
        m_currentSettings.linVersion = XL_LIN_VERSION_1_3;
    } else if (version == "2.0") {
        m_currentSettings.linVersion = XL_LIN_VERSION_2_0;
    } else if (version == "2.1") {
        m_currentSettings.linVersion = XL_LIN_VERSION_2_1;
    }

    m_currentSettings.masterID = static_cast<char>(m_ui->masterID->text().toInt());
    m_currentSettings.slaveID = static_cast<char>(m_ui->slaveID->text().toInt());
    m_currentSettings.master = m_ui->masterCheck->isChecked();
    m_currentSettings.slave = m_ui->slaveCheck->isChecked();

    QSettings s = Settings::get();
    m_currentSettings.save(s, settingsPath());
}

void SettingsDialog::ok() {
    emit message("SettingsDialog::ok");

    updateSettings();
    close();
    emit settingsUpdated();
}

void SettingsDialog::apply() {
    emit message("SettingsDialog::apply");

    updateSettings();
    emit settingsUpdated();
}

void SettingsDialog::cancel() {
    emit message("SettingsDialog::cancel");
    close();
}