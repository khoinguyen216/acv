#ifndef ACV_INST_GRAPH_H
#define ACV_INST_GRAPH_H

#include <QHash>
#include <QString>
#include <QStringList>

#include "acv_plugin.h"
#include "cable_info.h"


class inst_graph {
public:
	inst_graph();
	~inst_graph();

	QStringList list_instances() const;

	bool add_instance(QString const& id, acv_plugin* inst);
	bool add_cable(cable_info const& cable);

private:
	inst_graph(inst_graph const&);
	inst_graph& operator=(inst_graph const&);

private:
	bool connect_by_name(QObject* emmitter, QString const& sig,
						QObject* receiver, QString const& slot);

private:
	QHash<QString, acv_plugin*> insts_;
};


#endif //ACV_INST_GRAPH_H
