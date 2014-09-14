// rtp_elevator_system_drawer.cpp

#include "rtp_elevator_system_drawer.h"
#include "rtp_elevator_system.h"

#include <Wt/WPainter>
#include <Wt/WPaintDevice>

#include <boost/thread/lock_guard.hpp>


namespace rtp {

	auto const DisplayAspect = 1.0;
	auto const OnOffSideRatio = 2.0;

	// TODO: Currently per animation step, should be per second, and provide anim_drawer->set_frame_rate()
	auto const WalkingSpeed = 0.015;
	auto const DoorSpeed = 0.015;
	auto const ElevatorSpeed = 0.02;

	elevator_system_drawer::elevator_system_drawer(elevator_system const& sys):
		m_sys(sys),
		m_stage(AnimationStage::Init),
		m_stage_step(0),
		m_end_step(0),
		m_pix_multiplier(1.0)
	{

	}

	void elevator_system_drawer::start()
	{
		m_stage = AnimationStage::Init;
		advance();
	}
	
	animated_system_drawer::Result elevator_system_drawer::advance()
	{
		auto stage_result = advance_stage();
		if(stage_result == Result::Finished)
		{
			do
			{
				transition();

			} while(m_stage != AnimationStage::Ended && !initialize_stage());

			m_stage_step = 0;
			return m_stage == AnimationStage::Ended ? Result::Finished : Result::Ongoing;
		}
		else
		{
			return Result::Ongoing;
		}
	}

	animated_system_drawer::Result elevator_system_drawer::advance_stage()
	{
		if(m_stage == AnimationStage::Init)
		{
			return Result::Finished;
		}

		++m_stage_step;
		return m_stage_step == m_end_step ? Result::Finished : Result::Ongoing;
	}

	bool elevator_system_drawer::initialize_stage()
	{
		switch(m_stage)
		{
			case AnimationStage::Arrivals:
			initialize_arrivals();
			break;

			case AnimationStage::ExitDoorsOpening:
			case AnimationStage::ExitDoorsClosing:
			if(m_sys.m_transitions.num_got_off > 0)
			{
				auto const elevator_height = get_elevator_height();
				m_end_step = (size_t)(elevator_height / DoorSpeed) + 1;
			}
			else
			{
				m_end_step = 0;
			}
			break;

			case AnimationStage::EntryDoorsOpening:
			case AnimationStage::EntryDoorsClosing:
			if(m_sys.m_transitions.num_got_on > 0)
			{
				auto const elevator_height = get_elevator_height();
				m_end_step = (size_t)(elevator_height / DoorSpeed) + 1;
			}
			else
			{
				m_end_step = 0;
			}
			break;

			case AnimationStage::GettingOff:
			initialize_getting_off();
			break;

			case AnimationStage::GettingOn:
			initialize_getting_on();
			break;

			case AnimationStage::Moving:
			initialize_moving();
			break;

			case AnimationStage::Final:
			m_end_step = 1;	// Ensure at least a single frame is drawn, in case no other stages were required
			break;
		}

		return m_end_step != 0;
	}

	void elevator_system_drawer::initialize_arrivals()
	{
		auto const entry_width = get_entry_width();
		auto const person_width = get_person_width();
		auto const queue_gap_width = get_queue_gap_width();

		double max_distance = 0.0;
		for(size_t f = 0; f < m_sys.m_num_floors; ++f)
		{
			auto const& flr = m_sys.m_transitions.floor_arrivals[f];
			for(size_t dir = 0; dir < 2; ++dir)
			{
				auto const queuing = get_queue_length(f, (Direction)dir);//m_sys.m_state.floors[f].queues[dir].size();
				auto const arrival_count = flr[dir];
				if(arrival_count > 0)
				{
					auto dist = entry_width - queuing * (person_width + queue_gap_width);
					max_distance = std::max(max_distance, dist);
				}
			}
		}

		m_end_step = (size_t)(max_distance / WalkingSpeed);
		if(max_distance > 0.0)
		{
			// Round up
			++m_end_step;
		}
	}

	void elevator_system_drawer::initialize_getting_off()
	{
		auto const count = m_sys.m_transitions.num_got_off;
		if(count == 0)
		{
			m_end_step = 0;
			return;
		}

		auto const exit_width = get_exit_width();
		auto const door_gap = get_door_gap_width();
		auto const person_width = get_person_width();
		auto const queue_gap = get_queue_gap_width();

		auto const distance = door_gap + exit_width + count * person_width + (count - 1) * queue_gap;
		m_end_step = (size_t)(distance / WalkingSpeed) + 1;
	}

	void elevator_system_drawer::initialize_getting_on()
	{
		auto const count = m_sys.m_transitions.num_got_on;
		if(count == 0)
		{
			m_end_step = 0;
			return;
		}

		auto const door_gap = get_door_gap_width();
		auto const person_width = get_person_width();
		auto const queue_gap = get_queue_gap_width();

		auto const distance = door_gap + count * person_width + (count - 1) * queue_gap;
		m_end_step = (size_t)(distance / WalkingSpeed) + 1;
	}

	void elevator_system_drawer::initialize_moving()
	{
		if(!elevator_system::is_moving(m_sys.m_state.direction))
		{
			m_end_step = 0;
			return;
		}

		auto const floor_height = get_floor_height();
		m_end_step = (size_t)(floor_height / ElevatorSpeed) + 1;
	}

	void elevator_system_drawer::transition()
	{
		switch(m_stage)
		{
			case AnimationStage::Init:
			m_stage = AnimationStage::Arrivals;
			break;
			case AnimationStage::Arrivals:
			m_stage = AnimationStage::ExitDoorsOpening;
			break;
			case AnimationStage::ExitDoorsOpening:
			m_stage = AnimationStage::GettingOff;
			break;
			case AnimationStage::GettingOff:
			m_stage = AnimationStage::ExitDoorsClosing;
			break;
			case AnimationStage::ExitDoorsClosing:
			m_stage = AnimationStage::EntryDoorsOpening;
			break;
			case AnimationStage::EntryDoorsOpening:
			m_stage = AnimationStage::GettingOn;
			break;
			case AnimationStage::GettingOn:
			m_stage = AnimationStage::EntryDoorsClosing;
			break;
			case AnimationStage::EntryDoorsClosing:
			m_stage = AnimationStage::Moving;
			break;
			case AnimationStage::Moving:
			m_stage = AnimationStage::Final;
			break;
			case AnimationStage::Final:
			m_stage = AnimationStage::Ended;
			break;
		}
	}


	elevator_system_drawer::floor_t elevator_system_drawer::initial_floor() const
	{
		return elevator_system::get_prev_floor(m_sys.m_state.location, m_sys.m_state.direction);
	}
		
	elevator_system_drawer::floor_t elevator_system_drawer::final_floor() const
	{
		return m_sys.m_state.location;
	}

	size_t elevator_system_drawer::get_queue_length(floor_t floor, Direction dir) const
	{
		auto final_length = m_sys.m_state.floors[floor].queues[dir].size();
		auto arrived = m_sys.m_transitions.floor_arrivals[floor][dir];
		auto got_on = size_t{ 0 };
		if(floor == initial_floor() && dir == m_sys.m_state.direction)
		{
			got_on = m_sys.m_transitions.num_got_on;
		}

		if(m_stage == AnimationStage::Arrivals)
		{
			return final_length - arrived + got_on;
		}
		else if(m_stage <= AnimationStage::GettingOn)
		{
			return final_length + got_on;
		}
		else
		{
			return final_length;
		}
	}

	elevator_system_drawer::floor_t elevator_system_drawer::get_current_floor() const
	{
		if(m_stage <= AnimationStage::Moving)
		{
			return initial_floor();
		}
		else
		{
			return final_floor();
		}
	}

	size_t elevator_system_drawer::get_current_occupancy() const
	{
		if(m_stage <= AnimationStage::GettingOff)
		{
			return m_sys.m_state.occupancy + m_sys.m_transitions.num_got_off - m_sys.m_transitions.num_got_on;
		}
		else if(m_stage <= AnimationStage::GettingOn)
		{
			return m_sys.m_state.occupancy - m_sys.m_transitions.num_got_on;
		}
		else
		{
			return m_sys.m_state.occupancy;
		}
	}


	size_t elevator_system_drawer::get_num_floors() const
	{
		return m_sys.m_num_floors;
	}

	double elevator_system_drawer::get_floor_height(SizeFormat fmt) const
	{
		return 1.0 / get_num_floors() * get_pix_multiplier(fmt);
	}

	double elevator_system_drawer::get_elevator_height(SizeFormat fmt) const
	{
		return get_floor_height() * get_pix_multiplier(fmt);
	}

	double elevator_system_drawer::get_elevator_width(SizeFormat fmt) const
	{
		return get_elevator_height() * 0.7 * get_pix_multiplier(fmt);
	}

	double elevator_system_drawer::get_door_gap_width(SizeFormat fmt) const
	{
		return get_elevator_width() * 0.3 * get_pix_multiplier(fmt);
	}

	double elevator_system_drawer::get_entry_width(SizeFormat fmt) const
	{
		auto free_width = DisplayAspect - get_elevator_width() - 2 * get_door_gap_width();
		return free_width * (OnOffSideRatio / (OnOffSideRatio + 1.0)) * get_pix_multiplier(fmt);
	}

	double elevator_system_drawer::get_exit_width(SizeFormat fmt) const
	{
		auto free_width = DisplayAspect - get_elevator_width() - 2 * get_door_gap_width();
		return free_width * (1.0 / (OnOffSideRatio + 1.0)) * get_pix_multiplier(fmt);
	}

	double elevator_system_drawer::get_queue_height(SizeFormat fmt) const
	{
		return get_floor_height() * 0.4 * get_pix_multiplier(fmt);
	}
		
	double elevator_system_drawer::get_person_height(SizeFormat fmt) const
	{
		return get_queue_height() * 0.6 * get_pix_multiplier(fmt);
	}

	double elevator_system_drawer::get_person_width(SizeFormat fmt) const
	{
		return get_person_height() * 0.25 * get_pix_multiplier(fmt);
	}
		
	double elevator_system_drawer::get_queue_gap_width(SizeFormat fmt) const
	{
		return get_person_width() * 0.7 * get_pix_multiplier(fmt);
	}

	double elevator_system_drawer::get_queue_width(size_t length, SizeFormat fmt) const
	{
		if(length == 0)
		{
			return 0;
		}
		else
		{
			return (get_person_width() * length + get_queue_gap_width() * (length - 1)) * get_pix_multiplier(fmt);
		}
	}


	void elevator_system_drawer::set_pixel_multiplier(double canvas_y)
	{
		m_pix_multiplier = canvas_y;
	}

	void elevator_system_drawer::reset_pixel_multiplier()
	{
		m_pix_multiplier = 1.0;
	}

	double elevator_system_drawer::get_pix_multiplier(SizeFormat fmt) const
	{
		return fmt == SizeFormat::DisplayYProportion ? 1.0 : m_pix_multiplier;
	}


	void elevator_system_drawer::draw_person(Wt::WPainter& painter, Wt::WRectF const& rc)
	{
		painter.drawRect(rc);
	}

	void elevator_system_drawer::draw_queue(size_t length, Wt::WPainter& painter, Wt::WRectF rc)
	{
		auto const person_height = get_person_height(SizeFormat::Pixels);
		auto const person_width = get_person_width(SizeFormat::Pixels);
		auto const gap_width = get_queue_gap_width(SizeFormat::Pixels);

		auto const margin = (rc.height() - person_height) * 0.5;

		rc = Wt::WRectF(
			rc.left(),
			rc.top() + margin,
			person_width,
			person_height
			);
		double offset = person_width + gap_width;
		for(size_t i = 0; i < length; ++i)
		{
			draw_person(painter, rc);
			rc = Wt::WRectF(
				rc.left() + offset,
				rc.top(),
				rc.width(),
				rc.height()
				);
		}
	}

	Wt::WRectF elevator_system_drawer::floor_rect(floor_t floor, Wt::WRectF const& rc_side)
	{
		auto const floor_height = get_floor_height(SizeFormat::Pixels);
		return{
			rc_side.left(),
			rc_side.bottom() - (floor + 1) * floor_height,
			rc_side.width(),
			floor_height
		};
	}

	Wt::WRectF elevator_system_drawer::queue_corridor_rect(Direction dir, Wt::WRectF const& rc_floor)
	{
		auto const q_height = get_queue_height(SizeFormat::Pixels);
		return Wt::WRectF(
			rc_floor.left(),
			dir == Direction::Up ? rc_floor.top() : (rc_floor.bottom() - q_height),
			rc_floor.width(),
			q_height
			);
	}

	Wt::WRectF elevator_system_drawer::queue_rect(size_t length, QueueSide side, Wt::WRectF const& rc_bounds)
	{
		auto const person_width = get_person_width(SizeFormat::Pixels);
		auto const queue_gap = get_queue_gap_width(SizeFormat::Pixels);

		switch(side)
		{
			case QueueSide::Left:
			return Wt::WRectF(
				rc_bounds.left(),
				rc_bounds.top(),
				person_width * length + queue_gap * (length - 1),
				rc_bounds.height()
				);

			case QueueSide::Right:
			return Wt::WRectF(
				rc_bounds.right() - (person_width * length + queue_gap * (length - 1)),
				rc_bounds.top(),
				person_width * length + queue_gap * (length - 1),
				rc_bounds.height()
				);
		}
	}

	void elevator_system_drawer::draw_all_queues(Wt::WPainter& painter, Wt::WRectF const& rc)
	{
		auto const num_floors = get_num_floors();
		auto const floor_height = get_floor_height(SizeFormat::Pixels);
		auto const q_height = get_queue_height(SizeFormat::Pixels);

		for(elevator_system::floor_t floor = 0; floor < num_floors; ++floor)
		{
			auto rc_floor = floor_rect(floor, rc);

			for(size_t dir = 0; dir < 2; ++dir)
			{
				if(
					m_stage == AnimationStage::GettingOn &&
					floor == get_current_floor() &&
					dir == m_sys.m_state.direction
					)
				{
					continue;
				}

				auto length = get_queue_length(floor, (Direction)dir);
				if(length > 0)
				{
					auto rc_q_corridor = queue_corridor_rect((Direction)dir, rc_floor);
					auto rc_q = queue_rect(
						length,
						QueueSide::Right,
						rc_q_corridor
						);
					draw_queue(length, painter, rc_q);
				}
			}
		}
	}

	void elevator_system_drawer::draw_floor(Wt::WPainter& painter, Wt::WRectF const& rc)
	{
		painter.drawLine(rc.left(), rc.bottom(), rc.right(), rc.bottom());
	}

	void elevator_system_drawer::draw_all_floors(Wt::WPainter& painter, Wt::WRectF const& rc)
	{
		auto const num_floors = get_num_floors();

		for(elevator_system::floor_t floor = 0; floor < num_floors; ++floor)
		{
			draw_floor(painter, floor_rect(floor, rc));
		}
	}
	
	void elevator_system_drawer::draw_elevator(Wt::WPainter& painter, Wt::WRectF const& rc)
	{
		const auto floor_height = get_floor_height(SizeFormat::Pixels);
		const auto elevator_height = get_elevator_height(SizeFormat::Pixels);

		painter.save();

		Wt::WRectF rc_elevator(
			rc.left(),
			rc.bottom() - get_current_floor() * floor_height - elevator_height,
			rc.width(),
			elevator_height
			);

		if(m_stage == AnimationStage::Moving)
		{
			auto travelled = m_stage_step * ElevatorSpeed * m_pix_multiplier;
			travelled = std::min(travelled, floor_height);
			auto sign = m_sys.m_state.direction == Direction::Up ? 1.0 : -1.0;
			rc_elevator = Wt::WRectF(
				rc_elevator.left(),
				rc_elevator.top() - sign * travelled,
				rc_elevator.width(),
				rc_elevator.height()
				);
		}

		double exit_door_openness = 0.0;
		switch(m_stage)
		{
			case AnimationStage::ExitDoorsOpening:
			exit_door_openness = m_stage_step * DoorSpeed * m_pix_multiplier / elevator_height;
			exit_door_openness = std::min(exit_door_openness, 1.0);
			break;
			case AnimationStage::ExitDoorsClosing:
			exit_door_openness = 1.0 - m_stage_step * DoorSpeed * m_pix_multiplier / elevator_height;
			exit_door_openness = std::max(exit_door_openness, 0.0);
			break;
			case AnimationStage::GettingOff:
			exit_door_openness = 1.0;
			break;
		}

		double entry_door_openness = 0.0;
		switch(m_stage)
		{
			case AnimationStage::EntryDoorsOpening:
			entry_door_openness = m_stage_step * DoorSpeed * m_pix_multiplier / elevator_height;
			entry_door_openness = std::min(entry_door_openness, 1.0);
			break;
			case AnimationStage::EntryDoorsClosing:
			entry_door_openness = 1.0 - m_stage_step * DoorSpeed * m_pix_multiplier / elevator_height;
			entry_door_openness = std::max(entry_door_openness, 0.0);
			break;
			case AnimationStage::GettingOn:
			entry_door_openness = 1.0;
			break;
		}

		painter.drawLine(rc_elevator.topLeft(), rc_elevator.topRight());
		painter.drawLine(rc_elevator.bottomLeft(), rc_elevator.bottomRight());

		painter.drawLine(
			rc_elevator.left(),
			rc_elevator.top(),
			rc_elevator.left(),
			rc_elevator.top() + (1.0 - entry_door_openness) * elevator_height
			);
		painter.drawLine(
			rc_elevator.right(),
			rc_elevator.top(),
			rc_elevator.right(),
			rc_elevator.top() + (1.0 - exit_door_openness) * elevator_height
			);

		if(m_sys.is_moving(m_sys.m_state.direction))
		{
			painter.save();
			painter.translate(rc_elevator.left(), rc_elevator.top());
			if(m_sys.m_state.direction == elevator_system::Direction::Down)
			{
				painter.translate(0.0, rc_elevator.height() * 0.5);
				painter.scale(1.0, -1.0);
			}
			painter.scale(rc_elevator.width(), rc_elevator.height());
			std::array< Wt::WPointF, 3 > points{ {
					{ 0.5, 0.1 },
					{ 0.2, 0.4 },
					{ 0.8, 0.4 },
					} };
			painter.setPen(Wt::WPen(Wt::green));
			painter.setBrush(Wt::WBrush(Wt::green));
			painter.drawPolygon(&points[0], 3);
			painter.restore();
		}

		Wt::WFont font = painter.font();
		//font.setFamily(Wt::WFont::Default);
		font.setSize(20);
		painter.setFont(font);
		painter.drawText(
			Wt::WRectF(rc_elevator.left(), rc_elevator.center().y(), rc_elevator.width(), rc_elevator.height() * 0.5),
			Wt::AlignCenter | Wt::AlignMiddle,
			std::to_string(get_current_occupancy())
			);

		painter.restore();
	}

	void elevator_system_drawer::draw_arrivals(Wt::WPainter& painter, Wt::WRectF const& rc)
	{
		auto const num_floors = get_num_floors();

		for(elevator_system::floor_t floor = 0; floor < num_floors; ++floor)
		{
			auto rc_floor = floor_rect(floor, rc);

			for(size_t direction = 0; direction < 2; ++direction)
			{
				auto num_arrivals = m_sys.m_transitions.floor_arrivals[floor][direction];
				if(num_arrivals > 0)
				{
					auto rc_q_corridor = queue_corridor_rect((Direction)direction, rc_floor);
					auto rc_q = queue_rect(
						num_arrivals,
						QueueSide::Left,
						rc_q_corridor
						);

					auto existing_length = get_queue_length(floor, (Direction)direction);
					auto walked = WalkingSpeed * m_stage_step * m_pix_multiplier;
					auto max = rc_floor.width();
					max -= get_queue_width(existing_length, SizeFormat::Pixels);
					if(existing_length > 0)
					{
						max -= get_queue_gap_width(SizeFormat::Pixels);
					}
					walked = std::min(walked, max);

					rc_q = Wt::WRectF(
						rc_q_corridor.left() - rc_q.width() + walked,
						rc_q.top(),
						rc_q.width(),
						rc_q.height()
						);
					draw_queue(num_arrivals, painter, rc_q);
				}
			}
		}
	}

	void elevator_system_drawer::draw_getting_on(Wt::WPainter& painter, Wt::WRectF const& rc)
	{
		auto const floor = get_current_floor();
		Direction const dir = m_sys.m_state.direction;
		auto count = m_sys.m_transitions.num_got_on;

		auto rc_floor = floor_rect(floor, rc);
		auto rc_q_corridor = queue_corridor_rect(dir, rc_floor);
		auto rc_q = queue_rect(
			count,
			QueueSide::Right,
			rc_q_corridor
			);

		auto walked = WalkingSpeed * m_stage_step * m_pix_multiplier;
		rc_q = Wt::WRectF(
			rc_q.left() + walked,
			rc_q.top(),
			rc_q.width(),
			rc_q.height()
			);

		painter.save();
		Wt::WPainterPath clip_path;
		clip_path.addRect(
			rc_q_corridor.left(),
			rc_q_corridor.top(),
			rc_q_corridor.width() + get_door_gap_width(SizeFormat::Pixels),
			rc_q_corridor.height()
			);
		painter.setClipping(true);
		painter.setClipPath(clip_path);

		draw_queue(count, painter, rc_q);

		painter.restore();
	}

	void elevator_system_drawer::draw_getting_off(Wt::WPainter& painter, Wt::WRectF const& rc)
	{
		auto const floor = get_current_floor();
		auto count = m_sys.m_transitions.num_got_off;

		auto rc_floor = floor_rect(floor, rc);
		auto rc_q_corridor = queue_corridor_rect(Direction::Down, rc_floor);
		auto rc_q = queue_rect(
			count,
			QueueSide::Left,
			rc_q_corridor
			);

		auto walked = WalkingSpeed * m_stage_step * m_pix_multiplier;
		rc_q = Wt::WRectF(
			rc_q.left() - rc_q.width() - get_door_gap_width(SizeFormat::Pixels) + walked,
			rc_q.top(),
			rc_q.width(),
			rc_q.height()
			);

		painter.save();
		Wt::WPainterPath clip_path;
		clip_path.addRect(
			rc_q_corridor.left() - get_door_gap_width(SizeFormat::Pixels),
			rc_q_corridor.top(),
			rc_q_corridor.width() + get_door_gap_width(SizeFormat::Pixels),
			rc_q_corridor.height()
			);
		painter.setClipping(true);
		painter.setClipPath(clip_path);

		draw_queue(count, painter, rc_q);

		painter.restore();
	}

	void elevator_system_drawer::draw_system(Wt::WPainter& painter, options_t const& options)
	{
//		boost::lock_guard< async_system_drawer > guard(*this);

		size_t Margin = 10;

		painter.save();
		Wt::WPaintDevice* device = painter.device();

		Wt::WLength dev_width = device->width();
		Wt::WLength dev_height = device->height();
		
		double pix_width = dev_width.toPixels() - 2 * Margin;
		double pix_height = dev_height.toPixels() - 2 * Margin;
		auto avail_aspect = pix_width / pix_height;

		if(avail_aspect > DisplayAspect)
		{
			pix_width = pix_height * DisplayAspect;
		}
		else
		{
			pix_height = pix_width / DisplayAspect;
		}

		auto x_margin = (dev_width.toPixels() - pix_width) / 2.0;
		auto y_margin = (dev_height.toPixels() - pix_height) / 2.0;
		painter.setViewPort(
			x_margin,
			y_margin,
			pix_width,
			pix_height
			);
		painter.setWindow(
			0.0,
			0.0,
			pix_width,
			pix_height
			);

		set_pixel_multiplier(pix_height);

		auto const& st = m_sys.m_state;

		const auto entry_width = get_entry_width(SizeFormat::Pixels);
		const auto exit_width = get_exit_width(SizeFormat::Pixels);
		const auto door_gap = get_door_gap_width(SizeFormat::Pixels);
		const auto elevator_width = get_elevator_width(SizeFormat::Pixels);

		// On floor side
		Wt::WRectF rc_all_on_floors(
			0.0,
			0.0,
			entry_width,
			pix_height
			);
		draw_all_floors(painter, rc_all_on_floors);

		// Elevator shaft
		Wt::WRectF rc_shaft(entry_width + door_gap, 0.0, elevator_width, pix_height);
		draw_elevator(painter, rc_shaft);

		// Off floor side
		Wt::WRectF rc_all_off_floors(
			pix_width - exit_width,
			0.0,
			exit_width,
			pix_height
			);
		draw_all_floors(painter, rc_all_off_floors);

		// Queues
		draw_all_queues(painter, rc_all_on_floors);

		switch(m_stage)
		{
			case AnimationStage::Arrivals:
			draw_arrivals(painter, rc_all_on_floors);
			break;

			case AnimationStage::GettingOn:
			draw_getting_on(painter, rc_all_on_floors);
			break;

			case AnimationStage::GettingOff:
			draw_getting_off(painter, rc_all_off_floors);
			break;
		}

		/////
		Wt::WFont font = painter.font();
		font.setSize(30);
		painter.setFont(font);
		painter.drawText(
			Wt::WRectF(0.0, 0.0, 100.0, 30.0),
			Wt::AlignLeft | Wt::AlignTop,
			std::to_string((int)m_stage) + " / " + std::to_string(m_stage_step)
			);
		/////

		painter.restore();

		reset_pixel_multiplier();
	}

}



