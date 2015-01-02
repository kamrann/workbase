// load_script.h

#ifndef __WB_B2D_LOAD_SCRIPT_H
#define __WB_B2D_LOAD_SCRIPT_H

#include <box2d/box2d.h>

#include <boost/optional.hpp>

#include <string>
#include <map>


namespace b2dc {
	class rotation_joint;
	class sucker_cmp;
}

namespace b2ds {

	typedef std::string script;

	boost::optional< script > load_script_from_file(std::string const& script_filepath);

	struct instantiation_data
	{
		std::map< std::string, b2Body* > bodies;
		std::map< std::string, std::shared_ptr< b2dc::rotation_joint > > hinges;
		std::map< std::string, std::shared_ptr< b2dc::sucker_cmp > > suckers;
	};

	bool instantiate_script(script const& s, b2World* world, instantiation_data* id = nullptr);

}


#endif


