#include <list>

#include <media/media.h>
#include <graphics/graphics.h>

#include <engine-source/unit.hpp>
#include <engine-source/division.hpp>
#include <engine-source/processor.hpp>

#include "input.hpp"
#include "view.hpp"

#include <engine/session.hpp>

struct Set
{
	std::list<Object*> *objects;
	std::list<Unit*> *units;
	View *view;
	Input *input;
};

void render(Media_App *app)
{
	Set *set = static_cast<Set*>(app->data);
	auto &units = *set->units;
	auto &objects = *set->objects;
	
	gClear();
	
	gSetColorInt(G_RED);
	for(Unit *u : units)
	{
		set->view->drawUnit(u);
		set->view->drawUnitDst(u);
	}
	
	gSetColorInt(G_YELLOW);
	for(Object *o : objects)
	{
		set->view->drawObject(o);
	}
	
	set->input->drawUI();
}

int Media_main(Media_App *app)
{
	Processor proc;
	Storage storage;
	
	proc.setStorage(&storage);
	
	std::list<Object*> objects;
	std::list<Unit*> units;
	Division division;
	
	division.setWidth(0x14);
	division.setPosition(vec2(0,0));
	division.setDistance(0.8);
	division.setDestination(division.getPosition());
	division.setDirection(vec2(0,1));
	division.setSpeed(0.5);
	
	int units_num = 0x100;
	int rows_num = sqrt(units_num);
	for(int i = 0; i < units_num; ++i)
	{
		Unit *u;
		u = new Unit();
		u->setInvMass(1.0/80.0);
		u->setSize(0.25);
		u->setPos(vec2(rows_num/2 - i/rows_num,rows_num/2 - i%rows_num));
		u->setSpd(1.0);
		units.push_back(u);
		division.addUnit(u);
		storage.addObject(u);
		storage.addUnit(u);
	}
	
	for(int i = 0; i < 0xa; ++i)
	{
		Object *o;
		o = new Object();
		o->setInvMass(0.0001);
		o->setSize(2.0);
		o->setPos(5.0*vec2(i%5 - 2,i/5));
		objects.push_back(o);
		storage.addObject(o);
	}
	
	storage.addDivision(&division);
	
	division.redistribute();
	division.updatePositions();
	
	View view;
	
	Input input(app,&view,&division);
	
	Set set = {&objects,&units,&view,&input};
	app->data = static_cast<void*>(&set);

	app->renderer = &render;
	
	while(input.handle())
	{
		int iter = 0x1;
		for(int i = 0; i < iter; ++i)
		{
			proc.attract();
			proc.move(0.04/iter);
			proc.interact();
		}
		division.updatePositions();
		
		Media_renderFrame(app);
	}
	
	for(Object *o : objects)
	{
		delete o;
	}
	for(Unit *u : units)
	{
		delete u;
	}

	return 0;
}
