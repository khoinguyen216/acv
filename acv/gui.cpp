#include "gui.h"
#include "ui_mainwindow.h"
#include "acv.h"
#include "acv_plugin.h"

#include <QDebug>
#include <QLabel>


Gui::Gui(QWidget *parent) : QMainWindow(parent),
	ui_(new Ui_MainWindow())
{
	ui_->setupUi(this);

	plugins_model_.setStringList(*(new QStringList()));
	ui_->pluginListView->setModel(&plugins_model_);
	auto pp = ui_->pluginListView->palette();
	pp.setBrush(QPalette::Base, Qt::transparent);
	ui_->pluginListView->setPalette(pp);

	// Configure instance list view
	insts_model_.setStringList(*(new QStringList));;
	ui_->instListView->setModel(&insts_model_);
	auto pi = ui_->instListView->palette();
	pi.setBrush(QPalette::Base, Qt::transparent);
	ui_->instListView->setPalette(pi);
}

Gui::~Gui()
{
	delete ui_;
}

void Gui::setup_connections(acv *app)
{
	connect(app, SIGNAL(plugins_refreshed(QStringList const&)),
			this, SLOT(handle_plugins_refreshed(QStringList const&)));
	connect(app, SIGNAL(instance_created(acv_plugin *)),
			this, SLOT(handle_instance_created(acv_plugin *)));
}

void Gui::handle_plugins_refreshed(QStringList const& plugins)
{
	auto rows = plugins_model_.rowCount();
	plugins_model_.removeRows(0, rows);
	plugins_model_.insertRows(0, plugins.count());
	for (int i = 0; i < plugins.count(); ++i) {
		 plugins_model_.setData(plugins_model_.index(i), plugins[i]);
	}
}

void Gui::handle_instance_created(acv_plugin* inst)
{
	auto rows = insts_model_.rowCount();
	insts_model_.insertRows(rows, 1);
	insts_model_.setData(insts_model_.index(rows), inst->objectName());
	ui_->instListView->setFixedHeight(
			ui_->instListView->sizeHintForRow(0) * insts_model_.rowCount()
			+ 2 * ui_->instListView->frameWidth());
	insts_[inst->objectName()] = inst;
	connect_to_instance(inst);

	QFrame *frame = new QFrame();
	ui_->instStack->addWidget(frame);
	QMetaObject::invokeMethod(inst, "setupui", Qt::AutoConnection,
			Q_ARG(QWidget *, frame));
	inst_frames_[inst->objectName()] = frame;
}

void Gui::on_instListView_clicked(QModelIndex const& index)
{
	auto const& inst = insts_model_.data(index, Qt::DisplayRole).toString();;
	if (inst != active_inst_ && inst_frames_.count(inst) > 0) {
		ui_->instStack->setCurrentWidget(inst_frames_[inst]);
		ui_->activeInstLabel->setText(inst);
	}
}

void Gui::connect_to_instance(acv_plugin* inst)
{
}
