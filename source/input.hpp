#pragma once

#include <4u/la/vec.hpp>
#include <4u/la/mat.hpp>

#include <media/media.h>
#include <graphics/graphics.h>
#include <gui/factory.hpp>
#include <gui/handler.hpp>

#include "view.hpp"
#include <engine/playerhandle.hpp>
#include <engine/divisionhandle.hpp>

class Input
{
private:
	
	int done = 0;
	
	View *view;
	PlayerHandle *player;
	DivisionID division = 0;
	
public:
	Input(View *v, PlayerHandle *s)
	  : view(v), player(s)
	{
		viewSetMouseFunc([this](int index, float px, float py)
		{
			if(player != nullptr)
			{
				fvec3 p3 = view->StoW(vec2(px,py));
				fvec2 pos = vec2(p3.x(),p3.y());
				
				if(index == 2)
				{
					player->forDivisionHandleID(division,[this,pos](DivisionHandle *division)
					{
						division->setDestination(pos);
						division->setDirection(norm(division->getDestination() - division->getPosition()));
					});
				}
				else
				if(index == 0)
				{
					player->forEachDivisionHandle([this,pos](DivisionHandle *division)
					{
						vec2 dir = division->getDirection();
						mat2 rot = {dir.y(),-dir.x(),dir.x(),dir.y()};
						vec2 drp = rot*(pos - division->getPosition())/division->getDistance();
						drp.x() /= division->getWidth();
						drp.y() /= (division->getUnitsCount()/division->getWidth() + 1);
						if(fabs(drp.x()) < 0.5 && fabs(drp.y()) < 0.5)
						{
							setDivisionID(division->getID());
						}
					});
				}
			}
		});
	}

	~Input()
	{
		
	}
	
	// player->purchaseDivision(UNIT_SWORDSMAN,0x40);
	
	void setDivisionID(DivisionID id)
	{
		division = id;
		if(view != nullptr)
		{
			view->setSelection(id);
		}
	}
};
