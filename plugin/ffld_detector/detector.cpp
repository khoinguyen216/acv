#include "detector.h"

#include <cstdio>
#include <fstream>
#include <utility>

#include "ffld/Detection.h"
#include "ffld/Model.h"
#include "ffld/Patchwork.h"


detector::detector() :
	configured_(false)
{
	rois_.push_back(FFLD::Rectangle(41, 229, 63, 107));
	rois_.push_back(FFLD::Rectangle(110, 247, 70, 91));
	rois_.push_back(FFLD::Rectangle(196, 240, 80, 104));
	rois_.push_back(FFLD::Rectangle(275, 236, 88, 107));
}

detector::~detector()
{

}

void detector::configure(	QString const&	model,
							unsigned const	interval,
							double const	threshold)
{
	interval_ = interval;
	threshold_ = threshold;

	std::ifstream f(model.toStdString(), std::ios::binary);
	if (!f.is_open())
		goto model_failed;

	f >> mixture_;
	if (mixture_.empty())
		goto model_failed;

	mixture_.cacheFilters();

	// TODO: remove
	for (size_t i = 0, e = mixture_.models().size(); i < e; ++i) {
		auto const& root_sz = mixture_.models()[i].rootSize();
		auto const& part_sz = mixture_.models()[i].partSize();
		printf("Model %zu root_size %dx%d part_size %dx%d\n", i,
				root_sz.first, root_sz.second,
				part_sz.first, part_sz.second);
	}
	// TODO

	configured_ = true;
	return;

model_failed:
	configured_ = false;
	emit configure_status("Could not load model from file " + model);
	return;
}

void detector::process_frame(cv::Mat const& f)
{
	if (!configured_)
		return;

	// Extract patches
	auto r = rois_[0];
	auto patch = f.rowRange(r.y(), r.y() + r.height())
					.colRange(r.x(), r.x() + r.width());
	cv::imshow("test", patch);

	detect(patch, threshold_, 0.5);
}

bool detector::detect(cv::Mat const& patch,
		double threshold, double overlap) const
{
	using std::pair;
	using std::vector;
	using FFLD::Detection;
	using FFLD::HOGPyramid;
	using FFLD::Model;
	using FFLD::Mixture;
	using FFLD::Patchwork;
	using FFLD::Rectangle;

	bool detected;

	HOGPyramid pyra(patch, padding_, padding_, 2);
	if (pyra.empty())
		return false;

	if (!Patchwork::InitFFTW((pyra.levels()[0].rows() - padding_ + 15) & ~15,
				(pyra.levels()[0].cols() - padding_ + 15) & ~15)) {
		 printf("Could not initialize the Patchwork class\n");
		 return false;
	}

	vector<Detection> dets;
	vector<HOGPyramid::Matrix> scores;
	vector<Mixture::Indices> argmaxes;
	vector<vector<vector<Model::Positions>>> positions;

	mixture_.convolve(pyra, scores, argmaxes, &positions);


	return detected;
}
