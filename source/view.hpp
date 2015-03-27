#pragma once

#include <cmath>

#include <4u/la/vec.hpp>
#include <4u/la/mat.hpp>

#include <graphics/graphics.h>

#include <engine-source/unit.hpp>


class View
{
public:
	float factor = 20.0f;
	vec2 deviation = nullvec2;
	
	void drawUnit(const Unit *u)
	{
		float size = sizeWtoS(u->getSize());
		
		fvec2 pos = posWtoS(u->getPos());
		fvec2 dir = u->getDir();
		float cosa = (dir.x() - dir.y())*M_SQRT1_2, sina = (dir.x() + dir.y())*M_SQRT1_2;
		fmat2 ori = fmat2(cosa,sina,-sina,cosa);
		
		gTranslate(pos.data);
		gTransform((size*unifmat2).data);
		gDrawCircle();
		
		gTranslate((pos + 0.5f*dir*size*float(M_SQRT2)).data);
		gTransform(((0.5f*size)*ori).data);
		gDrawQuad();
	}
	
	void drawUnitDst(const Unit *u)
	{
		float size = sizeWtoS(u->getSize());
		
		fvec2 dst = posWtoS(u->getDst());
		
		gTranslate(dst.data);
		gTransform((0.6f*size*unifmat2).data);
		gDrawRing(0.5);
	}
	
	void drawObject(const Object *o)
	{
		float size = sizeWtoS(o->getSize());
		
		fvec2 pos = posWtoS(o->getPos());
		
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
};
