#include "acv.h"

#include <algorithm>
#include <memory>

#include <QDebug>
#include <QDir>
#include <QDomDocument>
#include <QLibrary>
#include <QMainWindow>
#include <QPluginLoader>

#include "acv_plugin.h"
#include "cable_info.h"
#include "gui.h"
#include "inst_graph.h"
#include "inst_info.h"
#include "plugin_factory.h"
#include "version.h"

using std::find_if;


acv::acv(int &argc, char **argv) :
	QApplication(argc, argv),
	mainwnd_(new Gui())
{
	setObjectName("acv");

	setOrganizationName("KAI Square Pte Ltd");
	setOrganizationDomain("kaisquare.com");
	setApplicationName("acv");
	setApplicationVersion(ACV_VERSION);

	auto plugindir = applicationDirPath() + "/" + DEFAULT_PLUGIN_DIR;
	plugin_factory_ = new plugin_factory(plugindir);

	inst_graph_ = new inst_graph();
}

acv::~acv()
{
}

void acv::process_cmdargs()
{

}

void acv::load_config()
{
	auto default_path = applicationDirPath() + "/" + DEFAULT_CONFIG_FILE;
	auto user_path = applicationDirPath() + "/" + USER_CONFIG_FILE;

	load_config_from_xml(default_path, config_);
	load_config_from_xml(user_path, config_);
}

void acv::load_config_from_xml(QString const& path, acv_config& config)
{
	QDomDocument doc;
	QFile file(path);
	if (!file.open(QIODevice::ReadOnly) || !doc.setContent(&file))
		return;

	// Parse instances
	auto const& instnode_list = doc.elementsByTagName("instance");
	for (int i = 0; i < instnode_list.size(); ++i) {
		auto const& instnode = instnode_list.item(i);
		auto const& attrs = instnode.attributes();
		auto const& instid = attrs.namedItem("id").nodeValue();
		auto const& plugin = attrs.namedItem("plugin").nodeValue();

		// Instance id and plugin
		auto it = find_if(config.insts.begin(), config.insts.end(),
				[&instid] (inst_info const& e) {
					return e.id() == instid;
				});
		if (it == config.insts.end()) {
			config.insts.emplace_back(instid, plugin);
			it = config.insts.end();
			--it;
		} else {
			*it = inst_info(instid, plugin);
		}

		// Instance default config
		QDomElement optnode = instnode.firstChildElement("option");
		while (!optnode.isNull()) {
			auto const& attrs = optnode.attributes();
			auto const& name = attrs.namedItem("name").nodeValue();
			auto value = attrs.namedItem("value").nodeValue();
			// Expand macros in option values
			value.replace("${APP_DIR}", QApplication::applicationDirPath());
			it->config(name, value);

			optnode = optnode.nextSiblingElement("option");
		}
	}

	// Parse cables
	auto const& cablenode_list = doc.elementsByTagName("cable");
	for (int i = 0; i < cablenode_list.size(); ++i) {
		auto const& cablenode = cablenode_list.item(i);
		auto const& attrs = cablenode.attributes();
		auto const& end0 = attrs.namedItem("end0").nodeValue();
		auto const& end1 = attrs.namedItem("end1").nodeValue();

		auto it = find_if(config.cables.begin(), config.cables.end(),
				[&end0, &end1] (cable_info const& e) {
					return (e.inst_a() == end0 && e.inst_b() == end1)
						|| (e.inst_a() == end1 && e.inst_b() == end0);
				});
		if (it == config.cables.end()) {
			cable_info w(" ", " ", " ", " ");
			make_cable_from_strings(end0, end1, w);
			config.cables.push_back(w);
		}
	}
}

void acv::setup()
{
	setup_ui();

	// Load all available plugins
	plugin_factory_->load_all();
	auto pluginlist = plugin_factory_->list_all();
	emit plugins_refreshed(pluginlist);

	// Create pre-configure and start all instances
	QStringList insts;
	for (auto const& info : config_.insts) {
		auto inst = plugin_factory_->create_instance(info.id(), info.plugin());
		if (inst != 0) {
			emit instance_created(inst);
			for (auto const& conf : info.config()) {
				QMetaObject::invokeMethod(inst, conf.first.toStdString().c_str(),
						Q_ARG(QString, conf.second));
			}
			QMetaObject::invokeMethod(inst, "start");
			inst_graph_->add_instance(info.id(), inst);
		}
	}

	// Create cables
	for (auto const& cable : config_.cables) {
		 qDebug() << cable.inst_a() << "." << cable.socket_a() << " -> "
				 << cable.inst_b() << "." << cable.socket_b();
		 inst_graph_->add_cable(cable);
	}
}

void acv::setup_ui()
{
	mainwnd_->setup_connections(this);
	mainwnd_->show();
}
