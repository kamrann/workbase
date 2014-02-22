// nac_widget.h

#ifndef __NOUGHTS_AND_CROSSES_WIDGET_H
#define __NOUGHTS_AND_CROSSES_WIDGET_H

#include "systems/noughts_and_crosses/noughts_and_crosses_system.h"

#include <Wt/WPaintedWidget>
#include <Wt/WPaintDevice>
#include <Wt/WPainter>
#include <Wt/WPen>
#include <Wt/WRectArea>


class nac_widget_base: public Wt::WPaintedWidget
{
public:
	// TODO: Move some stuff to a system generic widget base
	enum {
		Margin = 10,
	};
};


template <
	size_t Dimensions_,
	size_t Size_
>
class noughts_and_crosses_widget;

// 2D Widget specialization
template <
	size_t Size_
>
class noughts_and_crosses_widget< 2, Size_ >: public nac_widget_base
{
public:
	enum {
		Dimensions = 2,
		Size = Size_,

		LineWidth = 1,
	};

	typedef noughts_and_crosses_system< Dimensions, Size > system_t;
	typedef typename system_t::board_state state_t;

public:
	noughts_and_crosses_widget()
	{
		m_invalid_move = false;
		m_interaction_enabled = false;
		setLayoutSizeAware(true);
	}

public:
	void set_invalid_move(bool invalid)
	{
		m_invalid_move = invalid;
	}

	void enable_interaction(bool enable)
	{
		if(m_interaction_enabled != enable)
		{
			m_interaction_enabled = enable;
		}
	}

	void update_from_system_state(state_t const& st)
	{
		m_st = st;
		nac_widget_base::update();
	}

	Wt::Signal< coord< Dimensions > >& move_made()
	{
		return m_move_made_signal;
	}

protected:
	void layoutSizeChanged(int width, int height)
	{
		const std::vector< WAbstractArea* > int_areas = areas();
		for(WAbstractArea* a : int_areas)
		{
			removeArea(a);
		}

		size_t avail_board_size = std::min(width - 2 * Margin, height - 2 * Margin);
		size_t avail_squares_size = avail_board_size - (Size - 1) * LineWidth;
		size_t square_size = avail_squares_size / Size;
		size_t remainder = avail_squares_size % Size;

		size_t x_base = Margin + (width > height ?
			((width - height + remainder) / 2) :
			remainder / 2
			);
		size_t y_base = Margin + (width < height ?
			((height - width + remainder) / 2) :
			remainder / 2
			);

		for(size_t x = 0; x < Size; ++x)
		{
			size_t x_pix = x_base + (square_size + LineWidth) * x;
			
			for(size_t y = 0; y < Size; ++y)
			{
				size_t y_pix = y_base + (square_size + LineWidth) * y;

				WRectArea* area = new WRectArea((int)x_pix, (int)y_pix, (int)square_size, (int)square_size);
				addArea(area);
				area->clicked().connect(boost::bind(&noughts_and_crosses_widget::on_area_clicked,
					this, x, y));
			}
		}

		WPaintedWidget::layoutSizeChanged(width, height);
	}

	void paintEvent(Wt::WPaintDevice* device)
	{
		Wt::WLength dev_width = device->width();
		Wt::WLength dev_height = device->height();
		size_t avail_board_size = (size_t)std::min(dev_width.value() - 2 * Margin, dev_height.value() - 2 * Margin);
		size_t avail_squares_size = avail_board_size - (Size - 1) * LineWidth;
		size_t square_size = avail_squares_size / Size;
		size_t remainder = avail_squares_size % Size;

		Wt::WPainter painter(device);
		Wt::WPen pen(Wt::GlobalColor::black);
		painter.setPen(pen);

		size_t x_base = Margin + (dev_width.value() > dev_height.value() ?
			(size_t)((dev_width.value() - dev_height.value() + remainder) / 2) :
			remainder / 2
			);
		size_t y_base = Margin + (dev_width.value() < dev_height.value() ?
			(size_t)((dev_height.value() - dev_width.value() + remainder) / 2) :
			remainder / 2
			);

		// Draw vertical lines
		for(size_t x = 0; x < Size - 1; ++x)
		{
			size_t x_pix = x_base + square_size + (square_size + LineWidth) * x;
			painter.drawLine(x_pix, y_base, x_pix, y_base + (Size - 1) * LineWidth + Size * square_size);
		}

		// Draw horizontal lines
		for(size_t y = 0; y < Size - 1; ++y)
		{
			size_t y_pix = y_base + square_size + (square_size + LineWidth) * y;
			painter.drawLine(x_base, y_pix, x_base + (Size - 1) * LineWidth + Size * square_size, y_pix);
		}

		// Draw square contents
		pen.setWidth(3);
		painter.setPen(pen);
		for(size_t x = 0; x < Size; ++x)
		{
			size_t x_pix = x_base + (square_size + LineWidth) * x;
			for(size_t y = 0; y < Size; ++y)
			{
				size_t y_pix = y_base + (square_size + LineWidth) * y;
				size_t const InnerMargin = 10;
				Wt::WRectF rc(
					x_pix + InnerMargin,
					y_pix + InnerMargin,
					square_size - 2 * InnerMargin,
					square_size - 2 * InnerMargin
					);

				typename state_t::coord_t board_coord;
				board_coord[0] = x;
				board_coord[1] = y;
				switch(m_st.by_coord(board_coord))
				{
				case typename system_t::SquareState::Nought:
					{
						painter.drawEllipse(rc);
					}
					break;
				case typename system_t::SquareState::Cross:
					{
						painter.drawLine(rc.topLeft(), rc.bottomRight());
						painter.drawLine(rc.topRight(), rc.bottomLeft());
					}
					break;
				}
			}
		}

		Wt::WFont font = painter.font();
		//font.setFamily(Wt::WFont::Default);
		font.setSize(50);
		painter.setFont(font);

		boost::optional< typename system_t::Player > winner = m_st.is_winner();
		if(winner)
		{
			// Draw winning line
			typename state_t::line winning_line = *m_st.get_winning_line();
			pen.setColor(Wt::GlobalColor::red);
			painter.setPen(pen);
			Wt::WPointF p1(
				x_base + (square_size + LineWidth) * winning_line[0][0] + square_size * 0.5f,
				y_base + (square_size + LineWidth) * winning_line[0][1] + square_size * 0.5f
				);
			Wt::WPointF p2(
				x_base + (square_size + LineWidth) * winning_line[Size - 1][0] + square_size * 0.5f,
				y_base + (square_size + LineWidth) * winning_line[Size - 1][1] + square_size * 0.5f
				);
			painter.drawLine(p1, p2);

			// Output winner text
			pen.setColor(Wt::GlobalColor::blue);
			painter.setPen(pen);
			Wt::WRectF rc(
				0,
				0,
				dev_width.toPixels(),
				dev_height.toPixels()
				);
			std::string text;
			text += *winner == typename system_t::Player::Crosses ? "Crosses" : "Noughts";
			text += " Win";
			painter.drawText(rc, Wt::AlignCenter | Wt::AlignMiddle, text);
		}
		else if(m_st.game_over())
		{
			// Tie text
			pen.setColor(Wt::GlobalColor::blue);
			painter.setPen(pen);
			Wt::WRectF rc(
				0,
				0,
				dev_width.toPixels(),
				dev_height.toPixels()
				);
			std::string text = "Game Tied";
			painter.drawText(rc, Wt::AlignCenter | Wt::AlignMiddle, text);
		}
		else if(m_invalid_move)
		{
			pen.setColor(Wt::GlobalColor::blue);
			painter.setPen(pen);
			Wt::WRectF rc(
				0,
				0,
				dev_width.toPixels(),
				dev_height.toPixels()
				);
			std::string text = "Invalid Move Made";
			painter.drawText(rc, Wt::AlignCenter | Wt::AlignMiddle, text);
		}
	}

	void on_area_clicked(int x, int y)
	{
		if(m_interaction_enabled)
		{
			typename typename system_t::board_state::coord_t coord;
			coord[0] = x;
			coord[1] = y;
			m_move_made_signal.emit(coord);
		}
	}

private:
	state_t m_st;	// copy
	bool m_invalid_move;
	bool m_interaction_enabled;
	Wt::Signal< coord< Dimensions > > m_move_made_signal;
};


#endif


