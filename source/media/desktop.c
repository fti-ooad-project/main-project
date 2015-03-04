#ifndef __ANDROID__

#define DEBUG

#include "media.h"

#include <stdlib.h>
#include <stdio.h>

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/glu.h>

typedef struct PlatformContext
{
	SDL_Window *window;
	SDL_GLContext context;
	
	int width;
	int height;
	
	int mouse[3];
	
	int init_event_pushed;
}
PlatformContext;

static void pushAppEvent(Media_App *app, const Media_AppEvent *event) 
{
	if(app->listeners.app) { app->listeners.app(app,event); }
}
static void pushSurfaceEvent(Media_App *app, const Media_SurfaceEvent *event) 
{
	if(app->listeners.surface) { app->listeners.surface(app,event); }
}
static void pushMotionEvent(Media_App *app, const Media_MotionEvent *event) 
{
	if(app->listeners.motion) { app->listeners.motion(app,event); }
}
/*
static void pushSensorEvent(Media_App *app, const Media_SensorEvent *event) 
{
	if(app->listeners.sensor) { app->listeners.sensor(app,event); }
}
*/

int __initDisplay(PlatformContext *context)
{
	context->width = 800;
	context->height = 600;

	context->window = SDL_CreateWindow(
			"MediaApp",
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			context->width, context->height,
			SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
		);
	
	if(context->window == NULL)
	{
#ifdef DEBUG
		printf("Could not create SDL_Window\n");
#endif
		return -1;
	}
	
	context->context = SDL_GL_CreateContext(context->window);
	
	if(context->context == NULL)
	{
#ifdef DEBUG
		printf("Could not create SDL_GL_Context\n");
#endif
		return -2;
	}

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE,5);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,6);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,5);
	SDL_GL_SetSwapInterval(1);

	GLenum glew_status = glewInit();
	if(GLEW_OK != glew_status)
	{
#ifdef DEBUG
		printf("Could not init glew: %s\n",glewGetErrorString(glew_status));
#endif
		return -3;
	}
	if(!GLEW_VERSION_2_0)
	{
#ifdef DEBUG
		printf("No support for OpenGL 2.0 found\n");
#endif
		return -4;
	}
	
	return 0;
}

void __termDisplay(PlatformContext *context)
{
	SDL_GL_DeleteContext(context->context);
	SDL_DestroyWindow(context->window);
}

static void __handleEvent(Media_App *app, PlatformContext *context, const SDL_Event *event)
{
	Media_AppEvent app_event;
	Media_SurfaceEvent surface_event;
	Media_MotionEvent motion_event;
	if(!context->init_event_pushed)
	{
		surface_event.w = context->width;
		surface_event.h = context->height;
		surface_event.type = MEDIA_SURFACE_INIT;
		pushSurfaceEvent(app,&surface_event);
		surface_event.type = MEDIA_SURFACE_RESIZE;
		pushSurfaceEvent(app,&surface_event);
		context->init_event_pushed = 1;
	}
	
	switch(event->type)
	{
	case SDL_QUIT:
		surface_event.type = MEDIA_SURFACE_TERM;
		pushSurfaceEvent(app,&surface_event);
		app_event.type = MEDIA_APP_QUIT;
		pushAppEvent(app,&app_event);
		break;
	case SDL_WINDOWEVENT:
		switch(event->window.event) 
		{
		case SDL_WINDOWEVENT_RESIZED:
			context->width = event->window.data1;
			context->height = event->window.data2;
			surface_event.type = MEDIA_SURFACE_RESIZE;
			surface_event.w = context->width;
			surface_event.h = context->height;
			pushSurfaceEvent(app,&surface_event);
			break;
		/*
		// Not used due to bad behavior
		case SDL_WINDOWEVENT_MAXIMIZED:
			app_event.type = MEDIA_APP_SHOW;
			pushAppEvent(app,&app_event);
			break;
		case SDL_WINDOWEVENT_MINIMIZED:
			app_event.type = MEDIA_APP_HIDE;
			pushAppEvent(app,&app_event);
			break;
		
		*/
		default:
			break;
		}
		break;
	case SDL_MOUSEMOTION:
		if(context->mouse[0])
		{
			motion_event.action = MEDIA_ACTION_MOVE;
			motion_event.x = event->motion.x;
		  motion_event.y = event->motion.y;
			pushMotionEvent(app,&motion_event);
		}
		break;
	case SDL_MOUSEBUTTONDOWN:
		if(event->button.button == SDL_BUTTON_LEFT)
		{
			context->mouse[0] = 1;
			motion_event.action = MEDIA_ACTION_DOWN;
			motion_event.x = event->button.x;
		  motion_event.y = event->button.y;
			pushMotionEvent(app,&motion_event);
		}
		break;
	case SDL_MOUSEBUTTONUP:
		if(event->button.button == SDL_BUTTON_LEFT)
		{
			context->mouse[0] = 0;
			motion_event.action = MEDIA_ACTION_UP;
			motion_event.x = event->button.x;
		  motion_event.y = event->button.y;
			pushMotionEvent(app,&motion_event);
		}
		break;
	default:
		break;
	}
}

void Media_handleEvents(Media_App *app)
{
	SDL_Event event;
	while(SDL_PollEvent(&event))
	{
		__handleEvent(app,(PlatformContext*)(app->platform_context),&event);
	}
}

void Media_waitForEvent(Media_App *app)
{
	SDL_Event event;
	if(SDL_WaitEvent(&event))
	{
		__handleEvent(app,(PlatformContext*)(app->platform_context),&event);
	}
}

void Media_renderFrame(Media_App *app)
{
	if(app->renderer != NULL)
	{
		app->renderer(app);
	}
	SDL_GL_SwapWindow(((PlatformContext*)(app->platform_context))->window);
}

int Media_enableSensor(Media_App *app, Media_SensorType type, unsigned long rate)
{
	return -1;
}

int Media_disableSensor(Media_App *app, Media_SensorType type)
{
	return -1;
}

int main(int argc, char *argv[])
{
	Media_App app;
	PlatformContext context;
	
	app.data = NULL;
	app.renderer = NULL;
	app.listeners.app = NULL;
	app.listeners.surface = NULL;
	app.listeners.motion = NULL;
	app.listeners.sensor = NULL;
	
	context.init_event_pushed = 0;
	
	int returned_value;
	
	__initDisplay(&context);
	
	app.platform_context = (void*)&context;
	returned_value = Media_main(&app);
	
	__termDisplay(&context);
	
	return returned_value;
}

#endif
