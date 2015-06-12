#ifndef INST_INFO_H
#define INST_INFO_H

#include <cassert>

#include <QPair>
#include <QString>
#include <QVector>


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
	}

	QString const& id() const { return id_; }
	QString const& plugin() const { return plugin_; }
	QVector<QPair<QString, QString>> const& config() const { return config_; }
	void config(QString const& k, QString const& v)
	{
		config_.push_back(QPair<QString, QString>(k, v));
	}

private:
	QString id_;
	QString plugin_;
	QVector<QPair<QString, QString>> config_;

};

#endif
