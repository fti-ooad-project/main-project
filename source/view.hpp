#pragma once

#include <cmath>

#include <4u/la/vec.hpp>
#include <4u/la/mat.hpp>

#include <graphics/graphics.h>

#include <engine/spectator.hpp>
#include <engine/divisionspectator.hpp>

class View
{
public:
	float factor = 20.0f;
	vec2 deviation = nullvec2;
	
	Spectator *spec = nullptr;
	DivisionID seldiv = 0;
	
public:
	View(Spectator *s)
	  : spec(s)
	{
		
	}
	
	~View()
	{
		
	}
	
	void drawUnit(const vec2 &upos, const vec2 &udir, double usize)
	{
		float size = sizeWtoS(usize);
		
		fvec2 pos = posWtoS(upos);
		fvec2 dir = udir;
		float cosa = (dir.x() - dir.y())*M_SQRT1_2, sina = (dir.x() + dir.y())*M_SQRT1_2;
		fmat2 ori = fmat2(cosa,sina,-sina,cosa);
		
		gTranslate(pos.data);
		gTransform((size*unifmat2).data);
		gDrawCircle();
		
		gTranslate((pos + 0.5f*dir*size*float(M_SQRT2)).data);
		gTransform(((0.5f*size)*ori).data);
		gDrawQuad();
	}
	
	void drawUnitDst(const vec2 &udst, double usize)
	{
		float size = sizeWtoS(usize);
		
		fvec2 dst = posWtoS(udst);
		
		gTranslate(dst.data);
		gTransform((0.6f*size*unifmat2).data);
		gDrawRing(0.5);
	}
	
	void drawObject(const vec2 &opos, double osize)
	{
		float size = sizeWtoS(osize);
		
		fvec2 pos = posWtoS(opos);
		
		gTranslate(pos.data);
		gTransform((size*unifmat2).data);
		gDrawCircle();
	}
	
	vec2 posStoW(vec2 s) const
	{
		return s/factor + deviation;
	}
	
	vec2 posWtoS(vec2 w) const
	{
		return factor*(w - deviation);
	}
	
	double sizeStoW(double s) const
	{
		return s/factor;
	}
	
	double sizeWtoS(double w) const
	{
		return factor*w;
	}
	
	void draw()
	{
		for(int iplayer = 0; iplayer < spec->getPlayersCount(); ++iplayer)
		{
			PlayerSpectator *player = spec->getPlayerSpectator(iplayer);
			player->forEachDivisionSpectator([this](DivisionSpectator *division)
			{
				if(division->getID() != seldiv)
				{
					gSetColorInt(G_BLUE);
				}
				else
				{
					gSetColorInt(G_GREEN);
				}
				division->forEachUnitSpectator([this](UnitSpectator *unit)
				{
					drawUnit(unit->getPosition(),unit->getDirection(),unit->getSize());
					drawUnitDst(unit->getDestination(),unit->getSize());
				});
			});
		}
		
		gSetColorInt(G_CYAN);
		spec->forEachObjectSpectator([this](ObjectSpectator *object)
		{
			if((object->getObjectType() & 0x0100) == 0)
			{
				drawObject(object->getPosition(),object->getSize());
			}
		});
	}
	
	void setSelection(DivisionID id)
	{
		seldiv = id;
	}
};
