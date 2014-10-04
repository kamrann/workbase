// rtp_radial_detection_system.cpp

#include "rtp_radial_detection_system.h"
#include "../rtp_system_agents_data.h"
#include "../rtp_controller.h"
#include "../../rtp_evolvable_controller.h"
#include "../../rtp_composite_observer.h"
#include "../../rtp_resultant_objective.h"
#include "../../rtp_genome_mapping.h"
#include "../../rtp_properties.h"
#include "rtp_rd_system_drawer.h"

#include "wt_param_widgets/pw_yaml.h"
#include "wt_param_widgets/schema_builder.h"
#include "wt_param_widgets/param_accessor.h"

#include <Wt/WComboBox>


namespace rtp {

	std::string const DetectorTypeNames[rd_system::DetectorType::Count] = {
		"Direction (X/Y)",
		"Direction (Angle)",
		"Collision Probability",
	};

}

namespace YAML {

	template <>
	struct convert < rtp::rd_system::DetectorType >
	{
		static Node encode(rtp::rd_system::DetectorType const& rhs)
		{
			return Node(rtp::DetectorTypeNames[rhs]);
		}

		static bool decode(Node const& node, rtp::rd_system::DetectorType& rhs)
		{
			if(!node.IsScalar())
			{
				return false;
			}

			auto it = mapping_.find(node.Scalar());
			if(it == mapping_.end())
			{
				return false;
			}

			rhs = it->second;
			return true;
		}

		static std::map< std::string, rtp::rd_system::DetectorType > const mapping_;
	};

//	template <>
	std::map< std::string, rtp::rd_system::DetectorType > const convert < rtp::rd_system::DetectorType >::mapping_ = {
			{ "Direction (X/Y)", rtp::rd_system::DetectorType::SingleObjectDirectionXY },
			{ "Direction (Angle)", rtp::rd_system::DetectorType::SingleObjectDirectionBearing },
			{ "Collision Probability", rtp::rd_system::DetectorType::SingleObjectCollisionProbability },
	};
}

namespace rtp {

	std::vector< std::string > rd_system::generate_state_value_names(size_t num_sensors)
	{
		std::vector< std::string > names;
		names.emplace_back("Object Bearing");
		names.emplace_back("Object Radius");
		names.emplace_back("Collision Probability");
		names.emplace_back("Estimated Bearing");
		names.emplace_back("Bearing Error");
		names.emplace_back("Estimated Collision Probability");
		names.emplace_back("Collision Probability Error");

		for(size_t s = 0; s < num_sensors; ++s)
		{
			auto sname = std::string{ "S" } + std::to_string(s + 1);

			names.emplace_back(sname + ": Positive");
			names.emplace_back(sname + ": Distance");
			names.emplace_back(sname + ": Radial Velocity");
			names.emplace_back(sname + ": Lateral Velocity");
		}

		return names;
	}

	std::map< std::string, agent_sensor_id > rd_system::generate_state_value_idmap(size_t num_sensors)
	{
		std::map< std::string, agent_sensor_id > mp;
		auto names = generate_state_value_names(num_sensors);
		for(size_t id = 0; id < names.size(); ++id)
		{
			mp[names[id]] = id;
		}
		return mp;
	}


	namespace sb = prm::schema;

	std::string rd_system::update_schema_providor(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix, bool evolvable)
	{
		auto relative = std::string{ "rd_params" };
		auto path = prefix + relative;

		(*provider)[path + std::string("detector_type")] = [](prm::param_accessor)
		{
			auto s = sb::enum_selection(
				"detector_type",
				std::vector< std::string >{ std::begin(DetectorTypeNames), std::end(DetectorTypeNames) },
				1,
				1
				);
			sb::label(s, "Detector Type");
			return s;
		};

		(*provider)[path + std::string("sensor_count")] = [](prm::param_accessor)
		{
			auto s = sb::integer("sensor_count", 4, 1);
			sb::label(s, "Sensors");
			return s;
		};

		auto controller_rel = i_controller::update_schema_providor(provider, path, evolvable);

		(*provider)[path] = [=](prm::param_accessor param_vals)
		{
			auto s = sb::list(relative);
			sb::append(s, provider->at(path + std::string("detector_type"))(param_vals));
			sb::append(s, provider->at(path + std::string("sensor_count"))(param_vals));

			if(!evolvable)
			{
				sb::append(s, provider->at(path + controller_rel)(param_vals));
			}

			sb::border(s, std::string{ "Radial Detection" });
			sb::enable_import(s, "system.rd");
			return s;
		};

		return relative;
	}

	std::unique_ptr< i_system_factory > rd_system::generate_factory(prm::param_accessor param_vals)
	{
		return std::make_unique< system_factory >([=]()
		{
			auto acc(param_vals);
			acc.push_relative_path(prm::qualified_path("rd_params"));
			return std::make_unique< rd_system >(acc);
		});
	}

	agents_data rd_system::generate_agents_data(prm::param_accessor param_vals, bool evolvable)
	{
		agents_data data;
		size_t const controller_id = 0;

		param_vals.push_relative_path(prm::qualified_path("rd_params"));

		std::map< std::string, size_t > evolved_map;
		if(evolvable)
		{
			auto evolved_path = param_vals.find_path("evolved_container");
			auto ev_acc = param_vals.from_absolute(evolved_path);
			auto evolvable_list_node = ev_acc["evolved_list"];
			for(auto const& node : evolvable_list_node)
			{
				auto inst_num = node.first.as< unsigned int >();
				auto rel = prm::qualified_path{ "evolved_list" };
				rel.leaf().set_index(inst_num);
				ev_acc.push_relative_path(rel);
				ev_acc.push_relative_path(std::string{ "evolved" });

				std::unique_ptr< i_controller_factory > controller_factory;
				std::unique_ptr< i_genome_mapping > mapping;
				std::tie(mapping, controller_factory) = evolvable_controller::create_instance(ev_acc);

				std::vector< std::unique_ptr< i_observer > > required_observations;
				std::unique_ptr< i_population_wide_observer > resultant_obj = resultant_objective::create_instance(ev_acc, required_observations);
				std::unique_ptr< i_observer > observer = composite_observer::create_instance(std::move(required_observations));
				data.controller_factories[controller_id] = std::move(controller_factory);
				data.evolved[controller_id].genome_mapping = std::move(mapping);
				data.evolved[controller_id].resultant_obj = std::move(resultant_obj);
				data.evolved[controller_id].observer = std::move(observer);

				ev_acc.pop_relative_path();
				ev_acc.pop_relative_path();
			}
		}
		else
		{
			data.controller_factories[controller_id] = i_controller::create_factory(param_vals);
			/*	std::make_unique< controller_factory >(
				[](i_agent const*)
			{
				return std::make_unique< dumb_elevator_controller >();
			});
			*/
		}

		agents_data::agent_instance_data inst_data;
		inst_data.spec = rd_agent_specification{};
		inst_data.controller_id = controller_id;

		data.agents.push_back(inst_data);

		param_vals.pop_relative_path();

		return data;
	}

	std::vector< std::string > rd_system::get_state_values(prm::param_accessor param_vals)
	{
//		auto const num_floors = param_vals["floor_count"].as< floor_count_t >();
//		return generate_state_value_names(num_floors);
		return{};
	}

	std::vector< std::string > rd_system::get_agent_type_names()
	{
		return{ std::begin(DetectorTypeNames), std::end(DetectorTypeNames) };
	}

	std::vector< std::string > rd_system::get_agent_sensor_names(prm::param agent_type, prm::param_accessor param_vals)
	{
		// TODO: return get_state_values(param_vals);
		auto const num_sensors = param_vals["sensor_count"].as< size_t >();
		return generate_state_value_names(num_sensors);
	}

	size_t rd_system::get_agent_num_effectors(prm::param agent_type)
	{
		auto const dt = agent_type.as< DetectorType >();
		switch(dt)
		{
			case DetectorType::SingleObjectDirectionXY:
			// 1: X component of object direction
			// 2: Y component of object direction
			return 2;

			case DetectorType::SingleObjectDirectionBearing:
			// 1: Bearing of object
			return 1;

			case DetectorType::SingleObjectCollisionProbability:
			// 1: Probability
			return 1;

			default:
			throw std::runtime_error{ "Invalid Detector Type" };
		}
	}


	rd_system::rd_system(prm::param_accessor params)
	{
		m_num_sensors = params["sensor_count"].as< size_t >();
		m_detector_type = params["detector_type"][0].as< DetectorType >();

		m_state_value_names = generate_state_value_names(m_num_sensors);
		m_state_value_ids = generate_state_value_idmap(m_num_sensors);

		initialize_state_value_accessors();
	}

	rd_system::~rd_system()
	{

	}

	bool rd_system::is_instantaneous() const
	{
		return true;
	}
		
	i_system::update_info rd_system::get_update_info() const
	{
		update_info uinf;
		uinf.type = update_info::Type::Nonrealtime;
		return uinf;
	}

	void rd_system::initialize()
	{
		m_sensor_readings.clear();
		m_sensor_readings.resize(m_num_sensors, sensor_reading{});

		initialize_objects();

/*		m_sensor_update_time = 0;
		m_decision_update_time = 0;
		m_observer_update_time = 0;
*/	}

	void rd_system::clear_agents()
	{

	}


	i_system::agent_id rd_system::register_agent(agent_specification const& spec)
	{
		return 0;
	}

	void rd_system::register_agent_controller(agent_id agent, std::unique_ptr< i_controller > controller)
	{
		m_controller = std::move(controller);
	}

	i_agent const& rd_system::get_agent(agent_id id) const
	{
		return *this;
	}

	i_controller const& rd_system::get_agent_controller(agent_id id) const
	{
		return *m_controller;
	}

	void rd_system::register_interactive_input(interactive_input const& input)
	{

	}

	void rd_system::initialize_objects()
	{
		auto const distance = 0.75;
		auto const min_radius = 0.3;
		auto const max_radius = 0.3;
		auto const min_speed = 1.0;
		auto const max_speed = 1.0;

		auto dist = std::uniform_real_distribution<> { 0.0, 1.0 };
		auto const bearing = dist(m_rgen) * 2.0 * M_PI;
		m_object.position = dim_traits::bearing_to_vector(bearing) * distance;
		m_object.radius = min_radius + (max_radius - min_radius) * dist(m_rgen);
		m_object.velocity = random_dir_val< dim_traits::velocity_t >(m_rgen);
		m_object.velocity *= min_speed + (max_speed - min_speed) * dist(m_rgen);

		auto const obj_dir = m_object.velocity.normalized();
		auto const dot = m_object.position.dot(obj_dir);
		auto const perp_vector = m_object.position - obj_dir * dot;
		m_collision_prob = (dot < 0.0 && perp_vector.norm() < m_object.radius) ? 1.0 : 0.0;
	}

	std::tuple< bool, double > rd_system::line_circle_intersection(
		dim_traits::position_t const& a,	// Line start point
		dim_traits::direction_t const& d,	// Line direction, must be normalized
		dim_traits::position_t const& p,	// Circle center
		double r							// Circle radius
		)
	{
		auto const a_rel = a - p;
		auto const D = a_rel[0] * d[1] - a_rel[1] * d[0];
		auto const discriminant = r * r - D * D;
		if(discriminant < 0.0)
		{
			return std::tuple< bool, double >{ false, 0.0 };
		}
		else// if(discriminant > 0.0)
		{
			auto const root = std::sqrt(discriminant);

			double t1, t2;
			auto const abs_dy = std::abs(d[1]);
			auto const one_over_root2 = 1.0 / 1.414;
			if(abs_dy > one_over_root2)
			{
				auto const abs_dy_root = abs_dy * root;
				auto const minus_D_dx = -D * d[0];
				auto const y1 = minus_D_dx + abs_dy_root;
				auto const y2 = minus_D_dx - abs_dy_root;

				t1 = (y1 - a_rel[1]) / d[1];
				t2 = (y2 - a_rel[1]) / d[1];
			}
			else
			{
				auto const dx_sign_dy_root = std::copysign(d[0], d[1]) * root;
				auto const D_dy = D * d[1];
				auto const x1 = D_dy + dx_sign_dy_root;
				auto const x2 = D_dy - dx_sign_dy_root;

				t1 = (x1 - a_rel[0]) / d[0];
				t2 = (x2 - a_rel[0]) / d[0];
			}

			// For now, returning only t > 0 intersections 
			auto const tmin = std::min(t1, t2);
			auto const tmax = std::max(t1, t2);
			return tmax > 0.0 ?
				std::tuple< bool, double >{ true, tmin > 0.0 ? tmin : tmax } :
				std::tuple< bool, double >{ false, 0.0 };
		}
	}

	void rd_system::read_sensors()
	{
		auto start_ = std::chrono::high_resolution_clock::now();

		for(size_t i = 0; i < m_num_sensors; ++i)
		{
			dim_traits::position_t const src{ 0.0, 0.0 };
			double const bearing = ((double)i / m_num_sensors) * 2.0 * M_PI;
			dim_traits::direction_t const dir = dim_traits::bearing_to_vector(bearing);

			bool intersect;
			double param;
			std::tie(intersect, param) = line_circle_intersection(
				src,
				dir,
				m_object.position,
				m_object.radius
				);

			if(intersect)
			{
				m_sensor_readings[i].positive = 1.0;
				m_sensor_readings[i].distance = param;
				auto const relative_vel = m_object.velocity - dim_traits::velocity_t{ 0, 0 };	// TODO:
				auto const v_cos_theta = relative_vel.dot(dir);
				m_sensor_readings[i].radial_vel = v_cos_theta;
				// TODO: Check
				m_sensor_readings[i].lateral_vel = dir[0] * relative_vel[1] - dir[1] * relative_vel[0];
			}
			else
			{
				m_sensor_readings[i].positive = 0.0;
				m_sensor_readings[i].distance = 0.0;
				m_sensor_readings[i].radial_vel = 0.0;
				m_sensor_readings[i].lateral_vel = 0.0;
			}
		}

		auto end_ = std::chrono::high_resolution_clock::now();
		m_sensor_update_time += end_ - start_;
	}

	rd_system::decision rd_system::make_decision()
	{
		auto start_ = std::chrono::high_resolution_clock::now();

		decision dec{};

		auto required_inputs = m_controller->get_input_ids();
		auto num_inputs = required_inputs.size();
		i_controller::input_list_t inputs;
		for(auto const& input_id : required_inputs)
		{
			inputs.push_back(get_sensor_value(input_id));
		}

		auto outputs = m_controller->process(inputs);
		if(!outputs.empty())
		{
			switch(m_detector_type)
			{
				case DetectorType::SingleObjectDirectionXY:
				{
					auto const x = outputs[0];
					auto const y = outputs[1];
					dec.angle = dim_traits::vector_to_bearing(dim_traits::direction_t{ x, y });
				}
				break;

				case DetectorType::SingleObjectDirectionBearing:
				{
					dec.angle = outputs[0] * M_PI;
				}
				break;

				case DetectorType::SingleObjectCollisionProbability:
				{
					dec.collision_prob = std::max(0.0, std::min(1.0, outputs[0]));
				}
				break;
			}
		}

		auto end_ = std::chrono::high_resolution_clock::now();
		m_decision_update_time += end_ - start_;

		return dec;
	}

	bool rd_system::update(i_observer* obs)
	{
		read_sensors();
		m_decision = make_decision();

		if(obs)
		{
			auto start_ = std::chrono::high_resolution_clock::now();

			obs->update(this);

			auto end_ = std::chrono::high_resolution_clock::now();
			m_observer_update_time += end_ - start_;
		}

		return false;
	}

	i_observer::observations_t rd_system::record_observations(i_observer* obs) const
	{
		return obs->record_observations(i_observer::trial_data_t{});
	}

	void rd_system::initialize_state_value_accessors()
	{
		m_state_value_accessors.emplace_back([this]{ return dim_traits::vector_to_bearing(m_object.position); });
		m_state_value_accessors.emplace_back([this]{ return m_object.radius; });
		m_state_value_accessors.emplace_back([this]{ return m_collision_prob; });

		m_state_value_accessors.emplace_back([this]{ return m_decision.angle; });
		m_state_value_accessors.emplace_back([this]
		{
			return std::acos(m_object.position.dot(dim_traits::bearing_to_vector(m_decision.angle)) / m_object.position.norm());
		});

		m_state_value_accessors.emplace_back([this]{ return m_decision.collision_prob; });
		m_state_value_accessors.emplace_back([this]{ return std::abs(m_collision_prob - m_decision.collision_prob); });

		for(size_t s = 0; s < m_num_sensors; ++s)
		{
			auto sname = std::string{ "S" } +std::to_string(s + 1);

			m_state_value_accessors.emplace_back([this, s]{ return m_sensor_readings[s].positive; });
			m_state_value_accessors.emplace_back([this, s]{ return m_sensor_readings[s].distance; });
			m_state_value_accessors.emplace_back([this, s]{ return m_sensor_readings[s].radial_vel; });
			m_state_value_accessors.emplace_back([this, s]{ return m_sensor_readings[s].lateral_vel; });
		}
	}

	std::shared_ptr< i_properties const > rd_system::get_state_properties() const
	{
		auto props = std::make_shared< rtp_stored_properties >();

		for(auto const& name : m_state_value_names)
		{
			props->add_property(name);
		}

		return props;
	}

	std::shared_ptr< i_property_values const > rd_system::get_state_property_values() const
	{
		auto vals = std::make_shared< rtp_stored_property_values >();

		for(size_t id = 0; id < m_state_value_names.size(); ++id)
		{
			vals->set_value(m_state_value_names[id], m_state_value_accessors[id]());
		}

		return vals;
	}

	double rd_system::get_state_value(state_value_id id) const
	{
		// TODO:
		throw std::exception("TODO");
	}

	std::unique_ptr< i_system_drawer > rd_system::get_drawer() const
	{
		return std::make_unique< rd_system_drawer >(*this);
	}


	// i_agent interface
	std::string rd_system::get_name() const
	{
		return "Agent";
	}

	agent_sensor_list rd_system::get_mapped_inputs(std::vector< std::string > const& named_inputs) const
	{
		agent_sensor_list result;
		for(auto const& name : named_inputs)
		{
			result.emplace_back(m_state_value_ids.at(name));
		}
		return result;
	}

	double rd_system::get_sensor_value(agent_sensor_id const& sensor) const
	{
		return m_state_value_accessors[sensor]();
	}


	void rd_system::concatenate_performance_data(perf_data_t& pd) const
	{
		pd["Sensors"] += m_sensor_update_time;
		pd["Decision"] += m_decision_update_time;
		pd["Observers"] += m_observer_update_time;
	}

}

