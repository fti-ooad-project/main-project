#pragma once

#include <list>

#include "object.hpp"
#include "unit.hpp"
#include "division.hpp"

class Processor
{
private:
	std::list<Object*> objects;
	std::list<Unit*> units;
	std::list<Division*> divisions;
	
public:
	Processor();
	virtual ~Processor();
	
	void addObject(Object *o);
	void removeObject(Object *o);
	
	void addUnit(Unit *u);
	void removeUnit(Unit *u);
	
	void addDivision(Division *d);
	void removeDivision(Division *d);
	
	void attract();
	void move(double dt);
	void interact(double dev);
};
