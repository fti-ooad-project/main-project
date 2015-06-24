#pragma once

#include <map>

#include <engine/spectator.hpp>
#include <engine/divisionspectator.hpp>

#include "view_engine.hpp"

class View
{
public:
	enum State
	{
		CLOSED = 0,
		OPENED
	};

private:
	Spectator *spec = nullptr;
	DivisionID seldiv = 0;
	
	std::map<ObjectID,ViewObjectID> objs;
	double scale = 4.0;
	
public:
	View(Spectator *s)
	  : spec(s)
	{
		viewInit();
	}
	
	~View()
	{
		
	}
	
	State getState() const
	{
		if(viewGetState())
		{
			return State::OPENED;
		}
		return State::CLOSED;
	}
	
	void draw()
	{
		for(int iplayer = 0; iplayer < spec->getPlayersCount(); ++iplayer)
		{
			PlayerSpectator *player = spec->getPlayerSpectator(iplayer);
			player->forEachDivisionSpectator([this](DivisionSpectator *division)
			{
				int div = division->getID();
				division->forEachUnitSpectator([this,div](UnitSpectator *unit)
				{
					ViewObjectID view_obj_id = findOrInsert(unit->getID(),0);
					ViewObjectState state;
					state.px = unit->getPosition().x()*scale;
					state.py = unit->getPosition().y()*scale;
					state.dx = unit->getDirection().x();
					state.dy = unit->getDirection().y();
					viewSetObjectState(view_obj_id,&state);
					
					float spd = length(unit->getVelocity());
					const float wspd = 0.32, rspd = 0.81;
					if(spd < 0.01)
						viewSetObjectAnim(view_obj_id,2,0.2f); // idle
					else if(spd < 1.6)
						viewSetObjectAnim(view_obj_id,0,spd/wspd/scale); // walk
					else
						viewSetObjectAnim(view_obj_id,1,spd/rspd/scale); // run
					
					if(div != seldiv)
						viewSelectObject(view_obj_id,0);
					else
						viewSelectObject(view_obj_id,2);
					
				});
			});
		}
		
		spec->forEachObjectSpectator([this](ObjectSpectator *object)
		{
			if((object->getObjectType() & 0x0100) == 0)
			{
				ViewObjectID view_obj_id = findOrInsert(object->getID(),2);
				ViewObjectState state;
				state.px = object->getPosition().x()*scale;
				state.py = object->getPosition().y()*scale;
				state.dx = 1.0f;
				state.dy = 0.0f;
				viewSetObjectState(view_obj_id,&state);
			}
		});
	}
	
	ViewObjectID findOrInsert(ObjectID id, int type)
	{
		auto iter = objs.find(id);
		int view_obj_id = 0;
		if(iter == objs.end())
		{
			view_obj_id = viewAddObject(type);
			objs.insert(std::pair<ObjectID,ViewObjectID>(id,view_obj_id));
		}
		else
		{
			view_obj_id = iter->second;
		}
		return view_obj_id;
	}
	
	void setSelection(DivisionID id)
	{
		seldiv = id;
	}
	
	vec3 StoW(vec2 p) const
	{
		fvec3 r;
		viewGetMousePos(p.x(),p.y(),&r.x(),&r.y(),&r.z());
		return r/scale;
	}
};
