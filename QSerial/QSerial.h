#ifndef QSERIAL_H
#define QSERIAL_H

#include <QSerialPort>
#include <QMessageBox>
#include <QString>

#include "qserial_global.h"
#include "../api/api.h"
#include "settingsdialog.h"

class QSERIAL_EXPORT QSerial : public IODevice {

	Q_OBJECT

public:
	QSerial(Loader* ld, PluginsLoader* plugins, QObject* parent, const QString& path, SerialSettings * set, const QString& uuid);

	bool open(const QString& url = QString());

	qint64 write(const QString& data);

	qint64 write(const QByteArray& data);

	QString read(qint64 maxLen);

	qint64 bytesAvailable();

	void close();

	bool isOpen() const;

	bool flush();

	SettingsMdi* settingsWindow() const override;

	Q_INVOKABLE bool reset(Reset type = Reset::SOFT) {
		emit message("QSerial::reset(Reset type)");
		return true;
	}

	bool initialize() override;

	bool deinitialize() override;

public slots:

	void settingsChanged();

protected slots:

	void readData();

private:

	QSerialPort* m_serial = nullptr;
};

#endif