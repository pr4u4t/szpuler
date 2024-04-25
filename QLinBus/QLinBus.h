#ifndef QLINBUS_H
#define QLINBUS_H

#include <QTimer>
#include <QStandardItemModel>
#include <QScrollBar>

#include "qlinbus_global.h"
#include "../api/api.h"
#include "settingsdialog.h"
#include "ui_qlinbus.h"

QT_BEGIN_NAMESPACE

namespace Ui {
    class QLinBusUI;
}

//class QIntValidator;
QT_END_NAMESPACE

class QLINBUS_EXPORT QLinBus : public Widget {
    Q_OBJECT

public:
    QLinBus(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& settingsPath);

public slots:
    void settingsChanged();

    void init();

    void scanStep();

    void dataReady(const QByteArray& data);

    void scanStop();

    void scanClear();

private:
    SettingsDialog::LinBusSettings m_settings;
    Ui::QLinBusUI* m_ui = nullptr;
    uint16_t m_scan = 0;
    QSharedPointer<IODevice> m_lin;
    QStandardItemModel* m_model = nullptr;
};

#endif