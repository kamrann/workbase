// genome.h

#ifndef __WB_GA_GENOME_H
#define __WB_GA_GENOME_H

#include <vector>
#include <memory>


namespace ga {

	// Genome base class
	class i_genome
	{
	public:
		virtual std::unique_ptr< i_genome > clone() const = 0;
		virtual std::vector< unsigned char > to_binary() const = 0;

	public:
		virtual ~i_genome() {}
	};

	// Value-semantic wrapper for arbitrary genome implementation
	class genome
	{
	public:
		genome()
		{}
		genome(std::unique_ptr< i_genome > gn):
			m_gn(std::move(gn))
		{}
		genome(genome const& rhs):
			m_gn(rhs.m_gn->clone())
		{}
		genome(genome&&) = default;

	public:
		genome& operator= (genome const& rhs)
		{
			m_gn = rhs.m_gn->clone();
			return *this;
		}

		genome& operator= (genome&& rhs)
		{
			m_gn = std::move(rhs.m_gn);
			return *this;
		}

		template < typename GenomeType >
		inline GenomeType& as() const
		{
			return *static_cast<GenomeType*>(m_gn.get());
		}

		inline std::vector< unsigned char > to_binary() const
		{
			return m_gn->to_binary();
		}

	private:
		std::unique_ptr< i_genome > m_gn;
	};

}


#endif


