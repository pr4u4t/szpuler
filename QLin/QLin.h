#ifndef QLIN_H
#define QLIN_H

#include "qlin_global.h"
#include "../api/api.h"
#include "settingsdialog.h"

class QLIN_EXPORT QLin : public IODevice {

    Q_OBJECT

public:
    QLin(Loader* ld, PluginsLoader* plugins, QObject* parent, const QString& path = QString());

	bool open(const QString& url);

	qint64 write(const QString& data);

	QString read(qint64 maxLen);

	qint64 bytesAvailable();

	void close();

	bool isOpen() const;

	bool flush();

public slots:

	void settingsChanged();

protected slots:

	//void readData();

private:

	SettingsDialog::LinSettings m_settings;
};

#endif