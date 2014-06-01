// yaml_test.h

#ifndef __YAML_TEST_H
#define __YAML_TEST_H

namespace YAML {
	class Node;
}

class yaml_spec_A
{
public:
	YAML::Node get_param_schema();
};

class yaml_spec_B
{
public:
	YAML::Node get_param_schema();
};


#endif


