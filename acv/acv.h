#ifndef ACV_H
#define ACV_H

#include <QCommandLineParser>
#ifdef WITH_GUI
#include <QApplication>
#include <QMainWindow>
#else
#include <QCoreApplication>
#endif

#include "acv_config.h"


#ifdef WITH_GUI
class Gui;
#endif
class acv_plugin;
class inst_graph;
class plugin_factory;
class script_if;

#define CONFIG_MAXSIZE			(1024 * 1024)
#define DEFAULT_CONFIG_FILE		"acv.xml"
#define USER_CONFIG_FILE		"acv.user.xml"
#define DEFAULT_PLUGIN_DIR		"plugin"

#ifdef WITH_GUI
class acv : public QApplication
#else
class acv : public QCoreApplication
#endif
{
	Q_OBJECT

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
	void configure_cmdparser(QCommandLineParser& p);
	void load_config_from_xml(QString const& path, acv_config& config);
	void setup_ui();

private:
	acv_config			config_;
	plugin_factory*		plugin_factory_;
	inst_graph*			inst_graph_;
	script_if*			script_;

#ifdef WITH_GUI
	Gui*    mainwnd_;
#endif
};


#endif
