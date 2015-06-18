#ifndef INST_INFO_H
#define INST_INFO_H

#include <cassert>

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
	inst_info(QString const& id, QString const& plugin) :
		id_(id),
		plugin_(plugin)
	{
		assert(!id_.isEmpty());
		assert(!plugin_.isEmpty());
	}
	inst_info& operator=(inst_info const& b)
	{
		 id_ = b.id();
		 plugin_ = b.plugin();
		 return *this;
	}

	QString const& id() const { return id_; }
	QString const& plugin() const { return plugin_; }
	QVector<inst_option> const& options() const { return options_; }
	void option(QString const& k, QString const& t, QString const& v)
	{
		auto opt = inst_option(k, t, v);
		options_.push_back(opt);
	}

private:
	QString id_;
	QString plugin_;
	QVector<inst_option> options_;

};

#endif
