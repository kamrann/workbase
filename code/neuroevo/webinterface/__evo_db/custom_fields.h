// custom_fields.h

#ifndef __GADB_CUSTOM_FIELDS_H
#define __GADB_CUSTOM_FIELDS_H

#include <Wt/Dbo/SqlTraits>
#include <Wt/Dbo/SqlConnection>
#include <Wt/Dbo/SqlStatement>

#include <string>
#include <chrono>


namespace Wt {
	namespace Dbo {

		// TODO: Support other durations?
		template <>
		struct sql_value_traits< std::chrono::time_point< std::chrono::system_clock >, void >
		{
			typedef std::chrono::system_clock clock_t;
			typedef clock_t::duration duration_t;
			typedef std::chrono::time_point< clock_t, duration_t > time_point_t;

			static const bool specialized = true;

			static const char* type(SqlConnection *conn, int size)
			{
				return "BIGINT NOT NULL";
			}

			static void bind(const time_point_t& v, SqlStatement *statement, int column, int size)
			{
				auto as_integer = v.time_since_epoch().count();
				statement->bind(column, static_cast< long long >(as_integer));
			}

			static bool read(time_point_t& v, SqlStatement *statement, int column, int size)
			{
				long long as_integer = 0;
				bool result = statement->getResult(column, &as_integer);
				if(result)
				{
					v = time_point_t(clock_t::duration(static_cast< duration_t::rep >(as_integer)));
				}
				return result;
			}
		};

	}
}


#endif

