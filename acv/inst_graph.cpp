#include "inst_graph.h"

#include <cassert>

#include <QDebug>
#include <QMetaMethod>


inst_graph::inst_graph()
{

}

inst_graph::~inst_graph()
{

}

QStringList inst_graph::list_instances() const
{
	QStringList all;
	for (auto const& inst_name : insts_.keys()) {
		all.push_back(inst_name);
	}

	return all;
}

bool inst_graph::add_instance(QString const& id, acv_plugin* inst)
{
	if (insts_.count(id) == 1)
		return false;

	insts_[id] = inst;
	return true;
}

bool inst_graph::add_cable(cable_info const& cable)
{
	assert(insts_.contains(cable.inst_a()));
	assert(insts_.contains(cable.inst_b()));
	assert(!cable.socket_a().isEmpty());
	assert(!cable.socket_b().isEmpty());

	auto const a = insts_[cable.inst_a()];
	auto const b = insts_[cable.inst_b()];
	auto sock_a = a->expose_socket(cable.socket_a());
	auto sock_b = b->expose_socket(cable.socket_b());

	if (sock_a.wires.isEmpty() && sock_b.wires.isEmpty())
		return false;

	if (sock_a.dir_out == sock_b.dir_out) {
		qWarning() << "socket" << cable.socket_a() << "of" << cable.inst_a()
			<< "and socket" << cable.socket_b() << "of" << cable.inst_b()
			<< "must have opposite directions";
		return false;
	}

	for (auto const& k : sock_a.wires.keys()) {
		if (!sock_b.wires.contains(k))
			continue;

		auto const& wire_a = sock_a.wires[k];
		auto const& wire_b = sock_b.wires[k];
		if (sock_a.dir_out)
			connect_by_name(a, wire_a, b, wire_b);
		else
			connect_by_name(b, wire_b, a, wire_a);
	}

	return true;
}

bool inst_graph::connect_by_name(QObject* emitter, QString const& sig,
						QObject* receiver, QString const& slot)
{
	int index = emitter->metaObject()->indexOfSignal(
			QMetaObject::normalizedSignature(qPrintable(sig)));
	if (index == -1) {
		qWarning() << "No signal" << sig << "in" << emitter->objectName();
		return false;
	}
	auto const& sig_method = emitter->metaObject()->method(index);

	index = receiver->metaObject()->indexOfSlot(
			 QMetaObject::normalizedSignature(qPrintable(slot)));
	if (index == -1) {
		qWarning() << "No slot" << slot << "in" << receiver->objectName();
		return false;
	}
	auto const& slot_method = receiver->metaObject()->method(index);
	QObject::connect(emitter, sig_method, receiver, slot_method);

	return true;
}
