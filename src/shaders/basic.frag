#version 410 core
in vec3 texcoords;
in vec4 color2;
in float ice;
uniform sampler3D u_texture;
uniform float u_Scale;
in float longlen;
void main(){
	//if(ice>.1){
	//gl_FragColor=color2;
	//return;
	//}
	
	gl_FragColor=color2;//*texture3D(u_texture,texcoords )/2;
	
	
}
