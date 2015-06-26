#ifndef ACV_CONFIG_H
#define ACV_CONFIG_H

#include <QHash>
#include <QString>
#include <QVector>

#include "cable_info.h"
#include "inst_info.h"


class acv_config {
public:
	acv_config() {}
	~acv_config() {}

public:
	QHash<QString, inst_info>& insts() { return insts_; };
	QVector<cable_info>& cables() { return cables_; };
	void option(QString const& inst, inst_option const& opt);
	inst_option const* option(QString const& inst, QString const& k) const;

private:
	QHash<QString, inst_info> insts_;
	QVector<cable_info> cables_;
};

#endif //ACV_CONFIG_H
