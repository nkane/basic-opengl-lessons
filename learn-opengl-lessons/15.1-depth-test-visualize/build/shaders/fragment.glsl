#version 330 core
out vec4 frag_color;

in vec2 texture_coordinates;

uniform sampler2D texture_1;

float near = 0.1;
float far = 100.0;

float linearize_depth(float depth)
{
    // convert back to normalized device coordinates
    float z = depth * 2.0 - 1.0;
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main()
{
    // visualize regular depth testing set up
    //frag_color = vec4(vec3(gl_FragCoord.z), 1.0);
    // divide by z for demonstration
    float depth = linearize_depth(gl_FragCoord.z) / far;
    frag_color = vec4(vec3(depth), 1.0);
}
