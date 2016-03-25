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
    "uniform mat4 rotation;\n"
    "void main() {\n"
    "gl_Position = vec4(in_pos.xyz, 1.0) * rotation;\n"
    "}";



const char default_vertex_shader[] = \
  "#version 150\n"
  "in vec3 in_pos;\n"
  "in vec2 in_texcoord;\n"
  "out vec2 Texcoord;\n"
  "uniform mat4 modelview;\n"
  "void main() {\n"
  "    Texcoord = texcoord;\n"
  "    gl_Position = vec4(in_pos.xyz, 1.0) * modelview;\n"
  "}";


const char texture_frag_shader[] = \
  "#version 150\n"
  "in vec2 Texcoord;"
  "out vec4 out_color;\n"
  "void main() {\n"
  "out_color = texture(tex, Texcoord);\n"
  "}\n";
