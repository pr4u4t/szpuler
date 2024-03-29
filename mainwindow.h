// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QMenu>
#include <QMdiSubWindow>
#include <QMdiArea>
#include <QAction>
#include <QString>
#include <QRegExp>
#include <QHash>
#include <functional>
#include <QSet>

#include "api/api.h"
#include "ModuleLoader.h"
#include "MdiWindow.h"

#include "DockManager.h"
#include "DockAreaWidget.h"

class MainWindow : public Window {
    Q_OBJECT

signals:
    void settingsChanged();
    
public:
    static constexpr const char* winTitle = "szpuler";
    static constexpr const char* fatalError = "Fatal error occurred please contact support";

    MainWindow(MLoader* plugins = nullptr, Logger* logger = nullptr);

    ~MainWindow();

    QSettings& settings();  
    
    MainWindow& operator<< (const QString& QtClassLibrary1msg);
    
    //Logger* logger();
    
    MLoader* plugins();

    void setPlugins(MLoader* loader) {
        m_plugins = loader;
    }

    bool addSubWindow(QWidget* widget, const QString& title = QString()) override;

    qint64 restoreSession() {
        m_logger->message("MainWindow::restoreSession()");

        QString session = settings().value("session/plugins").toString();
        QStringList list = session.split(" ", Qt::SkipEmptyParts);
        qint64 ret = 0;
        QRegExp rx("^([A-Za-z0-9]+)");

        if (list.size() > 0) {
            m_logger->message("MainWindow::restoreSession(): restore plugins instances: "+session);
            for (auto it = list.begin(), end = list.end(); it != end; ++it, ++ret) {
                m_logger->message("MainWindow::restoreSession(): creating plugin: " + *it);
                if (rx.indexIn(*it) != -1) {
                    QString name = rx.cap(1);

                    auto plugin = plugins()->instance(name, this, *it);

                    if (plugin.isNull()) {
                        m_logger->message("MainWindow::restoreSession(): failed to create plugin: "+*it);
                        continue;
                    }

                    if (plugin->type() == Plugin::Type::WIDGET) {
                        m_logger->message("MainWindow::restoreSession(): adding widget: "+*it);
                        ads::CDockWidget* dockWidget = new ads::CDockWidget(plugin->name());

                        dockWidget->setWidget(dynamic_cast<Widget*>(plugin.data()));
                        dockWidget->setFeature(ads::CDockWidget::DontDeleteContent, true);

                        m_dockManager->addDockWidget(ads::CenterDockWidgetArea, dockWidget, m_area);
                    }
                }
            }
        } else {
            m_logger->message("MainWindow::restoreSession() :");
        }

        QByteArray state = settings().value("session/state").toByteArray();
        if (state.isEmpty() == false) {
            m_logger->message("MainWindow::restoreSession() : restoring dock session");
            m_dockManager->restoreState(state);
        }
        m_area = m_dockManager->dockArea(0);
        return ret;
    }
    

protected:

    ads::CDockWidget* addSubWindowInternal(QWidget* widget, const QString& title = QString());

    void closeEvent(QCloseEvent* ev) override;

    qint64 saveSession() {
        m_logger->message("MainWindow::saveSession()");
        QSettings& setts = settings();
        QString value;
        
        QByteArray state = m_dockManager->saveState();
        settings().setValue("session/state", state);

        if (settings().status() != QSettings::NoError) {
            m_logger->message("MainWindow::saveSession(): failed to save session/state");
        }

        QList<QSharedPointer<Plugin>> exts = plugins()->instances();
        QStringList order;

        while (exts.size() > 0) {
            for (int i = 0; i < exts.size() && exts.size() > 0; ++i) {
                if (exts[i]->depends().size() == 0) {
                    order << exts[i]->name();
                    exts.removeAt(i);
                    --i;
                } else {
                    QStringList tmp = exts[i]->depends();
                    QSet<QString> deps(tmp.begin(), tmp.end());
                    QSet<QString> met(order.begin(), order.end());
                    if (deps.subtract(met).size() == 0) {
                        order << exts[i]->name();
                        exts.removeAt(i);
                        --i;
                    }
                }
            }
        }

        QStringList result;
        auto instances = plugins()->instances();
        
        for (auto item : order) {
            for (auto instance : instances) {
                if (instance->name() == item) {
                    result << instance->name() + "-" + instance->uuid();
                }
            }
        }

        settings().setValue("session/plugins", result.join(" "));
        if (settings().status() != QSettings::NoError) {
            m_logger->message("MainWindow::saveSession(): failed to save session/plugins");
        }

        return 0;
    }

signals:

    void message(const QString& msg, LoggerSeverity severity = LoggerSeverity::LOG_NOTICE) const;

public slots:

    void createOrActivate() override;

    void showStatusMessage(const QString& msg, int timeout = 0) override;

private slots:
    void about();
    void switchLayoutDirection();
    //void createOrActivatePlugins();
    //void createOrActivateInstances();

private:

    bool createWindowByName(const QString& name);
    void createActions();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    
    //MdiChild *activeMdiChild() const;
    /*QMdiSubWindow**/ ads::CDockWidget* findChildWindow(const QString& name) const;
    
    static constexpr const int statusTimeOut = 2000;
    static constexpr const char* aboutText = "The <b>MDI</b> example demonstrates how to write multiple "
                                             "document interface applications using Qt.";
    static constexpr int QSlotInvalid = -1;

    ads::CDockManager* m_dockManager = nullptr;
    ads::CDockAreaWidget* m_area = nullptr;

    QMenu *m_windowMenu = nullptr;
    QAction *m_closeAct = nullptr;
    QAction *m_closeAllAct = nullptr;
    QAction *m_tileAct = nullptr;
    QAction *m_cascadeAct = nullptr;
    QAction *m_nextAct = nullptr;
    QAction *m_previousAct = nullptr;
    QAction *m_windowMenuSeparatorAct = nullptr;
    QSettings m_settings;
    QAction *m_actionConnect = nullptr;
    QAction *m_actionDisconnect = nullptr;
    QAction *m_actionConfigure = nullptr;
    MLoader* m_plugins = nullptr;
    //QMdiSubWindow* m_current = nullptr;
    Logger* m_logger = nullptr;
};

#endif
