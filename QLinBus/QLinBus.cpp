#include <QCoreApplication>
#include <QTranslator>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFileDialog>
#include <QProgressDialog>

#include "QLinBus.h"
#include "../core/core.h"

struct QLinBusMenu {
    QLinBusMenu(QCoreApplication* app)
        : m_app(app) {
        m_translator = new QTranslator();
        if (m_translator->load(QLocale::system(), "QLinBus", "_", "translations")) { //set directory of ts
            m_app->installTranslator(m_translator);
        }
    }

    QMenu* m_linbusMenu = nullptr;
    QAction* m_newInstance = nullptr;
    QMenu* m_settings = nullptr;
    QCoreApplication* m_app = nullptr;
    QTranslator* m_translator = nullptr;
};

static bool QLinBus_register(ModuleLoaderContext* ldctx, PluginsLoader* ld, QLinBusMenu* ctx, Logger* log) {
    log->message("QLinBus_register()");

    GuiLoaderContext* gtx = ldctx->to<GuiLoaderContext>();
    if (gtx == nullptr) {
        log->message("QLinBus_register(): application is non gui not registering");
        return false;
    }

    auto linbusMenu = gtx->m_win->findMenu(ctx->m_app->translate("MainWindow", "&LinBus"));

    if (linbusMenu == nullptr) {
        log->message("QLinBus_register(): linbus menu not found");
        return false;
    }

    windowAddInstanceSettings(linbusMenu, &ctx->m_settings, &ctx->m_newInstance, "QLinBus", ctx->m_app, log);

    QObject::connect(ctx->m_newInstance, &QAction::triggered, gtx->m_win, &Window::create);
    QObject::connect(ld, &PluginsLoader::loaded, [gtx, ctx, log, ld](const Plugin* plugin) {
        windowAddPluginSettingsAction<QLinBus, QLinBusMenu, GuiLoaderContext, SettingsDialog, Plugin>(plugin, QString("QLinBus"), gtx, ctx, log);
    });

    return true;
}

static bool QLinBus_unregister(ModuleLoaderContext* ldctx, PluginsLoader* ld, QLinBusMenu* ctx, Logger* log) {
    return true;
}

REGISTER_PLUGIN(
    QLinBus,
    Plugin::Type::WIDGET,
    "0.0.1",
    "pawel.ciejka@gmail.com",
    "example plugin",
    QLinBus_register,
    QLinBus_unregister,
    QLinBusMenu,
    {"QLin"},
    true,
    700,
    LinBusSettings
)

QLinBus::QLinBus(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& path, LinBusSettings* set, const QString& uuid)
	: Widget(ld, plugins, parent, path, set, uuid)
    , m_ui(new Ui::QLinBusUI)
    , m_model(new QStandardItemModel(0,5))
    , m_state(QLinBusState::INITIAL)
    , m_sniffEnabled(true){
    m_ui->setupUi(this);
    
    connect(m_ui->startButton, &QPushButton::clicked, this, &QLinBus::startScan);
    connect(m_ui->stopButton, &QPushButton::clicked, this, &QLinBus::scanStop);
    connect(m_ui->clearButton, &QPushButton::clicked, this, &QLinBus::scanClear);
    connect(m_ui->pauseButton, &QPushButton::clicked, this, &QLinBus::pauseScan);
    connect(m_ui->sniffEnable, &QPushButton::clicked, this, &QLinBus::enableSniff);
    connect(m_ui->sniffDisable, &QPushButton::clicked, this, &QLinBus::disableSniff);
    connect(m_ui->exportJson, &QPushButton::clicked, this, &QLinBus::exportToJson);

    m_ui->stopButton->setEnabled(false);
    m_ui->pauseButton->setEnabled(false);
    m_ui->sniffEnable->setEnabled(false);
    m_ui->sniffDisable->setEnabled(true);
    

    m_model->setHeaderData(0, Qt::Horizontal, tr("ID"));
    m_model->setHeaderData(1, Qt::Horizontal, tr("Type"));
    m_model->setHeaderData(2, Qt::Horizontal, tr("DLC"));
    m_model->setHeaderData(3, Qt::Horizontal, tr("Data"));
    m_model->setHeaderData(4, Qt::Horizontal, tr("Time"));

    m_ui->scanTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_ui->scanTable->setModel(m_model);

    connect(&m_timer, &QTimer::timeout, this, &QLinBus::scanStep);
}

bool QLinBus::initialize() {
    emit message("QLinBus::init()", LoggerSeverity::LOG_DEBUG);
    const auto set = settings<LinBusSettings>();

    m_timer.setInterval(set->scanInterval());
    
    auto plugin = plugins()->instance("QLin", 0);
    auto lin = plugin.dynamicCast<IODevice>();
    m_lin = lin;
    connect(lin.data(), &IODevice::dataReady, this, &QLinBus::dataReady);
    connect(lin.data(), &IODevice::error, this, &QLinBus::errorReady);
    
    if (lin->isOpen() == false) {
        if (lin->open() == false) {
        //TODO:
        }
    }
    
    m_ui->scanProgress->setMinimum(set->scanStartID());
    m_ui->scanProgress->setMaximum(set->scanStopID());

    return true;
}

bool QLinBus::deinitialize() {
    return true;
}

SettingsMdi* QLinBus::settingsWindow() const {
    auto ret = new SettingsDialog(nullptr, this);
    QObject::connect(ret, &SettingsDialog::settingsUpdated, this, &QLinBus::settingsChanged);
    return ret;
}

void QLinBus::settingsChanged() {
    emit message("QLinBus::settingsChanged()", LoggerSeverity::LOG_DEBUG);
    const auto set = settings<LinBusSettings>();
    const auto src = qobject_cast<SettingsDialog*>(sender());
    const auto nset = src->settings<LinBusSettings>();
    *set = *nset;

    initialize();

    emit settingsApplied();
}

void QLinBus::startScan() {
    emit message("QLinBus::startScan()", LoggerSeverity::LOG_DEBUG);

    if (m_lin.isNull() || m_lin->isOpen() == false) {
        emit message("QLinBus::startScan(): failed LIN device not open");
        QMessageBox::critical(
            this,
            tr("QLinBus: error"),
            tr("LIN device not open"),
            QMessageBox::StandardButton::Ok
        );

        return;
    }


    QMetaObject::invokeMethod(m_lin.data(), "slaveID", Qt::DirectConnection,
        Q_RETURN_ARG(int, m_slaveID));

    switch (m_state) {
    case QLinBusState::STOP:
    case QLinBusState::INITIAL:
        m_model->removeRows(0, m_model->rowCount());
    case QLinBusState::PAUSE:
        m_ui->startButton->setEnabled(false);
        m_ui->stopButton->setEnabled(true);
        m_ui->pauseButton->setEnabled(true);
        m_state = QLinBusState::SCAN;
        m_timer.start();
        break;
    }
}

void QLinBus::scanStep() {
    emit message("QLinBus::scanStep()");

    if (m_scan <= settings<LinBusSettings>()->scanStopID()) {
        if (m_scan != m_slaveID) {
            QByteArray data(1, static_cast<char>(m_scan));
            m_lin->write(data);

            QList<QStandardItem*> cells;
            QString lid = QString::number(m_scan, 16);
            if (lid.size() == 1) {
                lid = "0" + lid;
            }
            cells << new QStandardItem("0x" + lid) << new QStandardItem("REQUEST") << new QStandardItem("-") << new QStandardItem("-") << new QStandardItem("-");
            if (settings<LinBusSettings>()->enableColors() == true) {
                for (QStandardItem* item : cells) {
                    item->setData(QColor(177, 211, 239), Qt::BackgroundRole);
                }
            }
            m_model->appendRow(cells);

            m_ui->scanProgress->setValue(m_scan);
        }
        ++m_scan;
    } else {
        m_state = QLinBusState::STOP;
        m_scan = settings<LinBusSettings>()->scanStartID();
        m_ui->startButton->setEnabled(true);
        m_ui->stopButton->setEnabled(false);
        m_ui->pauseButton->setEnabled(false);
        m_timer.stop();
    }
}

void QLinBus::errorReady(const QByteArray& data) {
    emit message("QLinBus::errorReady()", LoggerSeverity::LOG_DEBUG);

    if (m_sniffEnabled == false && m_state != QLinBusState::SCAN) {
        emit message("QLinBus::errorReady(): sniffer disabled while data arrived", LoggerSeverity::LOG_DEBUG);
        return;
    }

    QList<QStandardItem*> cells;
    
    if (data.startsWith("LIN ERROR")) {
        QByteArrayList list = data.split(',');
        for (auto& item : list) {
            item = item.mid(item.indexOf(':') + 1);
        }

        cells << new QStandardItem("-") << new QStandardItem("ERR") << new QStandardItem("-") << new QStandardItem("-") << new QStandardItem(list[0]);
    } else if (data.startsWith("LIN SYNCERR")) {
        QByteArrayList list = data.split(',');
        for (auto& item : list) {
            item = item.mid(item.indexOf(':') + 1);
        }

        cells << new QStandardItem("-") << new QStandardItem("SYNCERR") << new QStandardItem("-") << new QStandardItem("-") << new QStandardItem(list[0]);
    }

    if (settings<LinBusSettings>()->enableColors() == true) {
        for (QStandardItem* item : cells) {
            item->setData(QColor(239, 177, 184), Qt::BackgroundRole);
        }
    }

    m_model->appendRow(cells);
}

void QLinBus::dataReady(const QByteArray& data) {
    emit message("QLinBus::dataReady(): " + data, LoggerSeverity::LOG_DEBUG);

    if (m_sniffEnabled == false && m_state != QLinBusState::SCAN) {
        emit message("QLinBus::dataReady(): sniffer disabled while data arrived", LoggerSeverity::LOG_DEBUG);
        return;
    }

    if (data.startsWith("LIN NOANS")) {
        QByteArrayList list = data.split(',');
        for (auto& item : list) {
            item = item.mid(item.indexOf(':') + 1);
        }
        QList<QStandardItem*> cells;
        cells << new QStandardItem(list[0].trimmed()) << new QStandardItem("NOANS") << new QStandardItem("-") << new QStandardItem("-") << new QStandardItem(list[2]);
        if (settings<LinBusSettings>()->enableColors() == true) {
            for (QStandardItem* item : cells) {
                item->setData(QColor(239, 228, 184), Qt::BackgroundRole);
            }
        }
        m_model->appendRow(cells);
    } else if (data.startsWith("ID")) {
        QByteArrayList list = data.split(',');
        for (auto& item : list) {
            item = item.mid(item.indexOf(':') + 1);
        }
        
        QList<QStandardItem*> cells;
        cells   << new QStandardItem(list[0].trimmed()) 
                << new QStandardItem("ANS") 
                << new QStandardItem(list[1].removeIf([](QChar ch) { return ch == '\'';  })) 
                << new QStandardItem(formatData(list[2].trimmed())) 
                << new QStandardItem(list[3]);

        QFont font("Monospace");
        font.setStyleHint(QFont::TypeWriter);
        cells[3]->setFont(font);

        if (settings<LinBusSettings>()->enableColors() == true) {
            for (QStandardItem* item : cells) {
                item->setData(QColor(177, 239, 188), Qt::BackgroundRole);
            }
        }
        m_model->appendRow(cells);
    } else if (data.startsWith("LIN WAKEUP")) {
        QByteArrayList list = data.split(',');
        for (auto& item : list) {
            item = item.mid(item.indexOf(':') + 1);
        }

        emit message("QLinBus::dataReady: WAKEUP");
    } else if (data.startsWith("LIN SLEEP")) {
        QByteArrayList list = data.split(',');
        for (auto& item : list) {
            item = item.mid(item.indexOf(':') + 1);
        }
    
        emit message("QLinBus::dataReady: SLEEP");
    }

    m_ui->scanTable->verticalScrollBar()->setSliderPosition(m_ui->scanTable->verticalScrollBar()->maximum());
}

QByteArray QLinBus::formatData(const QByteArray& input) const {
    QByteArray ret;

    for (int i = 2; i < input.size(); ++i) {
        ret += input[i];
        if ((i-1) % 2 == 0) {
            ret += ' ';
        }
    }
    
    return ret;
}

void QLinBus::scanStop() {
    emit message("QLinBus::scanStop()", LoggerSeverity::LOG_DEBUG);
    m_state = QLinBusState::STOP;
    m_scan = settings<LinBusSettings>()->scanStartID();
    m_timer.stop();
    m_ui->startButton->setEnabled(true);
    m_ui->stopButton->setEnabled(false);
    m_ui->pauseButton->setEnabled(false);
}

void QLinBus::scanClear() {
    emit message("QLinBus::scanClear()", LoggerSeverity::LOG_DEBUG);
    m_model->removeRows(0, m_model->rowCount());
    scanStop();
    m_ui->scanProgress->setValue(0);
}

void QLinBus::pauseScan() {
    emit message("QLinBus::pauseScan()", LoggerSeverity::LOG_DEBUG);
    m_state = QLinBusState::PAUSE;
    m_ui->startButton->setEnabled(true);
    m_ui->stopButton->setEnabled(true);
    m_ui->pauseButton->setEnabled(false);
    m_timer.stop();
}

void QLinBus::enableSniff() {
    emit message("QLinBus::enableSniff()", LoggerSeverity::LOG_DEBUG);
    m_sniffEnabled = true;
    m_ui->sniffEnable->setEnabled(false);
    m_ui->sniffDisable->setEnabled(true);
}

void QLinBus::disableSniff() {
    emit message("QLinBus::disableSniff()", LoggerSeverity::LOG_DEBUG);
    m_sniffEnabled = false;
    m_ui->sniffEnable->setEnabled(true);
    m_ui->sniffDisable->setEnabled(false);
}

void QLinBus::exportToJson() {
    emit message("QLinBus::exportToJson()", LoggerSeverity::LOG_DEBUG);

    if (m_model->rowCount() <= 0) {
        emit message("QLinBus::exportToJson(): model empty", LoggerSeverity::LOG_DEBUG);
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(
        this, 
        tr("Save File"),
        QString(),
        tr("Text (*.json)")
    );

    if (fileName.isEmpty()) {
        emit message("QLinBus::exportToJson(): operation cancelled", LoggerSeverity::LOG_DEBUG);
        return;
    }

    QFile saveFile(fileName);

    if (saveFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODeviceBase::Truncate) == false) {
        emit message(QString("QLinBus::exportToJson(): can't open file: %1 %2").arg(fileName).arg(saveFile.errorString()), LoggerSeverity::LOG_DEBUG);
        return;
    } 
    
    QJsonObject json;
    QJsonArray items;

    QProgressDialog progress(tr("Exporting to JSON"), tr("Abort"), 0, m_model->rowCount(), this);
    progress.setWindowModality(Qt::WindowModal);

    for (int row = 0; row < m_model->rowCount(); row++) {
        QJsonObject rowItem;
        for (int col = 0; col < m_model->columnCount(); ++col) {
            rowItem[m_model->headerData(col, Qt::Horizontal, Qt::DisplayRole).toString()] = QJsonValue(m_model->index(row, col).data().toString());
        }
        items.append(rowItem);
        progress.setValue(row);

        if (progress.wasCanceled()) {
            return;
        }
    }

    json["items"] = items;

    QJsonDocument saveDoc(json);
    saveFile.write(saveDoc.toJson());
    progress.setValue(m_model->rowCount());
}
