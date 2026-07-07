#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 mvp;
flat out int vFaceID;

void main() {
    gl_Position = mvp * vec4(aPos, 1.0);
    // gl_VertexID / 3 gives us the unique index of the triangle face currently being processed
    vFaceID = gl_VertexID / 3; 
}