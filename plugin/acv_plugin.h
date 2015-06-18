#ifndef ACV_PLUGIN_H
#define ACV_PLUGIN_H

#include <map>
#include <string>

#include <QHash>
#include <QtPlugin>
#include <QString>


class QWidget;

struct plugin_socket {
	bool dir_out;
	QHash<QString, QString> wires;
};

class acv_plugin : public QObject {
public:
	virtual	~acv_plugin() {}

	virtual void init() = 0;
	virtual	plugin_socket const expose_socket(QString const& s) const = 0;

public slots:
#ifdef WITH_GUI
	virtual void setupui(QWidget *parent) = 0;
#endif
};

Q_DECLARE_INTERFACE(acv_plugin, "acv_plugin")

#endif
