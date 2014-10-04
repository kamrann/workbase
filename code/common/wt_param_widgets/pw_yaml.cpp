// pw_yaml.cpp

#include "pw_yaml.h"


namespace YAML {

	std::string const convert< prm::container_par_wgt::Layout >::names_[prm::container_par_wgt::Layout::Count] = {
		"vertical",
		"horizontal",
	};

	std::map< std::string, prm::container_par_wgt::Layout > const convert< prm::container_par_wgt::Layout >::mapping_ = {
			{ "vertical", prm::container_par_wgt::Layout::Vertical },
			{ "horizontal", prm::container_par_wgt::Layout::Horizontal },
			{ "vert", prm::container_par_wgt::Layout::Vertical },
			{ "horiz", prm::container_par_wgt::Layout::Horizontal },
			{ "v", prm::container_par_wgt::Layout::Vertical },
			{ "h", prm::container_par_wgt::Layout::Horizontal },
	};

}


