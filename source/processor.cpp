#include "processor.hpp"

#include <4u/la/vec.hpp>
#include <4u/la/mat.hpp>

static const double 
  DELTA = 1e-1,
  DELTA2 = 1e-2,
	RDELTA = 1e-4;

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

void Processor::addDivision(Division *d)
{
	divisions.push_back(d);
}

void Processor::removeDivision(Division *d)
{
	divisions.remove(d);
}

void Processor::attract()
{
	for(Unit *u : units)
	{
		vec2 dist = u->getDst() - u->getPos();
		double len2 = dist*dist;
		if(len2 > DELTA2)
		{
			double len = sqrt(len2);
			vec2 dir = dist/len;
			u->setDir(dir);
			vec2 vel = dir*u->getSpd();
			
			double msd = 0.5;
			if(len < msd)
			{
				vel *= len/msd;
			}
			u->setVel(vel);
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
	for(Division *d : divisions)
	{
		vec2 dist = d->getDestination() - d->getPosition();
		if(dist*dist > DELTA2)
		{
			vec2 pos = d->getPosition();
			d->setPosition(pos + dt*norm(d->getDestination() - pos)*d->getSpeed());
		}
		
		vec2 dir = norm(dist);
		if(dir*d->getDirection() < 1.0 - RDELTA)
		{
			double da = d->getAngularSpeed()*dt;
			if((dir^d->getDirection()) < 0.0)
			{
				da = -da;
			}
			vec2 ndir = norm((unimat2 + mat2(0,1,-1,0)*da)*d->getDirection());
			d->setDirection(ndir);
		}
		d->updatePositions();
	}
}

void Processor::interact(double dev)
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
					double d0 = dev*o0->getInvMass(), d1 = dev*o1->getInvMass();
					o0->setPos(o0->getPos() + dir*d0);
					o1->setPos(o1->getPos() - dir*d1);
				}
			}
		}
	}
}
