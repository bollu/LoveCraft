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
    "uniform float phase;\n"
    "void main() {\n"
    "gl_Position = vec4(in_pos.xy, abs(sin(phase)) * 100, 1.0);\n"
    "}";

