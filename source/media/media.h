#pragma once

#ifdef __ANDROID__
#include <android/log.h>
#define printInfo(...) ((void)__android_log_print(ANDROID_LOG_INFO, "libmedia_log", __VA_ARGS__))
#define printWarn(...) ((void)__android_log_print(ANDROID_LOG_WARN, "libmedia_log", __VA_ARGS__))
#else // DESKTOP
#include <stdio.h>
#define printInfo(...) (fprintf(stdout,__VA_ARGS__))
#define printWarn(...) (fprintf(stderr,__VA_ARGS__))
#endif

typedef unsigned int Media_EventType;
typedef unsigned int Media_ActionType;
typedef unsigned int Media_SensorType;

/* Event types */
/* Application events */
#define MEDIA_APP_QUIT         0x0101
#define MEDIA_APP_SHOW         0x0105
#define MEDIA_APP_HIDE         0x0106
#define MEDIA_APP_SAVESTATE    0x0108
/* Surface events */
#define MEDIA_SURFACE_INIT     0x0202
#define MEDIA_SURFACE_TERM     0x0203
#define MEDIA_SURFACE_RESIZE   0x0204

/* Motion actions */
#define MEDIA_ACTION_MOVE          0x01
#define MEDIA_ACTION_UP            0x02
#define MEDIA_ACTION_DOWN          0x03
#define MEDIA_ACTION_POINTER_UP    0x04
#define MEDIA_ACTION_POINTER_DOWN  0x05

/* Sensors */
#define MEDIA_SENSOR_ACCELEROMETER   0x01

typedef struct Media_AppEvent
{
	Media_EventType type;
} 
Media_AppEvent;

typedef struct Media_SurfaceEvent
{
	Media_EventType type;
	int w,h;
} 
Media_SurfaceEvent;

typedef struct Media_MotionEvent
{
	Media_ActionType action;
	int index;
	int x,y;
} 
Media_MotionEvent;

typedef struct Media_SensorEvent
{
	Media_SensorType sensor;
	double x,y,z;
} 
Media_SensorEvent;

struct Media_App;
typedef struct Media_App Media_App;

typedef struct Media_ListenerSet
{
	void(*app)    (Media_App*, const Media_AppEvent*);
	void(*surface)(Media_App*, const Media_SurfaceEvent*);
	void(*motion) (Media_App*, const Media_MotionEvent*);
	void(*sensor) (Media_App*, const Media_SensorEvent*);
}
Media_ListenerSet;

struct Media_App
{
	Media_ListenerSet listeners;
	void(*renderer)(Media_App*);
	
	/* User data */
	void *data;
	
	/* Platform-specific data */
	void *platform_context;
};

#ifdef __cplusplus
extern "C" {
#endif

/* Handles all events in event queue */
void Media_handleEvents(Media_App *app);
/* Waits for events and handles first one */
void Media_waitForEvent(Media_App *app);

/* Wraps user-defined app->renderer function */
void Media_renderFrame (Media_App *app);

/* Enables sensor to produce events with given period
 * @return non-zero if there was an error */
int Media_enableSensor (Media_App *app, Media_SensorType type, unsigned long rate);
/* Disables sensor */
int Media_disableSensor(Media_App *app, Media_SensorType type);

/* Entry point for program
 * Must be implemented */
int Media_main(Media_App *app);

#ifdef __cplusplus
}
#endif


