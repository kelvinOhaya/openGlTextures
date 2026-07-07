#version 330 core
flat in int vFaceID;
out vec4 FragColor;

void main() {
    // Unpack the integer ID into the Red, Green, and Blue channels
    float r = ((vFaceID >>  0) & 0xFF) / 255.0;
    float g = ((vFaceID >>  8) & 0xFF) / 255.0;
    float b = ((vFaceID >> 16) & 0xFF) / 255.0;
    
    FragColor = vec4(r, g, b, 1.0);
}