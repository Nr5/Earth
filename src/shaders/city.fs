#version 410 core
in vec4 color;

void main(){
	//if(ice>.1){
	//gl_FragColor=color2;
	//return;
	//}
	
	gl_FragColor=color;//*texture3D(u_texture,texcoords )/2;
	
	
}