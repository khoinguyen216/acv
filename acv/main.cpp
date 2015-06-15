#include <iostream>

#include <boost/filesystem.hpp>

#include "acv.h"


using namespace std;
namespace fs = boost::filesystem;

int main(int argc, char **argv)
{
	fs::path p("abc");
	fs::exists(p);
	
	acv app(argc, argv);
	app.load_config();
	app.setup();

	return app.exec();
}
