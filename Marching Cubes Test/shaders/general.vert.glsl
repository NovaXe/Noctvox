#version 330

vec3 vertex_pos;

void main(){
	gl_Position = vec4( vertex_pos, 1);

}
