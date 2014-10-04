// message.h

#ifndef __WB_PWORK_MESSAGE_H
#define __WB_PWORK_MESSAGE_H

#include "message_types.h"
#include "message_detail.h"

#include <string>
#include <vector>


namespace pwork {

	// Class to encapsulate a generic message, used for both peer <-> peer and peer <-> client
	class message
	{
	public:
		message();
		message(MessageClass cls, msg_type type);

		/*
		template < typename... TagTypes >
		message::message(MessageClass cls, msg_type type, TagTypes... tags):
		m_cls(cls),
		m_type(type),
		m_content(std::make_shared< msg_content >(YAML::Node{ content }))
		{}
		*/

	private:
		message(MessageClass cls, msg_type type, detail::msg_content content);

		//		template < typename... TagTypes >
		//		message(MessageClass cls, msg_type type, TagTypes... tags);

	public:
		inline operator bool() const
		{
			return m_cls != MessageClass::Undefined;
		}

		inline bool operator! () const
		{
			return !operator bool();
		}

		inline MessageClass get_class() const
		{
			return m_cls;
		}

		template < MessageClass cls, typename ReturnType >
		inline ReturnType get_type() const;

		template <>
		inline Peer2PeerMsg get_type< MessageClass::Peer2Peer, Peer2PeerMsg >() const
		{
			return boost::get< Peer2PeerMsg >(m_type);
		}

		template <>
		inline Peer2ClientMsg get_type< MessageClass::Peer2Client, Peer2ClientMsg >() const
		{
			return boost::get< Peer2ClientMsg >(m_type);
		}

		template <>
		inline Client2PeerMsg get_type< MessageClass::Client2Peer, Client2PeerMsg >() const
		{
			return boost::get< Client2PeerMsg >(m_type);
		}

		detail::msg_content const& get_content() const
		{
			return m_content;
		}

		template < MsgTag tag >
		void add_tag(typename tag_type_traits< tag >::type tag_data)
		{
			m_content.add< tag >(tag_data);
		}

		template < MsgTag tag >
		bool has() const
		{
			return m_content.has(tag);
		}

		template < MsgTag tag >
		auto get() const -> typename tag_type_traits< tag >::type
		{
			return m_content.get(tag)[0].as< typename tag_type_traits< tag >::type >();
		}

		template < MsgTag tag >
		auto get_list() const -> std::vector < typename tag_type_traits< tag >::type >
		{
			return m_content.get(tag).as< std::vector< typename tag_type_traits< tag >::type > >();
		}
		
	private:
		MessageClass m_cls;
		msg_type m_type;
		detail::msg_content m_content;

		friend std::vector< char > encode_message(message const& msg);
		friend message extract_message(std::string str);
	};

	size_t const MessageSizeMarkerBytes = 4;

	std::vector< char > encode_message(message const& msg);
	message extract_message(std::string str);

}



#endif


