#pragma once

const char debug_frag_shader[] =  \
    "#version 150\n"
    "out vec4 out_color;\n"
    "uniform float phase;\n"
    "void main() {\n"
    "out_color = vec4(sin(phase), 0.0, 1.0, 1.0);\n"
    "}\n";


const char debug_vertex_shader[] = \
    "#version 150\n"
    "in vec3 in_pos;\n"
    "uniform mat4 modelview;\n"
    "void main() {\n"
    "gl_Position = vec4(in_pos.xyz, 1.0) * modelview;\n"
    "}";



const char texture_vertex_shader[] = \
  "#version 150\n"
    "in vec3 in_pos;\n"
    "in vec2 in_texcoord;\n"
    "out vec2 frag_texcoord;\n"
    "uniform mat4 modelview;\n"
    "void main() {\n"
    "    frag_texcoord = in_texcoord;\n"
    "    gl_Position = vec4(in_pos.xyz, 1.0) * modelview;\n"
    "}";


const char texture_frag_shader[] = \
  "#version 150\n"
    "in vec2 frag_texcoord;\n"
    "uniform sampler2D tex;\n"
    "out vec4 out_color;\n"
    "void main() {\n"
    "out_color = texture(tex, frag_texcoord);\n"
    "}\n";



const char color_frag_shader[] =  \
    "#version 150\n"
    "out vec4 out_color;\n"
    "uniform vec4 u_color\n;"
    "void main() {\n"
    "out_color = u_color;\n"
    "}\n";


const char color_vertex_shader[] = \
    "#version 150\n"
    "in vec3 in_pos;\n"
    "uniform mat4 u_modelview;\n"
    "void main() {\n"
    "gl_Position = vec4(in_pos.xyz, 1.0) * u_modelview;\n"
        "gl_Position.z = 0;\n"
    "}";