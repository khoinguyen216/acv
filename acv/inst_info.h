#ifndef INST_INFO_H
#define INST_INFO_H

#include <QHash>
#include <QString>
#include <QVector>


struct inst_option {
	QString key;
	QString type;
	QString value;

	inst_option() {}

	inst_option(QString const& k, QString const& t, QString const& v)
	{
		key = k;
		type = t;
		value = v;
	}
};

class inst_info {
public:
	inst_info(QString const& id, QString const& plugin);
	inst_info& operator=(inst_info const& b);

	QString const& id() const { return id_; }
	QString const& plugin() const { return plugin_; }
	QVector<inst_option> const& options() const { return options_; }
	inst_option const* option(QString const& k) const;
	void option(inst_option const& opt);
	void option(QString const& k, QString const& t, QString const& v);

private:
	QString id_;
	QString plugin_;
	QVector<inst_option> options_;
};

#endif
