#ifndef ACV_GUI_H
#define ACV_GUI_H

#include <QFrame>
#include <QHash>
#include <QMainWindow>
#include <QStringList>
#include <QStringListModel>
#include <acv_plugin.h>

class Ui_MainWindow;

class acv;
class acv_plugin;


class Gui : public QMainWindow {
	Q_OBJECT

public:
	explicit Gui(QWidget *parent = 0);
	~Gui();

	void setup_connections(acv *app);

public slots:
	// Handle events from acv
	void handle_plugins_refreshed(QStringList const& plugins);
	void handle_instance_created(acv_plugin* inst);

	// Handle events from UI
	void on_instListView_clicked(QModelIndex const& index);

private:
	void connect_to_instance(acv_plugin* inst);

private:
	Ui_MainWindow		*ui_;
	QStringListModel	plugins_model_;
	QStringListModel	insts_model_;
	QString				active_inst_;

	QHash<QString, acv_plugin *> insts_;
	QHash<QString, QFrame *> inst_frames_;
};


#endif //ACV_GUI_H
