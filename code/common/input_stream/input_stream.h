// input_stream.h

#ifndef __WB_INPUT_STREAM_H
#define __WB_INPUT_STREAM_H

#include <string>
#include <vector>


namespace input {

	typedef std::string input_element_name;

	enum class InputElementType {
		Button,
		Axis,
	};

	struct btn_element_id
	{
		unsigned int idval;

		inline bool operator< (btn_element_id const& rhs) const
		{
			return idval < rhs.idval;
		}
	};

	struct axis_element_id
	{
		unsigned int idval;

		inline bool operator< (btn_element_id const& rhs) const
		{
			return idval < rhs.idval;
		}
	};

	struct btn_element_state
	{
		enum class ButtonState {
			Pressed,
			Unpressed,
		};

		ButtonState st;

		btn_element_state():
			st{ ButtonState::Unpressed }
		{}
	};

	struct axis_element_state
	{
		float value;	// todo: maybe integer is better

		axis_element_state():
			value{ 0.0f }
		{}
	};

	typedef std::vector< input_element_name > input_element_name_list_t;
	
	typedef std::vector< btn_element_state > btn_state_list_t;
	typedef std::vector< axis_element_state > axis_state_list_t;

	/*
	TODO:
	Implement event based input (and possibly then implement generic way to build input state
	from event input).
	*/


	class input_stream_access;

	class input_stream
	{
	public:
		size_t button_count() const;
		size_t axis_count() const;

		input_element_name_list_t available_buttons() const;
		input_element_name_list_t available_axes() const;

	public:
		virtual void update() = 0;
		virtual input_stream_access lock() const;

	protected:
		btn_element_id register_button(input_element_name name);
		axis_element_id register_axis(input_element_name name);
		void initialize();

		inline btn_element_state const& btn_state(btn_element_id const& id) const
		{
			return btn_element_states_[id.idval];
		}

		inline axis_element_state const& axis_state(axis_element_id const& id) const
		{
			return axis_element_states_[id.idval];
		}

	protected:
		input_element_name_list_t btn_element_names_;	// index into vector is btn_element_id
		input_element_name_list_t axis_element_names_;	// index into vector is axis_element_id

		btn_state_list_t btn_element_states_;	// index into vector is btn_element_id
		axis_state_list_t axis_element_states_;	// index into vector is axis_element_id

		friend class input_stream_access;
	};


	class input_stream_access
	{
	public:
		inline btn_element_state const& operator[] (btn_element_id const& id) const
		{
			return btn_element_states_[id.idval];
		}

		inline axis_element_state const& operator[] (axis_element_id const& id) const
		{
			return axis_element_states_[id.idval];
		}

	private:
		input_stream_access(input_stream const& strm);

	protected:
		//input_stream const& strm_;
		btn_state_list_t btn_element_states_;
		axis_state_list_t axis_element_states_;

		friend class input_stream;
	};

}


#endif

