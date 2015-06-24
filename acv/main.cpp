#include <iostream>

#include <boost/filesystem.hpp>

#include "acv.h"


using namespace std;
namespace fs = boost::filesystem;

extern void ffmpeg_link();
extern void opencv_link();

int main(int argc, char **argv)
{
	fs::path p("abc");
	fs::exists(p);

	ffmpeg_link();
	opencv_link();

	acv app(argc, argv);
	app.load_config();
	app.setup();

	return app.exec();
}
