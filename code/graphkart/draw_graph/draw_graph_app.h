
#include "graph_defs/graph_defn.h"
#include "graphgen/gen_spatial.h"

#include "util/camera.h"

#include "gles_angle/esUtil.h"
#include "gles_angle/shader_program.h"
#include "gles_angle/mesh.h"

#include <Eigen/Dense>

#include <boost/random/mersenne_twister.hpp>
#include <boost/shared_ptr.hpp>

#include <map>


class draw_graph_app
{
public:
	draw_graph_app();
    ~draw_graph_app();

	typedef std::map<
		gk::g_node,
		Eigen::Vector2d,
		std::less< gk::g_node >,
		Eigen::aligned_allocator< std::pair< const gk::g_node, Eigen::Vector2d > >
	> node_location_map_t;

//	void boost_drawing(drawing_map_t& d);
//	void levels_drawing(drawing_map_t& d);

//	drawing_map_t generate_graph_topology_first();
	node_location_map_t generate_graph_spatial_first();

	//void handleEvent(poly::Event *e);
	//void UpdateDisplayText();
	bool setup_shaders();
	bool initialise(ESContext* context);

	void process_keyboard_input(unsigned char c, int x, int y);

	void increment_graph();

	void restart_player();

	void update(ESContext* context, float dtime);

	void draw_nodes();
	void draw(ESContext* context);

public:
	enum {
		SCREEN_WIDTH = 1024,
		SCREEN_HEIGHT = 768,

//		MARGIN = 100,
//		GRAPH_PLOT_WIDTH = SCREEN_WIDTH - 2 * MARGIN,
//		GRAPH_PLOT_HEIGHT = SCREEN_HEIGHT - 2 * MARGIN,
	};

private:
	boost::random::mt19937 rgen;
	gk::graph g;
	gk::ggen::iterative_gen_data g_gen_data;

	node_location_map_t node_locations;
	bool graph_connected;

	struct player_data
	{
		bool active;
		gk::g_node u, v;
		float pos;
		float rot_angle;
		float rot_speed;
		float energy_level;
		float inner_rot_angle;

		player_data(): active(false), pos(0.0f), rot_angle(0.0f), rot_speed(0.0f), energy_level(0.0f), inner_rot_angle(0.0f)
		{}
	};

	player_data player;

	shader_prog tex_shader, vcolor_shader;

	GLuint tex_obj_id;

	camera cam;
	bool use_texture_shader;

	mesh< float > node_mesh;
	mesh< float > connection_mesh;
	mesh< float > player_inner_mesh;
	mesh< float > player_torus_mesh;
	mesh< float > player_marker_mesh;

public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};
