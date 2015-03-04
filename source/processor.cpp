#include "processor.hpp"

Processor::Processor()
{
	
}

Processor::~Processor()
{
	
}

void Processor::addObject(Object *o)
{
	objects.push_back(o);
}

void Processor::removeObject(Object *o)
{
	objects.remove(o);
}

void Processor::addUnit(Unit *u)
{
	units.push_back(u);
}

void Processor::removeUnit(Unit *u)
{
	units.remove(u);
}

void Processor::attract()
{
	for(Unit *u : units)
	{
		vec2 dist = u->getDst() - u->getPos();
		double len2 = dist*dist;
		if(len2 > 1e-2)
		{
			double len = sqrt(len2);
			vec2 dir = dist/len;
			u->setDir(dir);
			vec2 vel = dir*len;
			u->setVel(vel*vel>1.0?dir:vel);
		}
		else
		{
			u->setVel(nullvec2);
		}
	}
}

void Processor::move(double dt)
{
	for(Object *o : objects)
	{
		o->setPos(o->getPos() + dt*o->getVel());
	}
}

void Processor::interact()
{
	for(int i = 0; i < 0x10; ++i)
	{
		for(auto i0 = objects.begin(); i0 != objects.end(); ++i0)
		{
			for(auto i1 = objects.begin(); i1 != i0; ++i1)
			{
				Object *o0 = *i0;
				Object *o1 = *i1;
				vec2 dist = o0->getPos() - o1->getPos();
				double mlen = o0->getSize() + o1->getSize();
				if(dist*dist < mlen*mlen)
				{
					double len = length(o0->getPos() - o1->getPos());
					vec2 dir = dist/len;
					double dev = 0.1, d0 = dev*o0->getInvMass(), d1 = dev*o1->getInvMass();
					o0->setPos(o0->getPos() + dir*d0);
					o1->setPos(o1->getPos() - dir*d1);
				}
			}
		}
	}
}
