#version 330 core

layout (location = 0) in vec2 aPos;

uniform float rotate;
uniform vec2 position;
uniform vec2 scale;

out vec2 texCoord;

void main() {
    float cosRot = cos(rotate);
    float sinRot = sin(rotate);
    
    vec2 scaledPos = aPos * scale;
    
    vec2 rotatedPos;
    rotatedPos.x = scaledPos.x * cosRot - scaledPos.y * sinRot;
    rotatedPos.y = scaledPos.x * sinRot + scaledPos.y * cosRot;
    
    vec2 transformedPos = rotatedPos + position;
    
    gl_Position = vec4(transformedPos * 0.5, 0.0, 1.0);
    
    texCoord = aPos;
}