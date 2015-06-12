#ifndef DETECTOR_H
#define DETECTOR_H

#include <vector>

#include <QObject>
#include <QString>

#include <opencv2/opencv.hpp>

#include "ffld/Detection.h"
#include "ffld/HOGPyramid.h"
#include "ffld/Mixture.h"


class detector : public QObject {
	Q_OBJECT

public:
	detector();
	~detector();

public slots:
	void configure(	QString const&	model,
					unsigned const	interval,
					double const	threshold);
	void process_frame(cv::Mat const& f);

signals:
	void configure_status(QString const& msg);
	void process_frame_status(QString const& msg);

private:
	bool detect(cv::Mat const& image,
				double threshold, double overlap) const;


private:
	bool			configured_;
	double			threshold_;
	unsigned		interval_;
	unsigned		padding_ = 6;
	FFLD::Mixture	mixture_;
	std::vector<FFLD::Rectangle> rois_;
};

#endif // DETECTOR_H
