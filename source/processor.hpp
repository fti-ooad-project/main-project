#pragma once

#include <list>

#include "object.hpp"
#include "unit.hpp"

class Processor
{
private:
	std::list<Object*> objects;
	std::list<Unit*> units;
	
public:
	Processor();
	virtual ~Processor();
	
	void addObject(Object *o);
	void removeObject(Object *o);
	
	void addUnit(Unit *u);
	void removeUnit(Unit *u);
	
	void attract();
	void move(double dt);
	void interact();
};
