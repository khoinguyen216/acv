#ifndef ACV_CONFIG_H
#define ACV_CONFIG_H

#include <string>
#include <vector>

#include "cable_info.h"
#include "inst_info.h"

class acv_config {
public:
	acv_config() {}
	~acv_config() {}

public:
	std::vector<inst_info> insts;
	std::vector<cable_info> cables;
};

#endif //ACV_CONFIG_H
