#include "acv.h"

#include <algorithm>
#include <memory>

#include <QDebug>
#include <QDir>
#include <QDomDocument>
#include <QLibrary>
#ifdef WITH_GUI
#include <QMainWindow>
#endif
#include <QPluginLoader>

#include "acv_plugin.h"
#include "cable_info.h"
#ifdef WITH_GUI
#include "gui.h"
#endif
#include "inst_graph.h"
#include "inst_info.h"
#include "plugin_factory.h"
#include "script_if.h"
#include "version.h"


using std::find_if;

acv::acv(int &argc, char **argv) :
#ifdef WITH_GUI
	QApplication(argc, argv)
#else
	QCoreApplication(argc, argv)
#endif
{
	setObjectName("acv");

	setOrganizationName("KAI Square Pte Ltd");
	setOrganizationDomain("kaisquare.com");
	setApplicationName("acv");
	setApplicationVersion(ACV_VERSION);

#ifdef WITH_GUI
	mainwnd_ = new Gui();
#endif

	auto plugindir = applicationDirPath() + "/" + DEFAULT_PLUGIN_DIR;
	plugin_factory_ = new plugin_factory(plugindir);

	inst_graph_ = new inst_graph();
	script_ = new script_if(this);
	script_->start_server(5555);
}

acv::~acv()
{
}

void acv::configure_cmdparser(QCommandLineParser& p)
{
	p.setApplicationDescription("KAI Square ACV");
	p.setSingleDashWordOptionMode(
			QCommandLineParser::ParseAsLongOptions);

	p.addOptions({
		{"i", "Input video source (file | http | rtsp)", "url"},
		{"recover", "Recover from input error"},
		{"tv", "Velocity threshold", "double"}
	});
}

void acv::process_cmdargs()
{
	QCommandLineParser p;
	// Default options
	auto const& help_opt = p.addHelpOption();
	auto const& ver_opt = p.addVersionOption();

	configure_cmdparser(p);

	if (!p.parse(arguments())) {
		QTextStream(stdout) << p.errorText();
		exit(-1);
	}

	if (p.isSet(help_opt)) {
		p.showHelp();
		Q_UNREACHABLE();
	}
	if (p.isSet(ver_opt)) {
		p.showVersion();
		Q_UNREACHABLE();
	}
	if (p.isSet("i")) {
		inst_option opt("source", "string", p.value("i"));
		config_.option("Frame In", opt);
	}
	if (p.isSet("recover")) {
		inst_option opt("recover_on_error", "boolean", "yes");
		config_.option("Frame In", opt);
	}
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
		if (!config_.insts().contains(instid)) {
			config_.insts().insert(instid, inst_info(instid, plugin));
		}
		auto it = config_.insts().find(instid);

		// Instance default config
		QDomElement optnode = instnode.firstChildElement("option");
		while (!optnode.isNull()) {
			auto const& attrs = optnode.attributes();
			auto const& name = attrs.namedItem("name").nodeValue();
			auto const& type = attrs.namedItem("type").nodeValue();
			auto value = attrs.namedItem("value").nodeValue();
			// Expand macros in option values
			value.replace("${APP_DIR}", applicationDirPath());
			it->option(name, type, value);

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

		auto it = find_if(config.cables().begin(), config.cables().end(),
				[&end0, &end1] (cable_info const& e) {
					return (e.inst_a() == end0 && e.inst_b() == end1)
						|| (e.inst_a() == end1 && e.inst_b() == end0);
				});
		if (it == config.cables().end()) {
			cable_info w(" ", " ", " ", " ");
			make_cable_from_strings(end0, end1, w);
			config.cables().push_back(w);
		}
	}
}

void acv::setup()
{
#ifdef WITH_GUI
	setup_ui();
#endif

	// Load all available plugins
	plugin_factory_->load_all();
	auto pluginlist = plugin_factory_->list_all();
	emit plugins_refreshed(pluginlist);

	// Create pre-configure and start all instances
	QStringList insts;
	for (auto const& info : config_.insts()) {
		auto inst = plugin_factory_->create_instance(info.id(), info.plugin());
		if (inst != 0) {
			inst->init();
			emit instance_created(inst);
			for (auto const& opt : info.options()) {
				if (opt.type == "string")
					QMetaObject::invokeMethod(inst, opt.key.toStdString().c_str(),
							Q_ARG(QString, opt.value));
				else if (opt.type == "number")
					QMetaObject::invokeMethod(inst, opt.key.toStdString().c_str(),
							Q_ARG(double, opt.value.toDouble()));
				else if (opt.type == "boolean") {
					bool v = false;
					if (opt.value == "yes")
						v = true;
					QMetaObject::invokeMethod(inst, opt.key.toStdString().c_str(),
							Q_ARG(bool, v));
				}

			}
			inst_graph_->add_instance(info.id(), inst);
		}
	}

	// Create cables
	for (auto const& cable : config_.cables()) {
		 qDebug() << cable.inst_a() << "." << cable.socket_a() << " -> "
				 << cable.inst_b() << "." << cable.socket_b();
		 inst_graph_->add_cable(cable);
	}
}

#ifdef WITH_GUI
void acv::setup_ui()
{
	mainwnd_->setup_connections(this);
	mainwnd_->show();
}
#endif
