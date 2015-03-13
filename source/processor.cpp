#include "processor.hpp"

#include <4u/la/vec.hpp>
#include <4u/la/mat.hpp>

static const double 
  DELTA = 1e-1,
  DELTA2 = 1e-2,
	RDELTA = 1e-4;

Processor::Processor()
  : tree(TreeKey(nullvec2,64.0),4)
{
	
}

Processor::~Processor()
{
	
}

void Processor::addObject(Object *o)
{
	objects.push_back(o);
	tree.insert(TreeKey(o->getPos(),o->getSize()),o);
}

void Processor::removeObject(Object *o)
{
	objects.remove(o);
	tree.remove(TreeKey(o->getPos(),o->getSize()),o);
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
	// tree._print();
	for(Object *o : objects)
	{
		vec2 pos = o->getPos() + dt*o->getVel();
		tree.replace(
		      TreeKey(o->getPos(),o->getSize()),
		      TreeKey(pos,o->getSize()),
		      o
		      );
		o->setPos(pos);
	}
	for(Division *d : divisions)
	{
		vec2 dist = d->getDestination() - d->getPosition();
		if(dist*dist > DELTA2)
		{
			vec2 pos = d->getPosition();
			vec2 dp = dt*norm(d->getDestination() - pos)*d->getSpeed();
			d->setPosition(pos + dp);
			d->movePositions(dp);
		}
	}
}

void Processor::interact()
{
	for(int i = 0; i < 0x1; ++i)
	{
		for(Object *o0 : objects)
		{
			tree.for_nearest(
						TreeKey(o0->getPos(),o0->getSize()),
						[this,o0](std::list<std::pair<TreeKey,Object*>> &list1)
			{
				for(const std::pair<TreeKey,Object*> &p1 : list1)
				{
					Object *o1 = p1.second;
					
					if(o0 >= o1)
					{
						continue;
					}
					
					vec2 dist = o0->getPos() - o1->getPos();
					double mlen = o0->getSize() + o1->getSize();
					if(dist*dist < mlen*mlen)
					{
						double len = length(dist);
						double dev = (mlen - len)/(o0->getInvMass() + o1->getInvMass());
						vec2 dir = dist/len;
						double d0 = dev*o0->getInvMass(), d1 = dev*o1->getInvMass();
						o0->setPos(o0->getPos() + dir*d0);
						o1->setPos(o1->getPos() - dir*d1);
					}
					
				}
			});
		}
		tree.update();
	}
}
