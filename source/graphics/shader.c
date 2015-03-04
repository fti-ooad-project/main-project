#define DEBUG

#ifdef __ANDROID__
#include <GLES2/gl2.h>
#else
#include <GL/glew.h>
#endif

#include <stdlib.h>
#include <string.h>

#include "shader.h"

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

static void printShaderCompilationErrors(GLuint id, const char *name)
{
	int   infologLen   = 0;
	int   charsWritten = 0;
	char *infoLog;
	glGetShaderiv(id, GL_INFO_LOG_LENGTH, &infologLen);
	if(infologLen > 1)
	{
		infoLog = (char*)malloc(sizeof(char)*infologLen);
		glGetShaderInfoLog(id, infologLen, &charsWritten, infoLog);
		printWarn("%s:\n%s\n",name,infoLog);
		free(infoLog);
	}
}

static void printProgramLinkingErrors(GLuint id, const char *name)
{
	int link_ok;
	glGetProgramiv(id, GL_LINK_STATUS, &link_ok);
	if(!link_ok) {
		printWarn("%s: Linking error",name);
	}
}

/*
static void checkError() 
{
	GLint i;
	for (i = glGetError(); i; i = glGetError())
	{
		printWarn("glError 0x%x\n", i);
	}
}
*/

#endif

static GLint pullUniform(GLuint prog, const char *name)
{
	GLint unif;
	unif = glGetUniformLocation(prog, name);
#ifdef DEBUG
	if(unif == -1)
	{
		printInfo("Could not bind uniform '%s'\n",name);
	}
#endif
	return unif;
}

static GLint pullAttribute(GLuint prog, const char *name)
{
	GLint attr;
	attr = glGetAttribLocation(prog, name);
#ifdef DEBUG
	if(attr == -1)
	{
		printInfo("Could not bind attrib '%s'\n",name);
	}
#endif
	return attr;
}

static void pullVariables(Program *prog)
{
	int i;
	
	for(i = 0; i < prog->attribs_length; ++i)
	{
		prog->attribs[i].id = pullAttribute(prog->id,prog->attribs[i].name);
	}
	
	for(i = 0; i < prog->uniforms_length; ++i)
	{
		prog->uniforms[i].id = pullUniform(prog->id,prog->uniforms[i].name);
	}
}

GLint getVariable(Variable *vars, int vars_length, const GLchar *name)
{
	int i;
	for(i = 0; i < vars_length; ++i)
	{
		if(!strcmp(name,vars[i].name))
		{
			return vars[i].id;
		}
	}
	return -1;
}

GLint getUniform(Program *prog, const GLchar *name)
{
	return getVariable(prog->uniforms,prog->uniforms_length,name);
}

GLint getAttribute(Program *prog, const GLchar *name)
{
	return getVariable(prog->attribs,prog->attribs_length,name);
}

Shader *createShader(GLenum type,
    const char *name,
    const GLchar *source,
    const GLchar **uniforms,
    int uniforms_length,
    const GLchar **attribs,
    int attribs_length
)
{
	Shader *shader = (Shader*)malloc(sizeof(Shader));
	shader->name = name;
	shader->source = source;
	
	int i;
	
	shader->attribs_lenght = attribs_length;
	shader->attribs = (const GLchar**)malloc(attribs_length);
	for(i = 0; i < attribs_length; ++i)
	{
		shader->attribs[i] = attribs[i];
	}
	
	shader->uniforms_lenght = uniforms_length;
	shader->uniforms = (const GLchar**)malloc(uniforms_length);
	for(i = 0; i < uniforms_length; ++i)
	{
		shader->uniforms[i] = uniforms[i];
	}
	
	shader->id = glCreateShader(type);
	glShaderSource(shader->id, 1, &(shader->source), NULL);
	glCompileShader(shader->id);
#ifdef DEBUG
	printShaderCompilationErrors(shader->id,shader->name);
#endif
	
	return shader;
}

Program *createProgram(
    const char *name,
    const Shader *vert,
    const Shader *frag
)
{
	Program *prog = (Program*)malloc(sizeof(Program));
	prog->name = name;
	prog->vertex = vert;
	prog->fragment = frag;
	
	int i;
	
	prog->attribs_length = vert->attribs_lenght;
	prog->attribs = (Variable*)malloc(sizeof(Variable)*prog->attribs_length);
	for(i = 0; i < vert->attribs_lenght; ++i)
	{
		prog->attribs[i].name = vert->attribs[i];
	}
	
	prog->uniforms_length = vert->uniforms_lenght + frag->uniforms_lenght;
	prog->uniforms = (Variable*)malloc(sizeof(Variable)*prog->uniforms_length);
	for(i = 0; i < vert->uniforms_lenght; ++i)
	{
		prog->uniforms[i].name = vert->uniforms[i];
	}
	for(i = 0; i < frag->uniforms_lenght; ++i)
	{
		prog->uniforms[i + vert->uniforms_lenght].name = frag->uniforms[i];
	}
	
	prog->id = glCreateProgram();
	glAttachShader(prog->id, vert->id);
	glAttachShader(prog->id, frag->id);
	
	glLinkProgram(prog->id);
#ifdef DEBUG
	printProgramLinkingErrors(prog->id,prog->name);
#endif
	
	pullVariables(prog);
	
	return prog;
}

void destroyProgram(Program *prog)
{
	glDetachShader(prog->id, prog->vertex->id);
	glDetachShader(prog->id, prog->fragment->id);

	glDeleteProgram(prog->id);
	
	free(prog->attribs);
	free(prog->uniforms);
	free(prog);
}

void destroyShader(Shader *shader)
{
	glDeleteShader(shader->id);
	
	free(shader->attribs);
	free(shader->uniforms);
	free(shader);
}
