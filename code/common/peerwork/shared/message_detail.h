// message_detail.h

#ifndef __WB_PWORK_MESSAGE_DETAIL_H
#define __WB_PWORK_MESSAGE_DETAIL_H

#include "message_tags.h"
#include "message_yaml.h"

#include <yaml-cpp/yaml.h>


namespace pwork {
	namespace detail {

		class msg_content
		{
		public:
			msg_content();
			msg_content(std::string str);
			msg_content(YAML::Node nd);

		public:
			bool has(MsgTag tag) const;
			YAML::Node get() const;
			YAML::Node get(MsgTag tag) const;

		public:
			template < MsgTag tag >
			void add(typename tag_type_traits< tag >::type tag_data)
			{
				m_yaml[tag_traits::mp.at(tag)].push_back(YAML::Node{ tag_data });
			}

		private:
			YAML::Node m_yaml;
		};

		inline msg_content::msg_content():
			m_yaml{}
		{

		}

		inline msg_content::msg_content(std::string str):
			m_yaml{ YAML::Load(str) }
		{

		}

		inline msg_content::msg_content(YAML::Node nd):
			m_yaml{ nd }
		{

		}

		inline bool msg_content::has(MsgTag tag) const
		{
			return m_yaml[tag_traits::mp.at(tag)].IsDefined();
		}

		inline YAML::Node msg_content::get() const
		{
			return m_yaml;
		}

		inline YAML::Node msg_content::get(MsgTag tag) const
		{
			return m_yaml[tag_traits::mp.at(tag)];
		}

	}
}


#endif

