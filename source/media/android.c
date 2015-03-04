/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifdef __ANDROID__

#include "media.h"

#include <jni.h>
#include <errno.h>

#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include <android/sensor.h>
#include <android/log.h>
#include <android_native_app_glue.h>

#define LOGI(...) (printInfo(__VA_ARGS__))
#define LOGW(...) (printWarn(__VA_ARGS__))

static int  initDisplay();
static void termDisplay();

struct engine 
{
	struct android_app* app;
	
	EGLDisplay display;
	EGLSurface surface;
	EGLContext context;
	int32_t width;
	int32_t height;
	int frame_counter;
  
	ASensorManager* sensorManager;
	const ASensor* accelerometerSensor;
	ASensorEventQueue* sensorEventQueue;
};

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
static void pushSensorEvent(Media_App *app, const Media_SensorEvent *event) 
{
	if(app->listeners.sensor) { app->listeners.sensor(app,event); }
}

int __initDisplay(struct engine *engine)
{
	// initialize OpenGL ES and EGL

	/*
	 * Here specify the attributes of the desired configuration.
	 * Below, we select an EGLConfig with at least 8 bits per color
	 * component compatible with on-screen windows
	 */
	const EGLint attribs[] = 
	{
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_BLUE_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_RED_SIZE, 8,
		EGL_NONE
	};
	EGLint w, h, dummy, format;
	EGLint numConfigs;
	EGLConfig config;
	EGLSurface surface;
	EGLContext context;

	EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

	eglInitialize(display, 0, 0);

	/* Here, the application chooses the configuration it desires. In this
	 * sample, we have a very simplified selection process, where we pick
	 * the first EGLConfig that matches our criteria */
	eglChooseConfig(display, attribs, &config, 1, &numConfigs);

	/* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
	 * guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
	 * As soon as we picked a EGLConfig, we can safely reconfigure the
	 * ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
	eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

	ANativeWindow_setBuffersGeometry(engine->app->window, 0, 0, format);

	surface = eglCreateWindowSurface(display, config, engine->app->window, NULL);
	
	EGLint attrib_list[] = 
	{
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};
	
	context = eglCreateContext(display, config, NULL, attrib_list);

	if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) 
	{
#ifdef DEBUG
		LOGW("Unable to eglMakeCurrent");
#endif
		return -1;
	}

	eglQuerySurface(display, surface, EGL_WIDTH, &w);
	eglQuerySurface(display, surface, EGL_HEIGHT, &h);

	engine->display = display;
	engine->context = context;
	engine->surface = surface;
	engine->width = w;
	engine->height = h;

	// Initialize GL state.
	glEnable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	return 0;
}

void __termDisplay(struct engine *engine)
{
	if (engine->display != EGL_NO_DISPLAY) {
		eglMakeCurrent(engine->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		if (engine->context != EGL_NO_CONTEXT) {
			eglDestroyContext(engine->display, engine->context);
		}
		if (engine->surface != EGL_NO_SURFACE) {
			eglDestroySurface(engine->display, engine->surface);
		}
		eglTerminate(engine->display);
	}

	engine->display = EGL_NO_DISPLAY;
	engine->context = EGL_NO_CONTEXT;
	engine->surface = EGL_NO_SURFACE;
}

/**
 * Process the next input event.
 */
static int32_t engine_handle_input(struct android_app* android_app, AInputEvent* event)
{
	Media_App *app = (Media_App*)(android_app->userData);
	Media_MotionEvent motion_event;
	int32_t action;
	switch(AInputEvent_getType(event))
	{
	case AINPUT_EVENT_TYPE_MOTION:
		switch(AInputEvent_getSource(event))
		{
		case AINPUT_SOURCE_TOUCHSCREEN:
			action = AKeyEvent_getAction(event);
			motion_event.index = action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK;
			switch(action & AMOTION_EVENT_ACTION_MASK)
			{
			case AMOTION_EVENT_ACTION_DOWN:
				motion_event.action = MEDIA_ACTION_DOWN;
				break;
			case AMOTION_EVENT_ACTION_UP:
				motion_event.action = MEDIA_ACTION_UP;
				break;
			case AMOTION_EVENT_ACTION_MOVE:
				motion_event.action = MEDIA_ACTION_MOVE;
				break;
			}
			break;
		}
		motion_event.x = AMotionEvent_getX(event, 0);
		motion_event.y = AMotionEvent_getY(event, 0);
		pushMotionEvent(app,&motion_event);
		return 1;
	/*
	case AINPUT_EVENT_TYPE_KEY:
		// handle key input...
		return 1;
	*/
  }
  
	return 0;
}

/**
 * Process the next main command.
 */
static void engine_handle_cmd(struct android_app* android_app, int32_t cmd) 
{
	Media_App *app = (Media_App*)(android_app->userData);
	struct engine *engine = (struct engine*)(app->platform_context);
	Media_AppEvent app_event;
	Media_SurfaceEvent surface_event;
#ifdef DEBUG
	printInfo("Command %d\n",cmd);
#endif
	switch(cmd) 
	{
	case APP_CMD_SAVE_STATE:
		app_event.type = MEDIA_APP_SAVESTATE;
		pushAppEvent(app,&app_event);
		break;
	case APP_CMD_INIT_WINDOW:
		__initDisplay(engine);
		surface_event.w = engine->width;
		surface_event.h = engine->height;
		surface_event.type = MEDIA_SURFACE_INIT;
		pushSurfaceEvent(app,&surface_event);
		surface_event.type = MEDIA_SURFACE_RESIZE;
		pushSurfaceEvent(app,&surface_event);
		break;
	case APP_CMD_TERM_WINDOW:
		__termDisplay(engine);
		surface_event.type = MEDIA_SURFACE_TERM;
		pushSurfaceEvent(app,&surface_event);
		break;
	case APP_CMD_CONFIG_CHANGED:
		engine->frame_counter = 4;
	case APP_CMD_GAINED_FOCUS:
		app_event.type = MEDIA_APP_SHOW;
		pushAppEvent(app,&app_event);
		break;
	case APP_CMD_LOST_FOCUS:
		app_event.type = MEDIA_APP_HIDE;
		pushAppEvent(app,&app_event);
		break;
	case APP_CMD_DESTROY:
		app_event.type = MEDIA_APP_QUIT;
		pushAppEvent(app,&app_event);
		break;
	default:
		break;
	}
}

static void engine_handle_events(Media_App *app, int mode)
{
	struct engine *engine = (struct engine*)(app->platform_context);
	if(engine->frame_counter > 0)
	{
		engine->frame_counter--;
		if(engine->frame_counter == 0)
		{
			Media_SurfaceEvent event;
			event.type = MEDIA_SURFACE_RESIZE;
			eglQuerySurface(engine->display, engine->surface, EGL_WIDTH, &(event.w));
			eglQuerySurface(engine->display, engine->surface, EGL_HEIGHT, &(event.h));
			pushSurfaceEvent(app,&event);
		}
	}
	
	// Read all pending events.
	int ident;
	int events;
	struct android_poll_source* source;

	// If not animating, we will block forever waiting for events.
	// If animating, we loop until all events are read, then continue
	// to draw the next frame of animation.
	while((ident=ALooper_pollAll(mode, NULL, &events, (void**)&source)) >= 0)
	{
		// LOGI("poll mode %d\n",mode);
	
		// Process this event.
		if(source != NULL) 
		{
			source->process(engine->app, source);
		}
		
		if(mode)
		{
			break;
		}
		
		// If a sensor has data, process it now.
		if (ident == LOOPER_ID_USER) 
		{
			if (engine->accelerometerSensor != NULL) 
			{
				Media_SensorEvent sensor_event;
				ASensorEvent event;
				while(ASensorEventQueue_getEvents(engine->sensorEventQueue,&event,1) > 0) 
				{
					sensor_event.sensor = MEDIA_SENSOR_ACCELEROMETER;
					sensor_event.x = event.acceleration.x;
					sensor_event.y = event.acceleration.y;
					sensor_event.z = event.acceleration.z;
					pushSensorEvent(app,&sensor_event);
				}
			}
		}
		
		// Check if we are exiting.
		if(engine->app->destroyRequested != 0) 
		{
			break;
		}
	}
}

void Media_handleEvents(Media_App *app)
{
	engine_handle_events(app,0);
}

void Media_waitForEvent(Media_App *app)
{
	engine_handle_events(app,-1);
}

void Media_renderFrame(Media_App *app)
{
	struct engine *engine = (struct engine*)(app->platform_context);
	if(engine->display == NULL) 
	{
		// No display.
		return;
	}
	if(app->renderer != NULL)
	{
		app->renderer(app);
	}
	eglSwapBuffers(engine->display, engine->surface);
}

int Media_enableSensor(Media_App *app, Media_SensorType type, unsigned long rate)
{
	struct engine *engine = (struct engine*)(app->platform_context);
	if(type == MEDIA_SENSOR_ACCELEROMETER && engine->accelerometerSensor != NULL) 
	{
		ASensorEventQueue_enableSensor(engine->sensorEventQueue,engine->accelerometerSensor);
		ASensorEventQueue_setEventRate(engine->sensorEventQueue,engine->accelerometerSensor,rate);
	}
}

int Media_disableSensor(Media_App *app, Media_SensorType type)
{
	struct engine *engine = (struct engine*)(app->platform_context);
	if(type == MEDIA_SENSOR_ACCELEROMETER && engine->accelerometerSensor != NULL) 
	{
		ASensorEventQueue_disableSensor(engine->sensorEventQueue,engine->accelerometerSensor);
	}
}

/**
 * This is the main entry point of a native application that is using
 * android_native_app_glue.  It runs in its own thread, with its own
 * event loop for receiving input events and doing other things.
 */
void android_main(struct android_app* state) 
{
	Media_App app;
	struct engine engine;
	
	// Make sure glue isn't stripped.
	app_dummy();
	
	app.data = NULL;
	app.renderer = NULL;
	app.listeners.app = NULL;
	app.listeners.surface = NULL;
	app.listeners.motion = NULL;
	app.listeners.sensor = NULL;
	
	memset(&engine,0,sizeof(struct engine));
	state->userData = &app;
	state->onAppCmd = engine_handle_cmd;
	state->onInputEvent = engine_handle_input;
	engine.app = state;
	
	app.platform_context = (void*)&engine;
	
	// Prepare to monitor accelerometer
  engine.sensorManager = ASensorManager_getInstance();
  engine.accelerometerSensor = ASensorManager_getDefaultSensor(engine.sensorManager,ASENSOR_TYPE_ACCELEROMETER);
  engine.sensorEventQueue = ASensorManager_createEventQueue(engine.sensorManager,state->looper, LOOPER_ID_USER, NULL, NULL);
	
	Media_main(&app);
}

#endif
