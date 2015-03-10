/******************************************************************************\
* Project:  Graphics Library Interface Run-Time Configuration                  *
* Authors:  Iconoclast                                                         *
* Release:  2014.12.19                                                         *
* License:  CC0 Public Domain Dedication                                       *
*                                                                              *
* To the extent possible under law, the author(s) have dedicated all copyright *
* and related and neighboring rights to this software to the public domain     *
* worldwide. This software is distributed without any warranty.                *
*                                                                              *
* You should have received a copy of the CC0 Public Domain Dedication along    *
* with this software.                                                          *
* If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.             *
\******************************************************************************/

#include <stdio.h>
#include "state.h"
#include "image.h"

const char* GL_errors[8] = {
    "GL_NO_ERROR",
    "GL_INVALID_ENUM",
    "GL_INVALID_VALUE",
    "GL_INVALID_OPERATION",
    "GL_STACK_OVERFLOW",
    "GL_STACK_UNDERFLOW",
    "GL_OUT_OF_MEMORY",

    "GL_UNKNOWN_ERROR"
};

GLboolean channels[4];
GLint viewport[4];
GLubyte* pixels;

GLfloat flip_sign = -1.f;
GLboolean BGR_ordering = GL_FALSE;
GLubyte bits_per_pixel = 8;

GLubyte * file_data;
GLint byte_offset;
long file_size;

GLint texture_limit = 64;

void DisplayGLError(char * text, GLenum status)
{
    signed int index;

    index = (signed int)status;
    if (status != GL_NO_ERROR)
        index = index - GL_INVALID_ENUM + 1;
    if (index < 0 || index > 7)
        index = 7;

    fprintf(stderr, "%s\n%s\n\n", GL_errors[index], text);
    return;
}

void key_press(int key, int x, int y)
{
    FILE * stream;
    const GLint bits = (GLint)(-flip_sign) * bits_per_pixel;

    switch (key)
    {
    case GLUT_KEY_F3:
        glReadPixels(
            0,
            0,
            viewport[2],
            viewport[3],
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            pixels
        );
        stream = fopen("r8g8b8a8.data", "wb");
        fwrite(pixels, 4 * viewport[2], viewport[3], stream);
        fclose(stream);
        break;
    case GLUT_KEY_F5:
        glFinish();
        printf(
            "%u x %u viewport at offset %08X\n",
            viewport[2], viewport[3], byte_offset
        );
        break;
    case GLUT_KEY_LEFT:
        --byte_offset;
        break;
    case GLUT_KEY_RIGHT:
        ++byte_offset;
        break;
    case GLUT_KEY_UP:
        byte_offset -= viewport[2] * bits/8;
        break;
    case GLUT_KEY_DOWN:
        byte_offset += viewport[2] * bits/8;
        break;
    case GLUT_KEY_PAGE_UP:
        byte_offset -= viewport[2]*viewport[3] * bits/8;
        break;
    case GLUT_KEY_PAGE_DOWN:
        byte_offset += viewport[2]*viewport[3] * bits/8;
        break;
    }
    glutPostRedisplay();
    return;
}

void key_press_ascii(unsigned char key, int x, int y)
{
    GLenum error;

    switch (key)
    {
    case '0': /* 1-bit monochrome (black and white) */
    case '1': /* 2-bit CGA chrome (black, white, gray and "silver") */
    case '2': /* 4-bit R1G1B1 */
    case '3': /* 8-bit R3G3B2 or L8 */
    case '4': /* 16-bit R5G5B5, R5G6B5, or R4G4B4 */
    case '5': /* 32-bit color with 4 8-bit components */
    case '6': /* 64-bit color with 4 16-bit components */
    case '7': /* 128-bit color with 4 32-bit components */
        bits_per_pixel = 1 << (key - '0');
        break;
    case '-':
        reshape(viewport[2] -= 1, viewport[3]);
        break;
    case '+':
        reshape(viewport[2] += 1, viewport[3]);
        break;
    case 'A':
        glDisable(GL_BLEND);
        glGetBooleanv(GL_COLOR_WRITEMASK, channels);
        channels[CVG] = GL_FALSE;
        break;
    case 'B':
        glGetBooleanv(GL_COLOR_WRITEMASK, channels);
        channels[BLU] = GL_FALSE;
        break;
    case 'G':
        glGetBooleanv(GL_COLOR_WRITEMASK, channels);
        channels[GRN] = GL_FALSE;
        break;
    case 'R':
        glGetBooleanv(GL_COLOR_WRITEMASK, channels);
        channels[RED] = GL_FALSE;
        break;
    case 'E':
        BGR_ordering = GL_FALSE;
        break;
    case 'F':
        flip_sign = -1.f;
        texture_vector[1][1] = texture_vector[0][1] = -1.f;
        texture_vector[3][1] = texture_vector[2][1] = +1.f;
        break;
    case 'a':
        glEnable(GL_BLEND);
        glGetBooleanv(GL_COLOR_WRITEMASK, channels);
        channels[CVG] = GL_TRUE;
        break;
    case 'b':
        glGetBooleanv(GL_COLOR_WRITEMASK, channels);
        channels[BLU] = GL_TRUE;
        break;
    case 'g':
        glGetBooleanv(GL_COLOR_WRITEMASK, channels);
        channels[GRN] = GL_TRUE;
        break;
    case 'r':
        glGetBooleanv(GL_COLOR_WRITEMASK, channels);
        channels[RED] = GL_TRUE;
        break;
    case 'e':
        BGR_ordering = GL_TRUE;
        break;
    case 'f':
        flip_sign = +1.f;
        texture_vector[1][1] = texture_vector[0][1] = +1.f;
        texture_vector[3][1] = texture_vector[2][1] = -1.f;
        break;
    }

    glClear(GL_COLOR_BUFFER_BIT);
    glColorMask(channels[RED], channels[GRN], channels[BLU], channels[CVG]);

    error = glGetError();
    if (error != GL_NO_ERROR)
        printf("OpenGL error.\n");
    glutPostRedisplay();
    return;
}