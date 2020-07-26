#version 330

in vec3 vertex;
out vec3 texCoord;
uniform mat4 u_Matrix;

void main() {
	gl_Position = u_Matrix * vec4(vertex, 1.0);
	texCoord = vertex;
}