#include <list>

#include <media/media.h>
#include <graphics/graphics.h>

#include "unit.hpp"
#include "division.hpp"
#include "processor.hpp"
#include "view.hpp"

struct State
{
	int done;
	int wait;
	int width;
	int height;
	bool ready;
	
	Processor proc;
	std::list<Unit*> units;
	Division division;
};

void handleAppEvent(Media_App *app, const Media_AppEvent *event)
{
	State *state = static_cast<State*>(app->data);
	switch(event->type)
	{
	case MEDIA_APP_SAVESTATE:
		printInfo("Save state\n");
		break;
	case MEDIA_APP_QUIT:
		state->done = 1;
		printInfo("Quit\n");
		break;
	case MEDIA_APP_SHOW:
		state->wait = 0;
		printInfo("Show\n");
		break;
	case MEDIA_APP_HIDE:
		state->wait = 1;
		printInfo("Hide\n");
		break;
	default:
		break;
	}
}

void handleSurfaceEvent(Media_App *app, const Media_SurfaceEvent *event)
{
	State *state = static_cast<State*>(app->data);
	switch(event->type)
	{
	case MEDIA_SURFACE_INIT:
		printInfo("Init surface\n");
		initGraphics();
		break;
	case MEDIA_SURFACE_TERM:
		printInfo("Term surface\n");
		disposeGraphics();
		break;
	case MEDIA_SURFACE_RESIZE:
		printInfo("Resize surface ( %d, %d )\n",event->w,event->h);
		state->width = event->w;
		state->height = event->h;
		resizeGraphics(event->w,event->h);
		state->ready = true;
		break;
	default:
		break;
	}
}

void handleMotionEvent(Media_App *app, const Media_MotionEvent *event)
{
	// State *state = static_cast<State*>(app->data);
	switch(event->action)
	{
	case MEDIA_ACTION_UP:
		//printInfo("Up\n");
		break;
	case MEDIA_ACTION_DOWN:
		//printInfo("Down\n");
		break;
	case MEDIA_ACTION_MOVE:
		//printInfo("Move\n");
		break;
	default:
		break;
	}
	//printInfo("Motion ( %d, %d )\n", static_cast<int>(pos.x()), static_cast<int>(pos.y()));
}

void handleSensorEvent(Media_App *app, const Media_SensorEvent *event)
{
	switch(event->sensor)
	{
	case MEDIA_SENSOR_ACCELEROMETER:
		// printInfo("Accelerometer ( %f, %f, %f)\n",event->x,event->y,event->z);
		break;
	default:
		break;
	}
}

void render(Media_App *app)
{
	State *state = static_cast<State*>(app->data);
	auto &units = state->units;
	
	clear();
	setColorInt(RED);
	for(Unit *u : units)
	{
		drawUnit(u);
	}
}

int Media_main(Media_App *app)
{
	State state = {0,0,0,0,false};
	state.division.setWidth(0x4);
	
	for(int i = 0; i < 0x10; ++i)
	{
		Unit *u;
		u = new Unit();
		u->setInvMass(1.0/80.0);
		u->setSize(0.25);
		u->setPos(vec2(8-i,-2));
		state.units.push_back(u);
		state.division.addUnit(u);
		state.proc.addObject(u);
		state.proc.addUnit(u);
	}
	
	app->data = static_cast<void*>(&state);

	app->listeners.app = &handleAppEvent;
	app->listeners.surface = &handleSurfaceEvent;
	app->listeners.motion = &handleMotionEvent;
	app->listeners.sensor = &handleSensorEvent;

	app->renderer = &render;

	for(;;)
	{
		if(state.wait)
		{
			Media_waitForEvent(app);
		}
		Media_handleEvents(app);

		if(state.done)
		{
			break;
		}

		if(state.ready)
		{
			state.proc.attract();
			state.proc.move(0.032);
			state.proc.interact();
		}

		// printInfo("Frame\n");
		Media_renderFrame(app);
	}
	
	for(Unit *u : state.units)
	{
		delete u;
	}

	return 0;
}
