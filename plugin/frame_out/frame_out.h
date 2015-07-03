#ifndef FRAME_OUT_H
#define FRAME_OUT_H

#include "acv_plugin.h"


class video_sink;
#ifdef WITH_GUI
class Ui_FrameOutUi;
#endif

class frame_out : public acv_plugin {
	Q_OBJECT
	Q_INTERFACES(acv_plugin)
	Q_PLUGIN_METADATA(IID "frame_out")

public:
	frame_out();
	~frame_out();

	plugin_socket const expose_socket(QString const& s) const override;
	plugin_options const& options() const override;
	void init() override;

public slots:
	// Plugin options
	void destination(QString const& v);
	void start();
	void stop();

signals:
	// Status
	void started();
	void stopped();

private:
	QHash<QString, plugin_socket>	sockets_;
	plugin_options	options_;
	QString			destination_;
	video_sink*		sink_			= nullptr;
	QThread*		workthread_		= nullptr;

#ifdef WITH_GUI
public slots:
	void setupui(QWidget* parent) override;

private:
	Ui_FrameOutUi*	ui_ = nullptr;

#endif
};

#endif
