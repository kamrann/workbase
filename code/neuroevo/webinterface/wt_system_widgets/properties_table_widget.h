// properties_table_widget.h

#ifndef __WB_PROPERTIES_TABLE_WIDGET_H
#define __WB_PROPERTIES_TABLE_WIDGET_H

#include "properties_widget.h"

#include <Wt/WTableView>
#include <Wt/WStandardItemModel>
#include <Wt/WStandardItem>
#include <Wt/WItemDelegate>
#include <Wt/WApplication>

#include <boost/math/constants/constants.hpp>


class properties_table_widget:
	public i_properties_widget,
	public Wt::WTableView
{
public:
	properties_table_widget(): model(nullptr)
	{
		model = new Wt::WStandardItemModel();
		setModel(model);
		//setRowHeaderCount(1);
		setHeaderHeight(0);	// TODO: Is this the only way to hide headers?
		auto del = new Wt::WItemDelegate{};
		del->setTextFormat("%.3f");
		setItemDelegate(del);
		reset();
	}

public:
	virtual void clear_content() override
	{
//		model->removeRows(0, model->rowCount());

		setRowHeaderCount(0);
		model->removeColumns(1, model->columnCount() - 1);
	}

	virtual void reset(std::shared_ptr< rtp::i_properties const > props = nullptr) override
	{
		model->clear();

		if(props)
		{
			size_t count = props->num_properties();
//			model->insertColumns(0, 1);
//			model->insertRows(0, count);
			std::vector< Wt::WStandardItem* > headers;
			for(size_t i = 0; i < count; ++i)
			{
//				model->setHeaderData(i, Wt::Orientation::Vertical, props->get_property_name(i));
				headers.push_back(new Wt::WStandardItem{ props->get_property_name(i) });
			}
			model->appendColumn(headers);
		}
		
		m_props = props;
	}
	
	virtual void register_data(std::shared_ptr< rtp::i_property_values const > vals) override
	{
		if(vals)
		{
			model->appendColumn(create_data_row(m_props, vals));
			setColumnAlignment(model->columnCount() - 1, Wt::AlignRight);
		}
		//setRowHeaderCount(1);	TODO: This doesnt seem to be working, probably Wt bug
	}

protected:
	std::vector< Wt::WStandardItem* > create_data_row(std::shared_ptr< rtp::i_properties const >& props, std::shared_ptr< rtp::i_property_values const > vals)
	{
		std::vector< Wt::WStandardItem* > row;
		Wt::WStandardItem* item;

		size_t num_props = props->num_properties();
		for(size_t i = 0; i < num_props; ++i)
		{
			std::string name = props->get_property_name(i);
			boost::any val = vals->get_property(name);
			
			item = new Wt::WStandardItem;
			item->setData(val, Wt::DisplayRole);
			row.push_back(item);
		}

		return row;
	}

private:
	Wt::WStandardItemModel* model;
	std::shared_ptr< rtp::i_properties const > m_props;
};


#endif


