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
		drawUnitP(u->getPos(),u->getDir(),u->getSize());
	}
	
	void drawUnitDst(const Unit *u)
	{
		drawUnitDstP(u->getDst(),u->getSize());
	}
	
	void drawObject(const Object *o)
	{
		drawObjectP(o->getPos(),o->getSize());
	}
	
	void drawUnitP(const vec2 &upos, const vec2 &udir, double usize)
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
	
	void drawUnitDstP(const vec2 &udst, double usize)
	{
		float size = sizeWtoS(usize);
		
		fvec2 dst = posWtoS(udst);
		
		gTranslate(dst.data);
		gTransform((0.6f*size*unifmat2).data);
		gDrawRing(0.5);
	}
	
	void drawObjectP(const vec2 &upos, double usize)
	{
		float size = sizeWtoS(usize);
		
		fvec2 pos = posWtoS(upos);
		
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
