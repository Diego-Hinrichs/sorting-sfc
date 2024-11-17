#version 420 core

in vec4 vCol;
out vec4 FragColor;

void main() {
    vec2 coord = gl_PointCoord * 2.0 - 1.0;
    float dist = dot(coord, coord);
    if (dist > 1.0) {
        discard;
    }
    float alpha = smoothstep(1.0, 0.8, dist);
    FragColor = vec4(vCol.rgb, alpha);
}
