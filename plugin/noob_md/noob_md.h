#ifndef noob_md_H
#define noob_md_H

#include "acv_plugin.h"

#include <vector>

#include <opencv2/opencv.hpp>

#ifdef WITH_GUI
#include <QGraphicsScene>
#include <QImage>
#include <QPixmap>
#include <QThread>
#endif


#ifdef WITH_GUI
class Ui_NoobMdUi;
#endif
class md_worker;

class noob_md : public acv_plugin {
	Q_OBJECT
	Q_INTERFACES(acv_plugin)
	Q_PLUGIN_METADATA(IID "noob_md")

public:
	noob_md();
	~noob_md();
	plugin_socket const expose_socket(QString const& s) const override;
	plugin_options const& options() const override;
	void init() override;

public slots:
#ifdef WITH_GUI
	void setupui(QWidget* parent) override;
	void visualize_result(cv::Mat const& fr);
#endif
	void process_frame(cv::Mat const& fr, double ts);

signals:
	void detected(std::vector<cv::Rect> boxes);

private:
#ifdef WITH_GUI
	QImage	cvMat_to_QImage(cv::Mat const& m);
	QPixmap	cvMat_to_QPixmap(cv::Mat const& m);
#endif

private:
	QHash<QString, plugin_socket>	sockets_;
	plugin_options		options_;
	QThread*			worker_thread_ = 0;
	md_worker*			worker_ = 0;

#ifdef WITH_GUI
	Ui_NoobMdUi*		ui_ = 0;
	QGraphicsScene		scene_;
#endif
};

#endif
