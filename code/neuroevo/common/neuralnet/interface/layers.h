// layers.h

#ifndef __WB_NN_LAYERS_H
#define __WB_NN_LAYERS_H

#include <boost/optional.hpp>

#include <set>


namespace nnet {

	enum class LayerType {
		Input,
		Hidden,
		Output,
	};

	struct layer_data
	{
		LayerType type;

		// Valid only when type = Hidden.
		// boost::none = any hidden layer, 0 = first hidden layer, 1 = second hidden layer, ...
		boost::optional< size_t > subindex;

		layer_data(LayerType _type, boost::optional< size_t > _subindex = boost::none):
			type(_type),
			subindex(_subindex)
		{}

		inline bool match(layer_data const& rhs) const
		{
			return type == rhs.type && (
				!subindex || !rhs.subindex || *subindex == *rhs.subindex
				);
		}

		inline bool operator< (layer_data const& rhs) const
		{
			return type < rhs.type || type == rhs.type && (
				subindex < rhs.subindex
				);
		}
	};

	typedef std::set< layer_data > layer_set;

}


#endif


