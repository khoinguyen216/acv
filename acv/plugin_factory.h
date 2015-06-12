#ifndef ACV_PLUGIN_FACTORY_H
#define ACV_PLUGIN_FACTORY_H

#include <QHash>
#include <QString>
#include <QStringList>
#include <QtCore/qpluginloader.h>


class acv_plugin;
class QPluginLoader;

class plugin_factory {
public:
	explicit plugin_factory(QString const& plugindir);
	~plugin_factory();

	void          load_all();
	QStringList   list_all();
	acv_plugin*	create_instance(QString const& name, QString const& plugin);

private:
	plugin_factory(plugin_factory const&);
	plugin_factory& operator=(plugin_factory const&);

	QPluginLoader* load_plugin(QString const& file);

private:
	QString plugindir_;
	QHash<QString, QPluginLoader *> plugins_;
};

#endif //ACV_PLUGIN_FACTORY_H
