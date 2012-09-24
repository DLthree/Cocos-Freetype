#!/usr/bin/python

import pyglet
from pyglet.gl import *
import cocos
import ctypes
from ctypes import c_void_p, c_char_p, c_wchar_p, c_float, c_int

freetype_gl = ctypes.cdll.LoadLibrary("libfreetype-gl.dylib")


new_font = freetype_gl.new_font
new_font.restype = c_void_p
new_font.argstype = [ c_char_p ]

del_font = freetype_gl.del_font
del_font.restype = None
del_font.argstype = [ c_void_p ]

new_buffer = freetype_gl.new_buffer
new_buffer.restype = c_void_p
new_buffer.argstype = []

del_buffer = freetype_gl.del_buffer
del_buffer.restype = None
del_buffer.argstype = [ c_void_p ]

draw_buffer = freetype_gl.draw_buffer
draw_buffer.restype = None
draw_buffer.argstype = [ c_void_p ]


make_text = freetype_gl.make_text
make_text.restype = None
make_text.argstype = [ c_void_p, c_void_p, c_wchar_p,
                       c_float, c_float,
                       c_float, c_float, c_float, c_float,
                       c_float, c_float, c_float, c_float, ]

set_font_outline = freetype_gl.set_font_outline
set_font_outline.restype = None
set_font_outline.argstype = [ c_void_p, c_int, c_float ]

class Font(object):
    def __init__(self, name):
        self.font = None
        self.font = c_void_p(new_font(name))

    def __del__(self):
        if self.font:
            del_font(self.font)
            self.font = None

    def set_outline(self, type, thick):
        set_font_outline(self.font, c_int(type), c_float(thick))

class Buffer(object):
    def __init__(self):
        self.buffer = None
        self.buffer = c_void_p(new_buffer())

    def __del__(self):
        if self.buffer:
            del_buffer(self.buffer)
            self.buffer = None

    def draw(self):
        draw_buffer(self.buffer)

    def add_text(self, font, text, pen, fg1, fg2):
        wtext = unicode(text)
        args = map(c_float, pen + fg1 + fg2)
        make_text(self.buffer, font.font, wtext, *args)
                  

class CocosFreetype(cocos.cocosnode.CocosNode):
    def __init__(self, font, text):
        super(CocosFreetype, self).__init__()
        self.buffer = Buffer()

        pen = (30, 40)
        black  = (0.0, 0.0, 0.0, 1.0)
        yellow = (1.0, 1.0, 0.0, 1.0)
        orange1 = (1.0, 0.9, 0.0, 1.0)
        orange2 = (1.0, 0.6, 0.0, 1.0)
        
        font.set_outline(2, 7)
        self.buffer.add_text(font, text, pen, black, black)

        font.set_outline(2, 5)
        self.buffer.add_text(font, text, pen, yellow, yellow)

        font.set_outline(1, 3)
        self.buffer.add_text(font, text, pen, black, black)

        font.set_outline(0, 0)
        self.buffer.add_text(font, text, pen, orange1, orange2)

    def draw(self, *args, **kwargs):
        glPushMatrix()

        self.transform()

        glEnable( GL_TEXTURE_2D )
        self.buffer.draw()
        glDisable(GL_TEXTURE_2D)

        glPopMatrix()



if __name__ == "__main__":

    class Scene(cocos.scene.Scene):
        def __init__(self):
            super(Scene, self).__init__()
            font = Font("fonts/ObelixPro.ttf")
            label = CocosFreetype(font, "Hello World!")
            self.add(label)
    
    from cocos.director import director

    director.init( 640, 480,
                   do_not_scale=True, resizable=False,
                   fullscreen=False, caption="freetype-gl.py")
    director.window.set_mouse_visible(False)

    director.run(Scene())
