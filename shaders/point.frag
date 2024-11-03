#version 420 core

in vec4 vCol;
out vec4 FragColor;

void main()
{
    vec2 coord = gl_PointCoord * 2.0 - 1.0;  // cnvertir el rango a [-1, 1] centrado
    
    float dist = dot(coord, coord);
    
    if (dist > 1.0) {
        discard;  // Descartar fragmento para crear un círculo
    }
    
    // Crear un borde suavizado alrededor del círculo
    float alpha = smoothstep(1.0, 0.8, dist);

    // Aplicar el suavizado en el canal alfa
    FragColor = vec4(vCol.rgb, alpha * vCol.a);
}
