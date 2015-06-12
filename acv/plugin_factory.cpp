#include "plugin_factory.h"

#include <QDebug>
#include <QDir>
#include <QPluginLoader>

#include "acv_plugin.h"


plugin_factory::plugin_factory(QString const& plugindir)
  : plugindir_(plugindir)
{
}

plugin_factory::~plugin_factory()
{
	for (auto& loader : plugins_) {
		loader->unload();
		delete loader;
	}
}

void plugin_factory::load_all()
{
	auto file_list = QDir(plugindir_).entryInfoList(
			QDir::Files | QDir::NoDotAndDotDot);
	for (auto const &info : file_list) {
		auto p = info.absoluteFilePath();
		QPluginLoader *ld;
		if ((ld = load_plugin(p)) != 0) {
			qDebug() << "Loaded plugin" << p;
			plugins_.insert(ld->metaData().value("IID").toString(), ld);
		} else
			qDebug() << p << "is not an acv plugin";
	}
}

QStringList plugin_factory::list_all()
{
	QStringList result;
	for (auto const& name : plugins_.keys()) {
		result.push_back(name);
	}
	return result;
}

acv_plugin* plugin_factory::create_instance(QString const& name,
											QString const& plugin)
{
	if (plugins_.count(plugin) != 1)
		return 0;

	auto inst = plugins_[plugin]->instance();
	inst->setObjectName(name);
	return static_cast<acv_plugin*>(inst);;
}

QPluginLoader *plugin_factory::load_plugin(QString const& file)
{
	if (!QLibrary::isLibrary(file))
		return 0;

	auto loader = new QPluginLoader(file);

	// Try creating an instance to see if it is really of our class
	auto inst = loader->instance();
	if (qobject_cast<acv_plugin *>(inst) == 0) {
		delete loader;
		loader = 0;
	}

	delete inst;

	return loader;
}
