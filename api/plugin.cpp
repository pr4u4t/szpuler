#include "plugin.h"

PluginPrivate::PluginPrivate(Loader* ld, PluginsLoader* plugins, const QString& path)
	: m_loader(ld)
	, m_plugins(plugins)
	, m_settingsPath(path) {
}

Plugin::Plugin(Loader* ld, PluginsLoader* plugins, const QString& path)
	:  m_d(ld, plugins, path){
	QRegExp rx("([0-9A-Fa-f]{8}-[0-9A-Fa-f]{4}-[0-9A-Fa-f]{4}-[0-9A-Fa-f]{4}-[0-9A-Fa-f]{12})");
	int idx;

	if (settingsPath().isEmpty()) {
		m_d.m_uuid = QUuid::createUuid().toString();
		m_d.m_settingsPath = QString(settings_path) + name() + "-" + m_d.m_uuid;
	} else if ((idx = rx.indexIn(path)) != -1) {
		m_d.m_uuid = "{" + rx.cap(1) + "}";
		m_d.m_settingsPath = QString(settings_path) + path;
	}
}

QString Plugin::name() const {
	return m_d.m_loader->name();
}

QString Plugin::version() const {
	return m_d.m_loader->version();
}

Plugin::Type Plugin::type() const {
	return m_d.m_loader->type();
}

PluginsLoader* Plugin::plugins() const {
	return m_d.m_plugins;
}

QString Plugin::uuid() const {
	return m_d.m_uuid;
}

QString Plugin::author() const {
	return m_d.m_loader->author();
}

QString Plugin::description() const {
	return m_d.m_loader->description();
}

QStringList Plugin::depends() const {
	return m_d.m_loader->depends();
}

QString Plugin::settingsPath() const {
	return m_d.m_settingsPath;
}

Loader* Plugin::loader() const {
	return m_d.m_loader;
}

bool Plugin::multipleInstances() const {
	return m_d.m_loader->multipleInstances();
}



Extension::Extension(Loader* ld, PluginsLoader* plugins, QObject* parent, const QString& path)
	: Plugin(ld, plugins, path) {
}

Widget::Widget(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& path)
	: Plugin(ld, plugins, path)
	, MdiChild() {
}

LoaderPrivate::LoaderPrivate(const QString& name, Plugin::Type type, const QString& version,
	const QString& author, const QString& description, const QStringList& depends, bool multiple)
	: m_name(name)
	, m_type(type)
	, m_version(version)
	, m_author(author)
	, m_description(description)
	, m_depends(depends)
	, m_enabled(true)
	, m_multiple(multiple){
}

Loader::Loader(const QString& name, Plugin::Type type, const QString& version,
	const QString& author, const QString& description, const QStringList& depends, bool multiple)
	: m_d(name, type, version, author, description, depends, multiple){
	}

QString Loader::name() const {	
	return m_d.m_name;
}

Plugin::Type Loader::type() const {
	return m_d.m_type;
}

QString Loader::version() const {
	return m_d.m_version;
}

QString Loader::author() const {
	return m_d.m_author;
}

QString Loader::description() const {
	return m_d.m_description;
}

QStringList Loader::depends() const {
	return m_d.m_depends;
}

bool Loader::enabled() const {
	return m_d.m_enabled;
}

void Loader::setEnabled(bool enabled) {
	m_d.m_enabled = enabled;
}

bool Loader::multipleInstances() const {
	return m_d.m_multiple;
}

ModuleLoaderContext::ModuleLoaderContext() {
}

ModuleLoaderContext::~ModuleLoaderContext() {
}
