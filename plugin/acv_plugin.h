#ifndef ACV_PLUGIN_H
#define ACV_PLUGIN_H

#include <map>
#include <string>

#include <QHash>
#include <QtPlugin>
#include <QString>
#include <QVector>


class QWidget;

struct plugin_socket {
	bool dir_out;
	QHash<QString, QString> wires;
};

struct plugin_option {
	QString name;
	QString type;
	QString desc;
};

typedef QVector<plugin_option> plugin_options;

class acv_plugin : public QObject {
public:
	acv_plugin() {}
	virtual	~acv_plugin() {}

	virtual void init() = 0;
	virtual	plugin_socket const expose_socket(QString const& s) const = 0;
	virtual plugin_options const& options() const = 0;

// Disable copy constructor and assignment operator
private:
	acv_plugin(acv_plugin const&);
	acv_plugin& operator=(acv_plugin const&);

public slots:
#ifdef WITH_GUI
	virtual void setupui(QWidget *parent) = 0;
#endif
};

Q_DECLARE_INTERFACE(acv_plugin, "acv_plugin")

#endif
