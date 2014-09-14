// rtp_elevator_system_drawer.h

#ifndef __NE_RTP_ELEVATOR_SYSTEM_DRAWER_H
#define __NE_RTP_ELEVATOR_SYSTEM_DRAWER_H

#include "../rtp_system_drawer.h"
#include "rtp_elevator_system.h"


namespace Wt {
	class WRectF;
}

namespace rtp {

	class elevator_system_drawer:
		public animated_system_drawer
	{
	public:
		elevator_system_drawer(elevator_system const& sys);

	public:
		virtual void start() override;
		virtual Result advance() override;
		virtual void draw_system(Wt::WPainter& painter, options_t const& options) override;

	protected:
		typedef elevator_system::floor_t floor_t;
		typedef elevator_system::floor_t floor_count_t;
		typedef elevator_system::Direction Direction;

		enum class AnimationStage {
			Init,	// Should never be drawn in this state

			Arrivals,
			ExitDoorsOpening,
			GettingOff,
			ExitDoorsClosing,
			EntryDoorsOpening,
			GettingOn,
			EntryDoorsClosing,
			Moving,
			Final,

			Ended,	// Should never be drawn in this state
		};

		enum class QueueSide {
			Left,
			Right,
		};

	protected:
		floor_count_t get_num_floors() const;

		enum class SizeFormat {
			DisplayYProportion,
			Pixels,
		};

		double get_floor_height(SizeFormat fmt = SizeFormat::DisplayYProportion) const;
		double get_elevator_height(SizeFormat fmt = SizeFormat::DisplayYProportion) const;
		double get_elevator_width(SizeFormat fmt = SizeFormat::DisplayYProportion) const;
		double get_door_gap_width(SizeFormat fmt = SizeFormat::DisplayYProportion) const;
		double get_entry_width(SizeFormat fmt = SizeFormat::DisplayYProportion) const;
		double get_exit_width(SizeFormat fmt = SizeFormat::DisplayYProportion) const;
		double get_queue_height(SizeFormat fmt = SizeFormat::DisplayYProportion) const;
		double get_person_height(SizeFormat fmt = SizeFormat::DisplayYProportion) const;
		double get_person_width(SizeFormat fmt = SizeFormat::DisplayYProportion) const;
		double get_queue_gap_width(SizeFormat fmt = SizeFormat::DisplayYProportion) const;
		double get_queue_width(size_t length, SizeFormat fmt = SizeFormat::DisplayYProportion) const;

		// These return false if the stage should be skipped, true if it will proceed
		bool initialize_stage();
		void initialize_arrivals();
		void initialize_getting_off();
		void initialize_getting_on();
		void initialize_moving();

		Result advance_stage();
		void transition();

		// State information helpers
		floor_t initial_floor() const;
		floor_t final_floor() const;

		// These retrieve state information, taking into account the current animation stage
		size_t get_queue_length(floor_t floor, Direction dir) const;
		floor_t get_current_floor() const;
		size_t get_current_occupancy() const;

		Wt::WRectF floor_rect(floor_t floor, Wt::WRectF const& rc_side);
		Wt::WRectF queue_corridor_rect(Direction dir, Wt::WRectF const& rc_floor);
		Wt::WRectF queue_rect(size_t length, QueueSide side, Wt::WRectF const& rc_bounds);

		void draw_person(Wt::WPainter& painter, Wt::WRectF const& rc);
		void draw_queue(size_t length, Wt::WPainter& painter, Wt::WRectF rc);
		void draw_all_queues(Wt::WPainter& painter, Wt::WRectF const& rc);
		void draw_floor(Wt::WPainter& painter, Wt::WRectF const& rc);
		void draw_all_floors(Wt::WPainter& painter, Wt::WRectF const& rc);
		void draw_elevator(Wt::WPainter& painter, Wt::WRectF const& rc);
		void draw_arrivals(Wt::WPainter& painter, Wt::WRectF const& rc);
		void draw_getting_on(Wt::WPainter& painter, Wt::WRectF const& rc);
		void draw_getting_off(Wt::WPainter& painter, Wt::WRectF const& rc);

		void set_pixel_multiplier(double canvas_y);
		void reset_pixel_multiplier();
		double get_pix_multiplier(SizeFormat fmt) const;

	private:
		elevator_system const& m_sys;

		AnimationStage m_stage;
//		stage_data_t m_stage_data;
		size_t m_stage_step;
		size_t m_end_step;

		double m_pix_multiplier;
	};

}


#endif

