#pragma once

#include <cmath>

#include <4u/la/vec.hpp>
#include <4u/la/mat.hpp>

#include <graphics/graphics.h>

#include "unit.hpp"


class View
{
public:
	float factor = 40.0f;
	vec2 deviation = nullvec2;
	
	void drawUnit(const Unit *u)
	{
		float size = sizeWtoS(u->getSize());
		
		fvec2 pos = posWtoS(u->getPos());
		fvec2 dir = u->getDir();
		float cosa = (dir.x() - dir.y())*M_SQRT1_2, sina = (dir.x() + dir.y())*M_SQRT1_2;
		fmat2 ori = fmat2(cosa,sina,-sina,cosa);
		
		translate(pos.data);
		transform((size*unifmat2).data);
		drawCircle();
		
		translate((pos + 0.5f*dir*size*float(M_SQRT2)).data);
		transform(((0.5f*size)*ori).data);
		drawQuad();
	}
	
	void drawUnitDst(const Unit *u)
	{
		float size = sizeWtoS(u->getSize());
		
		fvec2 dst = posWtoS(u->getDst());
		
		translate(dst.data);
		transform((0.6f*size*unifmat2).data);
		drawRing(0.5);
	}
	
	void drawObject(const Object *o)
	{
		float size = sizeWtoS(o->getSize());
		
		fvec2 pos = posWtoS(o->getPos());
		
		translate(pos.data);
		transform((size*unifmat2).data);
		drawCircle();
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
