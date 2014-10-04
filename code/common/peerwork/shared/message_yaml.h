// message_yaml.h

#ifndef __WB_PWORK_MESSAGE_YAML_H
#define __WB_PWORK_MESSAGE_YAML_H

#include "message_types.h"
#include "message_tags.h"
#include "job_description.h"
#include "../peerwork/peer_id.h"

#include <boost/asio.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <yaml-cpp/yaml.h>


namespace pwork {

	template < typename T >
	struct yaml_bimap_converter
	{
		typedef typename T::type type;

		static YAML::Node encode(type const& rhs)
		{
			return YAML::Node(T::mp.at(rhs));
		}

		static bool decode(YAML::Node const& node, type& rhs)
		{
			if(!node.IsScalar())
			{
				return false;
			}
			
			auto it = T::mp.right.find(node.Scalar());
			if(it == T::mp.right.end())
			{
				return false;
			}

			rhs = it->second;
			return true;
		}
	};

}

namespace YAML {

	template <>
	struct convert < pwork::MessageClass >: public pwork::yaml_bimap_converter< pwork::msg_cls_traits >
	{};

	template <>
	struct convert < pwork::Peer2PeerMsg >: public pwork::yaml_bimap_converter< pwork::p2p_msg_traits >
	{};

	template <>
	struct convert < pwork::Peer2ClientMsg >: public pwork::yaml_bimap_converter< pwork::p2c_msg_traits >
	{};

	template <>
	struct convert < pwork::Client2PeerMsg >: public pwork::yaml_bimap_converter< pwork::c2p_msg_traits >
	{};

	template <>
	struct convert < pwork::MsgTag >: public pwork::yaml_bimap_converter< pwork::tag_traits >
	{};


	template <>
	struct convert < pwork::job_description >
	{
		static Node encode(pwork::job_description const& rhs)
		{
			Node n;
			n.push_back(rhs.type);
			n.push_back(rhs.specification);
			return n;
		}

		static bool decode(Node const& node, pwork::job_description& rhs)
		{
			if(
				!node.IsSequence() ||
				node.size() != 2 ||
				!node[0].IsScalar() ||
				!node[1].IsScalar()
				)
			{
				return false;
			}

			rhs.type = node[0].as< std::string > ();
			rhs.specification = node[1].as< std::string >();
			return true;
		}
	};

	template <>
	struct convert < boost::uuids::uuid >
	{
		static Node encode(boost::uuids::uuid const& rhs)
		{
			std::stringstream ss;
			ss << rhs;
			return Node{ ss.str() };
		}

		static bool decode(Node const& node, boost::uuids::uuid& rhs)
		{
			if(!node.IsScalar())
			{
				return false;
			}

			std::istringstream ss(node.Scalar());
			return bool(ss >> rhs);
		}
	};

	template <>
	struct convert < pwork::peer_id_and_details >
	{
		static Node encode(pwork::peer_id_and_details const& rhs)
		{
			Node n;
			n.push_back(rhs.id);
			n.push_back(rhs.name);
			n.push_back(rhs.endpt.address().to_string());
			n.push_back(rhs.endpt.port());
			return n;
		}

		static bool decode(Node const& node, pwork::peer_id_and_details& rhs)
		{
			if(
				!node.IsSequence() ||
				node.size() != 4 ||
				!node[0].IsScalar() ||
				!node[1].IsScalar() ||
				!node[2].IsScalar() ||
				!node[3].IsScalar()
				)
			{
				return false;
			}

			rhs.id = node[0].as< pwork::peer_id >();
			rhs.name = node[1].as< std::string >();
			auto addr = boost::asio::ip::address::from_string(node[2].as< std::string >());
			auto port = node[3].as< unsigned short >();
			rhs.endpt = tcp::endpoint{ addr, port };
			return true;
		}
	};

}


#endif


