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
uniform sampler2D tex1; // main
uniform sampler2D tex2; //temp

struct Camera {
    vec3	cameraPos,
        cameraFront,
        cameraUp,
        cameraRight;
    float fov, dist;
};

uniform int curr_sample;
uniform vec2 resolution;
uniform Camera cam, cam_prev;

out vec4 color;



void main() {
    vec4 cl1 = texture(tex1, tx_cord); // main
    vec4 cl0 = texture(tex2, tx_cord); // temp

  // float pix = (tan(cam.fov / 2.f) * cam.dist * 2) / resolution.x;
  // vec3 ro = cam.cameraPos;
  // vec3 rd = normalize(cam.cameraFront * cam.dist
  //     + vec3((gl_FragCoord.x - resolution.x / 2) * pix) * cam.cameraRight
  //     + vec3((gl_FragCoord.y - resolution.y / 2) * pix) * cam.cameraUp);
  //
  // vec3 worldPosition = ro + cl0.a * rd;
  //
  // vec3 objPos = worldPosition - cam_prev.cameraPos;
  //
  // objPos = vec3(dot( cam_prev.cameraFront, objPos), dot(cam_prev.cameraRight, objPos), dot(cam_prev.cameraUp, objPos));
  //
  // vec2 screen = vec2(objPos.y / (objPos.x * tan(cam.fov / 2.f)), objPos.z / (objPos.x * tan(cam.fov / 2.f) * float(resolution.y) / float(resolution.x)));
  // screen = (screen + 1.0) / 2.f;
  //
  // bvec4 inside = bvec4(screen.x > 0.0, screen.y > 0.0,
  //                 screen.x < 1, screen.y < 1);
  //
  //if (all(inside) && curr_sample > 1 && cl0.a != -1) {
  //    float disocclusion = abs(texture(tex1, screen).a - cl0.a);
  //    if (disocclusion < 0.2f) {
  //        color = vec4(mix(texture(tex1, screen).rgb, cl0.rgb, 0.020f), cl0.a);
  //        //color = vec4(texture(tex1, screen).rgb, cl0.a);
  //        return;
  //    }
  //}

    color = mix(cl1, cl0, 1.f/float(curr_sample));
}
