#define DEBUG

#ifdef __ANDROID__
#include <GLES2/gl2.h>
#else
#include <GL/glew.h>
#endif

#ifdef DEBUG

#ifdef __ANDROID__
#include <android/log.h>
#define printInfo(...) ((void)__android_log_print(ANDROID_LOG_INFO, "libmedia_log", __VA_ARGS__))
#define printWarn(...) ((void)__android_log_print(ANDROID_LOG_WARN, "libmedia_log", __VA_ARGS__))
#else
#include <stdio.h>
#define printInfo(...) (fprintf(stdout,__VA_ARGS__))
#define printWarn(...) (fprintf(stderr,__VA_ARGS__))
#endif

static void checkError() 
{
	GLint i;
	for (i = glGetError(); i; i = glGetError())
	{
		printWarn("glError 0x%x\n", i);
	}
}

#endif

#include "shader.h"
#include "shader_source.h"

/* Constants */
static const float NULLVEC[2] = {0,0};
static const float NULLMAT[4] = {0,0,0,0};
static const float UNIMAT[4] = {1,0,0,1};

typedef struct Buffers
{
	GLuint quad;
}
Buffers;

typedef struct Shaders
{
	Shader *vert;
	Shader *frag_fill;
	Shader *frag_quad;
	Shader *frag_circle;
	Shader *frag_ring;
}
Shaders;

typedef struct Programs
{
	Program *fill;
	Program *quad;
	Program *circle;
	Program *ring;
}
Programs;

typedef struct Context
{
	int width, height;
	
	Shaders shaders;
	Programs programs;
	
	Buffers buffers;
	
	float projection_matrix[4];
	float modelview_matrix[4];
	
	float translation[2];
	float color[4];
}
Context;
static Context context;

static void loadBuffers(Buffers *buffers)
{
	glGenBuffers(1, &buffers->quad);
	glBindBuffer(GL_ARRAY_BUFFER, buffers->quad);
	float quad[12] = {
		1.0f,1.0f,
		-1.0f,1.0f,
		-1.0f,-1.0f,
		1.0f,1.0f,
		-1.0f,-1.0f,
		1.0f,-1.0f
	};
	glBufferData(GL_ARRAY_BUFFER, 2*3*2*sizeof(float), quad, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

static void deleteBuffers(Buffers *buffers)
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &buffers->quad);
}

static void loadShaders(Shaders *shaders)
{
	shaders->vert = createShader(
	  GL_VERTEX_SHADER, "vertex", SRC_VERT,
	  UNIF_VERT, ARRAY_SIZE(UNIF_VERT),
	  ATTR_VERT, ARRAY_SIZE(ATTR_VERT)
	);
	shaders->frag_fill = createShader(
	  GL_FRAGMENT_SHADER, "fill", SRC_FRAG_FILL,
	  UNIF_FRAG_FILL, ARRAY_SIZE(UNIF_FRAG_FILL), NULL, 0
	);
	shaders->frag_quad = createShader(
	  GL_FRAGMENT_SHADER, "quad", SRC_FRAG_QUAD,
	  UNIF_FRAG_QUAD, ARRAY_SIZE(UNIF_FRAG_QUAD), NULL, 0
	);
	shaders->frag_circle = createShader(
	  GL_FRAGMENT_SHADER, "circle", SRC_FRAG_CIRCLE,
	  UNIF_FRAG_CIRCLE, ARRAY_SIZE(UNIF_FRAG_CIRCLE), NULL, 0
	);
	shaders->frag_ring = createShader(
	  GL_FRAGMENT_SHADER, "ring", SRC_FRAG_RING,
	  UNIF_FRAG_RING, ARRAY_SIZE(UNIF_FRAG_RING), NULL, 0
	);
}

static void deleteShaders(Shaders *shaders)
{
	destroyShader(shaders->vert);
	destroyShader(shaders->frag_fill);
	destroyShader(shaders->frag_quad);
	destroyShader(shaders->frag_circle);
	destroyShader(shaders->frag_ring);
}

static void loadPrograms(Programs *progs, Shaders *shaders)
{
	progs->fill = createProgram("fill",shaders->vert,shaders->frag_fill);
	progs->quad = createProgram("quad",shaders->vert,shaders->frag_quad);
	progs->circle = createProgram("circle",shaders->vert,shaders->frag_circle);
	progs->ring = createProgram("ring",shaders->vert,shaders->frag_ring);
}

static void deletePrograms(Programs *progs)
{
	destroyProgram(progs->fill);
	destroyProgram(progs->quad);
	destroyProgram(progs->circle);
	destroyProgram(progs->ring);
}

static void setTranslation(float x, float y)
{
	context.translation[0] = x;
	context.translation[1] = y;
}

static void setModelview(float a00, float a01, float a10, float a11)
{
	context.modelview_matrix[0] = a00;
	context.modelview_matrix[1] = a01;
	context.modelview_matrix[2] = a10;
	context.modelview_matrix[3] = a11;
}
static void setProjection(float a00, float a01, float a10, float a11)
{
	context.projection_matrix[0] = a00;
	context.projection_matrix[1] = a01;
	context.projection_matrix[2] = a10;
	context.projection_matrix[3] = a11;
}

/* Initializes graphics subsystem */
int initGraphics()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);

	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT);

	loadShaders(&(context.shaders));
	loadPrograms(&(context.programs),&(context.shaders));
	loadBuffers(&(context.buffers));
	
	setModelview(1,0,0,1);
	
	setTranslation(0,0);
	
	//printInfo("initGraphics()\n");

#ifdef DEBUG
	checkError();
#endif
	
	return 0;
}

/* Safely disposes graphics subsystem */
int disposeGraphics()
{
	deleteBuffers (&(context.buffers));
	deletePrograms(&(context.programs));
	deleteShaders (&(context.shaders));
	
	//printInfo("disposeGraphics()\n");

#ifdef DEBUG
	checkError();
#endif
	
	return 0;
}

/* Performs resizing of window */
void resizeGraphics(int width, int height)
{
	context.width = width;
	context.height = height;
	
	glViewport(0,0,width,height);
	
	setProjection(2.0f/width,0,0,2.0f/height);
	
	//printInfo("resizeGraphics(%d,%d)\n",width,height);
	
#ifdef DEBUG
	checkError();
#endif
}

void translate(const float *vector)
{
	setTranslation(vector[0],vector[1]);
}

void transform(const float *matrix)
{
	setModelview(matrix[0],matrix[1],matrix[2],matrix[3]);
}

#define __red(c)   ((c>>0)&0xff)
#define __green(c) ((c>>8)&0xff)
#define __blue(c)  ((c>>16)&0xff)
#define __alpha(c) ((c>>24)&0xff)

#define __tofloat(c) (c*(1.0f/255.0f))

void setColorInt(unsigned color)
{
	context.color[0] = __tofloat(__red(color));
	context.color[1] = __tofloat(__green(color));
	context.color[2] = __tofloat(__blue(color));
	context.color[3] = __tofloat(__alpha(color));
}

void setColor(const float *color)
{
	context.color[0] = color[0];
	context.color[1] = color[1];
	context.color[2] = color[2];
	context.color[3] = color[3];
}

void clear()
{
	glClear(GL_COLOR_BUFFER_BIT);
}

static void setVectorUniforms(Program *prog, const float *v, const float *m, const float *p)
{
	glUniform2fv(getUniform(prog,U_TRANSLATION), 1, v);
	glUniformMatrix2fv(getUniform(prog,U_PROJECTION), 1, GL_FALSE, p);
	glUniformMatrix2fv(getUniform(prog,U_MODELVIEW), 1, GL_FALSE, m);
}

static void setColorUniform(Program *prog, const float *c)
{
	glUniform4fv(getUniform(prog,U_COLOR), 1, c);
}

static void drawArray(Program *prog)
{
		glEnableVertexAttribArray(getAttribute(prog,A_COORD));
		{
			glBindBuffer(GL_ARRAY_BUFFER, context.buffers.quad);
			glVertexAttribPointer(getAttribute(prog,A_COORD), 2, GL_FLOAT, GL_FALSE, 0, NULL);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glDrawArrays(GL_TRIANGLES,0,6);
		}
		glDisableVertexAttribArray(getAttribute(prog,A_COORD));
}

void fill()
{
	Program *prog = context.programs.fill;
	glUseProgram(prog->id);
	{
		setVectorUniforms(prog,context.translation,context.modelview_matrix,context.projection_matrix);
		setColorUniform(prog,context.color);
		
		drawArray(prog);
	}
	glUseProgram(0);
#ifdef DEBUG
	checkError();
#endif
}

void drawQuad()
{
	Program *prog = context.programs.quad;
	glUseProgram(prog->id);
	{
		setVectorUniforms(prog,context.translation,context.modelview_matrix,context.projection_matrix);
		setColorUniform(prog,context.color);
		
		drawArray(prog);
	}
	glUseProgram(0);
#ifdef DEBUG
	checkError();
#endif
}

void drawCircle()
{
	Program *prog = context.programs.circle;
	glUseProgram(prog->id);
	{
		setVectorUniforms(prog,context.translation,context.modelview_matrix,context.projection_matrix);
		setColorUniform(prog,context.color);
		
		drawArray(prog);
	}
	glUseProgram(0);
#ifdef DEBUG
	checkError();
#endif
}

void drawRing(float in)
{
	Program *prog = context.programs.ring;
	glUseProgram(prog->id);
	{
		setVectorUniforms(prog,context.translation,context.modelview_matrix,context.projection_matrix);
		setColorUniform(prog,context.color);
		glUniform1fv(getUniform(prog,U_INNER_MUL), 1, &in);
		
		drawArray(prog);
	}
	glUseProgram(0);
#ifdef DEBUG
	checkError();
#endif
}
