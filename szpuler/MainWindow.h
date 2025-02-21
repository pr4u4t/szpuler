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
#include <QToolBar>
#include <QComboBox>
#include <QWidgetAction>
#include <QInputDialog>
#include <QMessageBox>
#include <QCheckBox>
#include <QProgressDialog>

#include "../api/api.h"
#include "../core/core.h"
#include "MdiWindow.h"

#include "DockManager.h"
#include "DockAreaWidget.h"
#include "DockComponentsFactory.h"
#include "DockWidgetTab.h"
#include "DockWidget.h"
#include "Help.h"
#include "LoadingDialog.h"

enum class MainWindowQuit {
    QUIT_SAVE,
    QUIT_WITHOUT_SAVE
};

struct MainWindowSettings {

    static constexpr const char* const geometryKey = "geometry";
    static constexpr const char* const geometryValue = nullptr;
    static constexpr const char* const fontSizeKey = "fontSize";
    static constexpr const int fontSizeValue = -1;
    static constexpr const char* const statusTimeoutKey = "statusTimeout";
    static constexpr const int statusTimeoutValue = 2000;
    static constexpr const char* const translationsKey = "translations";
    static constexpr const bool translationsValue = false;
    static constexpr const char* const hideMenuKey = "hideMenu";
    static constexpr const bool hideMenuValue = false;
    static constexpr const char* const lockKey = "lock";
    static constexpr const bool lockValue = false;
    static constexpr const char* const kioskKey = "kiosk";
    static constexpr const bool kioskValue = false;
    static constexpr const char* const perspectiveKey = "perspective";
    static constexpr const char* const perspectiveValue = "Default";

    MainWindowSettings()
        : geometry(geometryValue)
        , fontSize(fontSizeValue)
        , statusTimeout(statusTimeoutValue)
        , translations(translationsValue)
        , hideMenu(hideMenuValue)
        , lock(lockValue)
        , kiosk(kioskValue)
        , perspective(perspectiveValue){
    }

    MainWindowSettings(const QSettings& settings, const QString& settingsPath) 
        : geometry(settings.value(settingsPath + "/" + geometryKey, geometryValue).toByteArray())
        , fontSize(settings.value(settingsPath + "/" + fontSizeKey, fontSizeValue).toInt())
        , statusTimeout(settings.value(settingsPath + "/" + statusTimeoutKey, statusTimeoutValue).toInt())
        , translations(settings.value(settingsPath + "/" + translationsKey, translationsValue).toBool())
        , hideMenu(settings.value(settingsPath + "/" + hideMenuKey, hideMenuValue).toBool())
        , lock(settings.value(settingsPath + "/" + lockKey, lockValue).toBool())
        , kiosk(settings.value(settingsPath + "/" + kioskKey, kioskValue).toBool())
        , perspective(settings.value(settingsPath + "/" + perspectiveKey, perspectiveValue).toString()){
    }

    void save(QSettings& settings, const QString& settingsPath) {
        settings.setValue(settingsPath + "/" + geometryKey, geometry);
        settings.setValue(settingsPath + "/" + fontSizeKey, fontSize);
        settings.setValue(settingsPath + "/" + statusTimeoutKey, statusTimeout);
        settings.setValue(settingsPath + "/" + translationsKey, translations);
        settings.setValue(settingsPath + "/" + hideMenuKey, hideMenu);
        settings.setValue(settingsPath + "/" + lockKey, lock);
        settings.setValue(settingsPath + "/" + kioskKey, kiosk);
        settings.setValue(settingsPath + "/" + perspectiveKey, perspective);
    }

    QByteArray geometry;
    int fontSize;
    int statusTimeout;
    bool translations;
    bool hideMenu;
    bool lock;
    bool kiosk; 
    QString perspective;
};

class Session;

class DeleteWorkspaceDialog : public QDialog {
    Q_OBJECT

public:
    DeleteWorkspaceDialog(QWidget* parent = nullptr, const QStringList& wlist = QStringList()) 
        : QDialog(parent) {
        // Set up the dialog layout
        QVBoxLayout* layout = new QVBoxLayout(this);

        // Add a label
        QLabel* label = new QLabel("Select an option:", this);
        layout->addWidget(label);

        // Add a combo box
        comboBox = new QComboBox(this);
        for (auto item : wlist) {
            comboBox->addItem(item);
        }
        layout->addWidget(comboBox);

        QHBoxLayout* buttons = new QHBoxLayout();
        layout->addItem(buttons);
        buttons->addStretch();
        // Add a "Submit" button
        QPushButton* submitButton = new QPushButton(tr("Submit"), this);
        buttons->addWidget(submitButton);

        // Connect the submit button to accept the dialog
        connect(submitButton, &QPushButton::clicked, this, &DeleteWorkspaceDialog::accept);

        QPushButton* cancelButton = new QPushButton(tr("Cancel"), this);
        buttons->addWidget(cancelButton);

        // Connect the submit button to accept the dialog
        connect(cancelButton, &QPushButton::clicked, this, &DeleteWorkspaceDialog::reject);
    }

    QString selectedOption() const {
        return comboBox->currentText(); 
    }

private:
    QComboBox* comboBox;
};


class ChooseWidgetsDialog : public QDialog {
    Q_OBJECT

public:
    explicit ChooseWidgetsDialog(QWidget* parent = nullptr, const QStringList& widgets = QStringList()) 
        : QDialog(parent) {
        setWindowTitle(tr("Choose widgets to copy"));
        resize(300, 200);

        // Create the main layout
        QVBoxLayout* layout = new QVBoxLayout(this);
        setLayout(layout);

        for (const auto& widget : widgets) {
            QCheckBox* box = new QCheckBox(widget);
            layout->addWidget(box);
        }
        
        QHBoxLayout* buttons = new QHBoxLayout();
        layout->addItem(buttons);
        buttons->addStretch();
        // Add a "Submit" button
        QPushButton* submitButton = new QPushButton(tr("Submit"), this);
        buttons->addWidget(submitButton);

        // Connect the submit button to accept the dialog
        connect(submitButton, &QPushButton::clicked, this, &ChooseWidgetsDialog::accept);

        QPushButton* cancelButton = new QPushButton(tr("Cancel"), this);
        buttons->addWidget(cancelButton);

        // Connect the submit button to accept the dialog
        connect(cancelButton, &QPushButton::clicked, this, &ChooseWidgetsDialog::reject);
    }

    QStringList selectedOptions() const {
        QStringList ret;
        const auto list = findChildren<QCheckBox*>();
        for (const auto item : list) {
            if (item->isChecked()) {
                ret << item->text();
            }
        }
        return ret;
    }
};

class MainWindow : public Window {
    Q_OBJECT

    friend class Session;

signals:
    void settingsChanged();
    
public:
   
    MainWindow(const QSettings& settings, const QString& settingsPath, MLoader* plugins = nullptr, Logger* logger = nullptr);

    ~MainWindow(); 
    
    MainWindow& operator<<(const QString& msg);
    
    MLoader* plugins();

    void setPlugins(MLoader* loader);

    bool addSubWindow(QWidget* widget, const QString& title = QString()) override;

    bool addSubWindow(Widget* widget);

    Widget* find(const QString& uuid) const;

    bool toggleWindow(const QString& title);

    bool hasWindowByTitle(const QString& name) const {
        return findChildWindowByTitle(name) != nullptr;
    }

    bool openPerspective(const QString& state);

protected:

    ads::CDockWidget* addSubWindowInternal(QWidget* widget, const QString& title = QString());

    void closeEvent(QCloseEvent* ev) override;

signals:

    void message(const QString& msg, LoggerSeverity severity = LoggerSeverity::LOG_NOTICE) const;

    void aboutToQuit(const MainWindowQuit& type);

    void sessionStore();

public slots:

    void createOrActivate() override;

    void create() override;

    void showStatusMessage(const QString& msg, int timeout = 0) override;

    void createPreferences();

    void sessionRestored();

private slots:

    void storeSettings() {
        emit message("MainWindow::storeSettings()");
        emit sessionStore();
        savePerspectives();
    }

    void copyPerspective(const QString& src) {
        emit message("MainWindow::copyPerspective(const QString& src)");

        auto state = m_dockManager->saveState();
        QString s(state);

        const QString perspectiveName = QInputDialog::getText(this, tr("Create perspective"), tr("Enter unique name:"));
        if (perspectiveName.isEmpty()) {
            emit message("MainWindow::copyPerspective: perspective name is empty");
            return;
        }

        if (m_dockManager->perspectiveNames().contains(perspectiveName) == true) {
            emit message(QString("MainWindow::copyPerspective: Perspective %1 already exists").arg(perspectiveName));
            QMessageBox::critical(this, tr("Error perspective already exists"), tr("Perspective %1 already exists").arg(perspectiveName));
            return;
        }
    
        auto winMap = m_dockManager->dockWidgetsMap();
        auto widgets = m_dockManager->dockWidgetsMap().keys();

        ChooseWidgetsDialog dialog(nullptr, widgets);
        if (dialog.exec() != QDialog::Accepted){
            emit message("MainWindow::copyPerspective: operation rejected");
            return;
        }

        auto result = dialog.selectedOptions();

        QProgressDialog progress("Copying workspace", "Abort Copy", 0, result.size(), this);
        progress.setWindowModality(Qt::WindowModal);

        newPerspective(perspectiveName);
        updatePerspectiveSwitcher(perspectiveName);
        m_dockManager->openPerspective(perspectiveName);

        auto wins = m_dockManager->dockWidgetsMap();
        for (auto it = wins.cbegin(), end = wins.cend(); it != end; ++it) {
            it.value()->toggleView(false);
        }

        for (const auto widget : result) {
            auto src = plugins()->findByObjectName(widget);
            progress.setValue(progress.value()+1);
            if (src == nullptr) {
                emit message(QString("MainWindow::copyPerspective: Unable to find widget by name: %1").arg(widget));
                continue;
            }
            
            if (progress.wasCanceled()) {
                break;
            }

            PluginsLoader::PluginType dst;

            if (src->multipleInstances()) {
                auto dst = plugins()->copy(src->uuid());
                if (dst == nullptr) {
                    emit message(QString("MainWindow::copyPerspective: failed to copy plugin: %1").arg(src->uuid()));
                    continue;
                }

                auto sWin = src.dynamicCast<Widget>();
                auto dWin = dst.dynamicCast<Widget>();

                emit message(QString("MainWindow::copyPerspective: copied %1 -> %2").arg(sWin->objectName()).arg(dWin->objectName()));

                const auto sourcePattern = QString("\"%1\"").arg(sWin->objectName());
                const auto dstPattern = QString("\"%1\"").arg(dWin->objectName());

                s.replace(sourcePattern, dstPattern);
                addSubWindow(dWin.data());
            }
        }

        m_dockManager->restoreState(s.toLocal8Bit());
        progress.setValue(result.size());
    }

    void changePerspective(const QString& name) {
        emit message("MainWindow::changePerspective(const QString& name)");
        if (name == m_currentPerspective) {
            return;
        }

        QByteArray current = m_dockManager->saveState();
        QByteArray stored = m_dockManager->getPerspective(m_currentPerspective);


        if (current != stored && QMessageBox::question(this, tr("store current perspective"),
            tr("Your current workspace contains changes would you like to save them?")) == QMessageBox::Yes) {
            m_dockManager->addPerspective(m_currentPerspective);
        }

        m_dockManager->openPerspective(name);
    }

    void startPerspectiveChange(const QString& perspective) {
        emit message("MainWindow::startPerspectiveChange(const QString& perspective)");
        m_dockManager->setEnabled(false);
        m_loading.show();
    }

    void endPerspectiveChange(const QString& perspective) {
        emit message("MainWindow::endPerspectiveChange(const QString& perspective)");
        m_loading.hide();
        m_dockManager->setEnabled(true);
        m_currentPerspective = perspective;
    }

    void about();

    void help();

    void switchLayoutDirection();

    void subWindowClosed(QObject* ptr = nullptr);

    void savePerspectives(){
        emit message("MainWindow::savePerspectives()");
        QSettings set = Settings::get();
        m_dockManager->savePerspectives(set);
    }

    void restorePerspectives(){
        emit message("MainWindow::restorePerspectives()");
        QSettings set = Settings::get();
        m_dockManager->loadPerspectives(set);
        m_perspectiveComboBox->clear();
        const QStringList perspectives = m_dockManager->perspectiveNames();
        m_perspectiveComboBox->addItems(perspectives);
    }

    void toggleToolbar() {
        emit message("MainWindow::toggleToolbar()");
        if (m_tbar->isHidden()) {
            m_tbar->show();
        } else {
            m_tbar->hide();
        }
    }

    void lockWorkspace(bool value){
        emit message("MainWindow::lockWorkspace(bool value)");
        if (value) {
            m_dockManager->lockDockWidgetFeaturesGlobally();
        } else {
            m_dockManager->lockDockWidgetFeaturesGlobally(ads::CDockWidget::NoDockWidgetFeatures);
        }
    }

    void newPerspective(const QString& name){
        emit message("MainWindow::newPerspective(const QString& name)");
        QString perspectiveName = name;

        if (perspectiveName.isEmpty()) {
            emit message("MainWindow::newPerspective: perspectiveName is empty");
            perspectiveName = QInputDialog::getText(this, tr("Create perspective"), tr("Enter unique name:"));
            if (perspectiveName.isEmpty()) {
                emit message("MainWindow::newPerspective: perspectiveName empty after dialog");
                return;
            }
        }

        if (m_dockManager->perspectiveNames().contains(perspectiveName) == true) {
            QMessageBox::critical(this, tr("Error perspective already exists"), tr("Perspective %1 already exists").arg(perspectiveName));
            return;
        }

        m_dockManager->addPerspective(perspectiveName);
        updatePerspectiveSwitcher(perspectiveName);

        //savePerspectives();
    }

    void updatePerspectiveSwitcher(const QString& perspectiveName){
        emit message("MainWindow::updatePerspectiveSwitcher(const QString& perspectiveName)");
        QSignalBlocker Blocker(m_perspectiveComboBox);
        m_perspectiveComboBox->clear();
        QStringList perspectives = m_dockManager->perspectiveNames();
        if (perspectives.size() > 0) {
            m_perspectiveComboBox->addItems(perspectives);
        }
        m_perspectiveComboBox->setCurrentText(perspectiveName);
    }

    void newPerspective(bool checked = false) {
        emit message("MainWindow::newPerspective(bool checked = false)");
        QString perspectiveName = QInputDialog::getText(this, tr("Save Perspective"), tr("Enter unique name:"));
        if (perspectiveName.isEmpty()) {
            emit message("MainWindow::newPerspective: perspective name empty");
            return;
        }

        if (m_dockManager->perspectiveNames().contains(perspectiveName) == true) {
            emit message(QString("MainWindow::newPerspective: perspective %1 already exists").arg(perspectiveName));
            QMessageBox::critical(this, tr("Error perspective already exists"), tr("Perspective %1 already exists").arg(perspectiveName));
            return;
        }

        m_dockManager->addPerspective(perspectiveName);
        QSignalBlocker Blocker(m_perspectiveComboBox);
        m_perspectiveComboBox->clear();
        m_perspectiveComboBox->addItems(m_dockManager->perspectiveNames());
        m_perspectiveComboBox->setCurrentText(perspectiveName);

        savePerspectives();
    }

    void deletePerspective(const QString& name) {
        emit message("MainWindow::deletePerspective(const QString& name)");
        this->m_dockManager->removePerspective(name);
        updatePerspectiveSwitcher(m_currentPerspective);
    }

    void savePerspective(bool checked = false) {
        emit message("MainWindow::savePerspective(bool checked = false)");
        QString perspectiveName = m_perspectiveComboBox->currentText();
        m_dockManager->addPerspective(perspectiveName);
        //savePerspectives();
    }

    void restoringState() {
        emit message("MainWindow::restoringState()");
        m_dockManager->setEnabled(false);
    }
   
    void stateRestored() {
        emit message("MainWindow::stateRestored()");
        m_dockManager->setEnabled(true);
    }

private:

    std::optional<QString> windowTitleByInstance(const Widget* instance) const;

    int findFreeIndex(const QString& name) const;

    bool createWindowByName(const QString& name, bool newInstance = false);

    void createActions();
    
    void createStatusBar();
    
    //void writeSettings();
    
    void createToolbar();

    ads::CDockWidget* findChildWindow(const QString& name) const;
    
    ads::CDockWidget* findChildWindowByTitle(const QString& name) const;

    static constexpr const char* winTitle = "Automatron 3000";
    static constexpr const char* aboutText = "This example demonstrates how to write multiple "
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
    QAction *m_actionConnect = nullptr;
    QAction *m_actionDisconnect = nullptr;
    QAction *m_actionConfigure = nullptr;
    MLoader* m_plugins = nullptr;
    //QMdiSubWindow* m_current = nullptr;
    Logger* m_logger = nullptr;
    MainWindowSettings m_winSettings;
    QToolBar* m_tbar = nullptr;
    QComboBox* m_perspectiveComboBox = nullptr;
    QWidgetAction* m_perspectiveListAction = nullptr;
    QMenu* m_viewMenu = nullptr;
    QMenu* m_visualMenu = nullptr;
    QAction* m_toggleToolbar = nullptr;
    QAction* m_saveState = nullptr;
    QAction* m_restoreState = nullptr;
    QAction* m_newPerspective = nullptr;
    QAction* m_copyPerspective = nullptr;
    QAction* m_deletePerspective = nullptr;
    LoadingDialog m_loading;
    QString m_currentPerspective;
};

class CustomDockWidgetTab : public ads::CDockWidgetTab {

    Q_OBJECT

public:
    CustomDockWidgetTab(ads::CDockWidget* DockWidget, MainWindow* win)
        : CDockWidgetTab(DockWidget)
        , m_win(win){
    }

protected:
    void fillMenu(QMenu* Menu) const {
        ads::CDockWidgetTab::fillMenu(Menu);
        Plugin* plugin = dynamic_cast<Plugin*>(dockWidget()->widget());
        if (plugin != nullptr) {
            QObject* o = dynamic_cast<QObject*>(plugin);
            const QList<QAction*> actions = Menu->actions();
            QAction* settings = new QAction(tr("Settings"));
            Menu->insertAction(actions[actions.size() - 1], settings);
            Menu->insertSeparator(actions[actions.size() - 1]);
            QObject* w = dockAreaWidget()->parent();

            QObject::connect(settings, &QAction::triggered, [plugin, o, this]() {
                SettingsMdi* settings = plugin->settingsWindow();
                if (settings == nullptr) {
                    //TODO: log message here
                    return;
                }

                if (this->m_win->hasWindowByTitle(o->objectName() + "/Settings")) {
                    this->m_win->toggleWindow(o->objectName() + "/Settings");
                } else {
                    this->m_win->addSubWindow(settings, o->objectName() + "/Settings");
                }
            });
        }
    }
private:

    MainWindow* m_win = nullptr;
};

#endif
