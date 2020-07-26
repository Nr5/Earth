#version 410 core 

in float color;
uniform vec4 u_Light;
uniform sampler2D tex;
uniform vec3 u_Cursor;
uniform int u_Height;
uniform float u_Csize;
uniform vec2 u_Dim;
const int lo = 180;
const int la = 90;



vec3 euclidian(vec3 polar){
	return vec3(
			polar.x*cos(polar.y)*sin(polar.z),
			polar.x*cos(polar.z),
	
			polar.x*sin(polar.y)*sin(polar.z));
			
}


void main(){
	vec4 color=texture2D(tex, vec2(gl_FragCoord.x/u_Dim.x,gl_FragCoord.y/u_Dim.y));
	float heat=color.a;
	float angle = dot(normalize(u_Light),vec4(normalize(euclidian(color.xyz)),1));
	if(angle<1)heat+=(1/(angle*angle))/150000.0;

	heat*=.99999;
	//if (color.x>.74){
	//	heat*=1-(color.x-.74)/10   ;
	//}
	
	//gl_FragColor.a=heat;
	//gl_FragColor=texture2D(tex, vec2(gl_FragCoord.x/u_Dim.x,gl_FragCoord.y/u_Dim.y));
	
	float d=distance(euclidian( vec3(.7,color.yz) ),u_Cursor );
	if (d<u_Csize){
		if (d<.02)d=.02;
		color.x+=.00001*u_Height/d;
		if (color.x<.7)color.x=.7;
		//heat=1;
	}
	
	
	gl_FragColor=vec4(color.xyz,heat);
	//gl_FragColor=vec4(u_Cursor,heat);
	
}
