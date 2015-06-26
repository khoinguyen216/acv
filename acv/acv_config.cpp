#include "acv_config.h"


void acv_config::option(QString const& inst, inst_option const& opt)
{
	auto iter = insts_.find(inst);
	if (iter == insts_.end())
		return;
	iter->option(opt);
}

inst_option const* acv_config::option(QString const& inst, QString const& k)
	const
{
	auto iter = insts_.find(inst);
	return (iter == insts_.end() ? nullptr : iter->option(k));
}
