~~vertex~~
#version 430 core
layout(location = 0) in vec2 position;
layout(location = 1) in vec2 tex_coord;

out vec2 tx_cord;

void main() {
    gl_Position = vec4(position.x, position.y, 0.0, 1.0);
    tx_cord = tex_coord;
}


~~fragment~~
#version 430 core

in vec2 tx_cord;
uniform sampler2D tex1;
uniform sampler2D tex2; //temp

uniform int curr_sample;

out vec4 color;

void main() {

    vec4 cl1 = texture(tex1, tx_cord); // main
    vec4 cl0 = texture(tex2, tx_cord); // buf1

    float blend = (cl1.a == 0.0f || curr_sample == 1) ? 1.0f : 1.0f / (1.0f + (1.0f / cl1.a));
    color = vec4(mix(cl1.rgb, cl0.rgb, blend), blend);
}
