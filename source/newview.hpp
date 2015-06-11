#pragma once

#include "view.hpp"
#include <engine/spectator.hpp>

class NewView
{
private:
	Spectator *spec = nullptr;
	View *view = nullptr;
	
public:
	NewView(Spectator *s, View *v)
	  : spec(s), view(v)
	{
		
	}
	
	~NewView()
	{
		
	}
	
	void draw()
	{
		gSetColorInt(G_BLUE);
		for(int iplayer = 0; iplayer < spec->getPlayersCount(); ++iplayer)
		{
			PlayerSpectator *player = spec->getPlayerSpectator(iplayer);
			player->forEachDivisionSpectator([this](DivisionSpectator *division)
			{
				division->forEachUnitSpectator([this](UnitSpectator *unit)
				{
					view->drawUnitP(unit->getPosition(),unit->getDirection(),unit->getSize());
					view->drawUnitDstP(unit->getDestination(),unit->getSize());
				});
			});
		}
		
		gSetColorInt(G_CYAN);
		spec->forEachObjectSpectator([this](ObjectSpectator *object)
		{
			if((object->getObjectType() & 0x0100) == 0)
			{
				view->drawObjectP(object->getPosition(),object->getSize());
			}
		});
	}
};
