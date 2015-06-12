#include <list>

#include <media/media.h>
#include <graphics/graphics.h>

#include "input.hpp"
#include "view.hpp"

#include <engine/session.hpp>

struct Set
{
	Session *session;
	
	Input *input;
	
	View *view;
};

void render(Media_App *app)
{
	Set *set = static_cast<Set*>(app->data);
	
	gClear();
	
	set->view->draw();
	
	set->input->drawUI();
}

int Media_main(Media_App *app)
{
	Session session = Session(1);
	PlayerHandle *handle = session.getPlayerHandle(0);
	DivisionID swordsmen = handle->purchaseDivision(UNIT_SWORDSMAN,0x100);
	DivisionID archers = handle->purchaseDivision(UNIT_ARCHER,0x40);
	
	// session.loadMap();
	
	handle->forDivisionHandleID(swordsmen,[](DivisionHandle *division)
	{
		division->setWidth(0x14);
		division->setPosition(vec2(0,-8));
		division->setDistance(0.8);
		division->setDestination(division->getPosition());
	});
	
	handle->forDivisionHandleID(archers,[](DivisionHandle *division)
	{
		division->setWidth(0x8);
		division->setPosition(vec2(0,8));
		division->setDistance(1.2);
		division->setDestination(division->getPosition());
	});
	
	View view = View(session.getSpectator());
	
	Input input = Input(app,&view,session.getPlayerHandle(0));
	input.setDivisionID(swordsmen);
	
	Set set = {&session,&input,&view};
	app->data = static_cast<void*>(&set);

	app->renderer = &render;
	
	while(input.handle())
	{
		session.process(0.04);
		
		Media_renderFrame(app);
	}

	return 0;
}
