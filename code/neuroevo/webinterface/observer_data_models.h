// observer_data_models.h

#ifndef __OBSERVER_DATA_MODELS_H
#define __OBSERVER_DATA_MODELS_H

#include "systems/noughts_and_crosses/observers/basic_observer.h"

#include <Wt/WStandardItemModel>
#include <Wt/WStandardItem>


template < typename T >
struct observer_data_model;


template <>
struct observer_data_model< basic_observer >
{
	static Wt::WStandardItemModel* generate(std::vector< basic_observer::per_trial_data > const& data)
	{
		Wt::WStandardItemModel* model = new Wt::WStandardItemModel(data.size(), 2);
		model->setHeaderData(0, std::string("# moves"));
		model->setHeaderData(1, std::string("result"));

		size_t row = 0;
		for(basic_observer::per_trial_data const& d : data)
		{
			Wt::WStandardItem* item = new Wt::WStandardItem;
			item->setData(boost::any(d.total_moves_made), Wt::DisplayRole);
			model->setItem(row, 0, item);

			item = new Wt::WStandardItem;
			std::string result;
			if(d.game_completed)
			{
				if(d.winner)
				{
					result = (*d.winner == noughts_and_crosses_base::Player::Crosses) ? "crosses" : "noughts";
				}
				else
				{
					result = "tied";
				}
			}
			else
			{
				result = "invalid";
			}
			item->setData(boost::any(result), Wt::DisplayRole);
			model->setItem(row, 1, item);

			++row;
		}
		return model;
	}
};


#endif


