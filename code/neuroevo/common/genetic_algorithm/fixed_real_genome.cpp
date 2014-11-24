// fixed_real_genome.cpp

#include "fixed_real_genome.h"


namespace ga {

	std::unique_ptr< i_genome > fixed_real_genome::clone() const
	{
		return std::make_unique< fixed_real_genome >(*this);
	}

	std::vector< unsigned char > fixed_real_genome::to_binary() const
	{
		// TODO: Perhaps better to write a to_binary(gene_t) function, and implement this method
		// in fixed_array_genome instead of in all derived classes
		std::vector< unsigned char > bytes(size() * BytesPerValue);
		unsigned char* ptr = &bytes[0];
		for(auto const& val : *this)
		{
			std::memcpy(ptr, &val, BytesPerValue);
			ptr += BytesPerValue;
		}
		return bytes;
	}

	fixed_real_genome fixed_real_genome::from_binary(std::vector< unsigned char > const& bytes)
	{
		if(bytes.size() % BytesPerValue != 0)
		{
			throw std::runtime_error("invalid binary data size");
		}

		auto const num_genes = bytes.size() / BytesPerValue;
		fixed_real_genome gn;
		gn.resize(num_genes);
		unsigned char const* ptr = &bytes[0];
		for(auto& val : gn)
		{
			std::memcpy(&val, ptr, BytesPerValue);
			ptr += BytesPerValue;
		}

		return gn;
	}

}



