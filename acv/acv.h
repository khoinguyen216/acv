#ifndef ACV_H
#define ACV_H

#include <QHash>
#include <QApplication>
#include <QPluginLoader>
#include <QMainWindow>
#include <QDomDocument>

#include "acv_config.h"


class QPluginLoader;

class acv_plugin;
class framein_plugin;
class Gui;
class inst_graph;
class plugin_factory;


class acv : public QApplication {
	Q_OBJECT

public:
	static uint32_t const CONFIG_MAXSIZE = 1024 * 1024;
	static constexpr char const* DEFAULT_CONFIG_FILE = "acv.xml";
	static constexpr char const* USER_CONFIG_FILE = "acv.user.xml";
	static constexpr char const* DEFAULT_PLUGIN_DIR = "plugins";

public:
	acv(int &argc, char **argv);
	~acv();

	void process_cmdargs();
	void load_config();
	void setup();

signals:
	void plugins_refreshed(QStringList const& plugins);
	void instance_created(acv_plugin* inst);

private:
	void load_config_from_xml(QString const& path, acv_config& config);
	void setup_ui();

private:
	acv_config			config_;
	plugin_factory*		plugin_factory_;
	inst_graph*			inst_graph_;

	Gui*    mainwnd_;
};


#endif
