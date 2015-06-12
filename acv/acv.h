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

#define CONFIG_MAXSIZE (1024 * 1024)
#define DEFAULT_CONFIG_FILE	"acv.xml"
#define USER_CONFIG_FILE	"acv.user.xml"
#define DEFAULT_PLUGIN_DIR	"plugins"

class acv : public QApplication {
	Q_OBJECT

public:
	

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
