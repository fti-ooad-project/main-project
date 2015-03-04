#pragma once

#include <cmath>

#include <4u/la/vec.hpp>
#include <4u/la/mat.hpp>

#include <graphics/graphics.h>

#include "unit.hpp"

static const float FACTOR = 40.0f;

void drawUnit(const Unit *u)
{
	float size = FACTOR*u->getSize();
	
	fvec2 pos = FACTOR*u->getPos();
	fvec2 dir = u->getDir();
	float cosa = (dir.x() - dir.y())*M_SQRT1_2, sina = (dir.x() + dir.y())*M_SQRT1_2;
	fmat2 ori = fmat2(cosa,sina,-sina,cosa);
	
	translate(pos.data);
	transform((size*unifmat2).data);
	drawCircle();
	
	translate((pos + 0.5f*dir*size*float(M_SQRT2)).data);
	transform(((0.5f*size)*ori).data);
	drawQuad();
	
	fvec2 dst = FACTOR*u->getDst();
	
	translate(dst.data);
	transform((0.6f*size*unifmat2).data);
	drawRing(0.5);
}
