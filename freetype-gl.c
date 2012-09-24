/* =========================================================================
 * Freetype GL - A C OpenGL Freetype engine
 * Platform:    Any
 * WWW:         http://code.google.com/p/freetype-gl/
 * -------------------------------------------------------------------------
 * Copyright 2011,2012 Nicolas P. Rougier. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY NICOLAS P. ROUGIER ''AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL NICOLAS P. ROUGIER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are
 * those of the authors and should not be interpreted as representing official
 * policies, either expressed or implied, of Nicolas P. Rougier.
 * ========================================================================= */
#include "freetype-gl.h"
#include "vertex-buffer.h"
#include "markup.h"

// ------------------------------------------------------- typedef & struct ---
typedef struct {
    float x, y, z;
    float u, v;
    vec4 color;
} vertex_t;


// ---------------------------------------------------------- texture_font ---

texture_font_t*
new_font(char* name)
{
    texture_atlas_t* atlas = texture_atlas_new( 1024, 1024, 1 );
    texture_font_t *font = texture_font_new( atlas, "fonts/ObelixPro.ttf", 128 );
    return font;
}

void
set_font_outline(texture_font_t* font, int type, float thickness)
{
    font->outline_type = type;
    font->outline_thickness = thickness;
}

void
del_font(texture_font_t* font)
{
  texture_atlas_t* atlas = font->atlas;
  texture_font_delete(font);
  texture_atlas_delete(atlas);
}

// ---------------------------------------------------------- vertex_buffer ---
vertex_buffer_t*
new_buffer(void)
{
  return vertex_buffer_new( "v3f:t2f:c4f" ); 
}

void
del_buffer(vertex_buffer_t* buffer)
{
  return vertex_buffer_delete(buffer);
}

void draw_buffer(vertex_buffer_t* buffer)
{
    vertex_buffer_render( buffer, GL_TRIANGLES, "vtc" );
}

// --------------------------------------------------------------- add_text ---
void make_text( vertex_buffer_t * buffer, texture_font_t * font,
               wchar_t *text, float pen_x, float pen_y, 
               float fg1r, float fg1g, float fg1b, float fg1a,
               float fg2r, float fg2g, float fg2b, float fg2a)

{

    vec2 pen = {{ pen_x, pen_y }};
    vec4 fg_color_1 = {{ fg1r, fg1g, fg1b, fg1a }};
    vec4 fg_color_2 = {{ fg2r, fg2g, fg2b, fg2a }};

    size_t i;
    for( i=0; i<wcslen(text); ++i )
    {

        texture_glyph_t *glyph = texture_font_get_glyph( font, text[i] );

        float kerning = 0;
        if( i > 0)
        {
            kerning = texture_glyph_get_kerning( glyph, text[i-1] );
        }
        pen.x += kerning;


        /* Actual glyph */
        float x0  = ( pen.x + glyph->offset_x );
        float y0  = (int)( pen.y + glyph->offset_y );
        float x1  = ( x0 + glyph->width );
        float y1  = (int)( y0 - glyph->height );
        float s0 = glyph->s0;
        float t0 = glyph->t0;
        float s1 = glyph->s1;
        float t1 = glyph->t1;
        GLuint index = buffer->vertices->size;
        GLuint indices[] = {index, index+1, index+2,
                            index, index+2, index+3};
        vertex_t vertices[] = {
            { (int)x0,y0,0,  s0,t0,  fg_color_1 },
            { (int)x0,y1,0,  s0,t1,  fg_color_2 },
            { (int)x1,y1,0,  s1,t1,  fg_color_2 },
            { (int)x1,y0,0,  s1,t0,  fg_color_1 } };

        vertex_buffer_push_back_indices( buffer, indices, 6 );
        vertex_buffer_push_back_vertices( buffer, vertices, 4 );
        pen.x += glyph->advance_x;

    }
}

