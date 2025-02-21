#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QDialog>
#include <QIntValidator>
#include <QSettings>
#include <QHash>
#include <QStandardItemModel>

#include "../api/api.h"
#include "../core/core.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class Preferences;
}

//class QIntValidator;
QT_END_NAMESPACE

class PreferencePage;

class PageLoader {
public:

    PageLoader(const QString& name, const QPixmap& icon) 
        : m_name(name)
        , m_icon(icon){
    };

    virtual PreferencePage* load() const = 0;

    virtual ~PageLoader() = default;

    QString name() const {
        return m_name;
    }

    QPixmap icon() const {
        return m_icon;
    }

private:

    QString m_name;
    QPixmap m_icon;
};

template<typename T>
class PreferencePageLoader : public PageLoader{
public:

    PreferencePageLoader(const QString& name, const QPixmap& icon)
        : PageLoader(name, icon) {}

    PreferencePage* load() const {
        return new T(this);
    }
};

class Preferences : public SettingsMdi {

    Q_OBJECT

public:

    Preferences(QWidget* mwin, Loader* loader, const QString& settingsPath, MLoader* plugins);

    ~Preferences();

    bool saveSettings() {
        return true;
    }

    static void registerPage(PageLoader* ld) {
        m_pageLoaders.append(ld);
    }

private slots:
    void apply();
    void cancel();
    void ok();
    void settingsChanged() {}
    void clicked(const QModelIndex& index);
protected:

    void updateSettings();

private:
    Ui::Preferences* m_ui = nullptr;
    QIntValidator* m_intValidator = nullptr;
    static QList<PageLoader*> m_pageLoaders;
    MLoader* m_plugins = nullptr;
    QStandardItemModel* m_model = nullptr;
};

#define REGISTER_PREFERENCE_PAGE(name, icon) \
	PreferencePageLoader<name> name##PreferencePage(#name, icon); \
	RegisterPreferencePage name##RegisterPreferencePage(&name##PreferencePage);

class RegisterPreferencePage {
public:
    RegisterPreferencePage(PageLoader* ld) {
        Preferences::registerPage(ld);
    }
};

#endif
