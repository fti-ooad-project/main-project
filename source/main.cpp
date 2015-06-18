#define SERVER

#include <list>

#include <media/media.h>
#include <graphics/graphics.h>

#include "input.hpp"
#include "view.hpp"

#ifdef SERVER
#include <engine/localsession.hpp>
#else
#include <engine/remotesession.hpp>
#endif

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
	const int port = 18765;
#ifdef SERVER
	LocalSession session = LocalSession(1,port);
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
#else
	RemoteSession session("192.168.0.65",port);
#endif
	
	View view = View(session.getSpectator());
	
	Input input = Input(app,&view,
#ifdef SERVER
	  session.getPlayerHandle(0)
#else
	  nullptr
#endif
	);
	
	Set set = {&session,&input,&view};
	app->data = static_cast<void*>(&set);

	app->renderer = &render;
	
	while(input.handle())
	{
#ifdef SERVER
		session.process(0.04);
#endif
		Media_renderFrame(app);
	}

	return 0;
}
