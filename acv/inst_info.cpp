#include "inst_info.h"

#include <cassert>


inst_info::inst_info(QString const& id, QString const& plugin) :
	id_(id),
	plugin_(plugin)
{
	assert(!id_.isEmpty());
	assert(!plugin_.isEmpty());
}

inst_info& inst_info::operator=(inst_info const& b)
{
	id_ = b.id();
	plugin_ = b.plugin();
	return *this;
}

inst_option const* inst_info::option(QString const& k) const
{
	for (auto const& opt : options_) {
		if (opt.key == k)
			return &opt;
	}
	return nullptr;
}

void inst_info::option(inst_option const& opt)
{
	option(opt.key, opt.type, opt.value);
}

void inst_info::option(QString const& k, QString const& t, QString const& v)
{
	bool found = false;
	int idx = 0;
	for (int eidx = options_.size(); idx < eidx; ++idx) {
		if (options_[idx].key == k) {
			found = true;
			break;
		}
	}

	auto opt = inst_option(k, t, v);
	if (found)
		options_.replace(idx, opt);
	else
		options_.push_back(opt);
}
