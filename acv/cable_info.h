#ifndef ACV_cable_info_H
#define ACV_cable_info_H

#include <cassert>
#include <QString>


class cable_info {
public:
	cable_info(QString const& inst_a, QString const& socket_a,
				QString const& inst_b, QString const& socket_b)
		: inst_a_(inst_a), socket_a_(socket_a),
		inst_b_(inst_b), socket_b_(socket_b)
	{
		assert(!inst_a_.isEmpty());
		assert(!socket_a_.isEmpty());
		assert(!inst_b_.isEmpty());
		assert(!socket_b_.isEmpty());
	}
	cable_info& operator=(cable_info const& b)
	{
		 inst_a_	= b.inst_a();
		 socket_a_	= b.socket_a();
		 inst_b_	= b.inst_b();
		 socket_b_	= b.socket_b();
	}

	QString const& inst_a() const { return inst_a_; }
	QString const& socket_a() const { return socket_a_; }
	QString const& inst_b() const { return inst_b_; }
	QString const& socket_b() const { return socket_b_; }

private:
	QString inst_a_;
	QString socket_a_;
	QString inst_b_;
	QString socket_b_;
};

bool make_cable_from_strings(QString const& end0, QString const& end1,
							cable_info& w);

#endif //ACV_cable_info_H
