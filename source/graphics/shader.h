#pragma once

#ifdef __ANDROID__
#include <GLES2/gl2.h>
#else
#include <GL/glew.h>
#endif

typedef struct Variable
{
	GLint id;
	const GLchar *name;
}
Variable;

typedef struct Shader
{
	GLuint id;
	const char *name;
	const GLchar *source;
	
	const GLchar **attribs;
	int attribs_lenght;
	
	const GLchar **uniforms;
	int uniforms_lenght;
}
Shader;

typedef struct Program
{
	GLuint id;
	const char *name;
	
	const Shader *vertex;
	const Shader *fragment;
	
	Variable *attribs;
	int attribs_length;
	
	Variable *uniforms;
	int uniforms_length;
}
Program;

GLint getUniform(Program *prog, const GLchar *name);
GLint getAttribute(Program *prog, const GLchar *name);

Shader *createShader(
    GLenum type,
    const char *name,
    const GLchar *source,
    const GLchar **uniforms,
    int uniforms_length,
    const GLchar **attribs,
    int attribs_length
);
Program *createProgram(
    const char *name,
    const Shader *vert,
    const Shader *frag
);

void destroyProgram(Program *prog);
void destroyShader(Shader *shader);
