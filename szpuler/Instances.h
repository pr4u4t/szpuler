#ifndef INSTANCES_H
#define INSTANCES_H

#include <QTableView>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QHeaderView>
#include "../api/api.h"
#include "../core/core.h"

class Instances : public Widget {

	Q_OBJECT

public:

	enum Columns : qint32 {
		NAME,
		OBJECT,
		VERSION,
		UUID
	};

	Instances(Loader* ld, PluginsLoader* plugins, QWidget* parent = nullptr, const QString& settingsPath = QString());

	bool saveSettings();

	void settingsChanged();

public slots:

	void loaded(const Plugin* plugin);

protected slots:

	void activated(const QModelIndex& index);

private:

	QTableView* m_view = nullptr;
	QStandardItemModel* m_model = nullptr;

};

#endif