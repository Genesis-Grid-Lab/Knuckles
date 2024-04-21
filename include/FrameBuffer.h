#ifndef FRAME_H_
#define FRAME_H_
#include "GL/glew.h"

void create_framebuffer();
void bind_framebuffer();
void unbind_framebuffer();
void rescale_framebuffer(float width, float height);
GLuint &GetTextureId();
GLuint &GetFBO();

#endif // !FRAME_H_
