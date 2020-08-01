#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in float temp;
layout(location = 2) in float minerals;

out vec4  color2;
out vec3  texcoords;
out float ice;
uniform mat4  u_Projection;
uniform mat4  u_ModelView;

uniform vec3  u_Cursor;
uniform vec4  u_Camera;
uniform float u_Csize;
uniform vec4  u_Spectrum[16];

uniform vec3  u_Light;
uniform float u_Scale;
uniform float u_Roughness;
uniform float u_Shine;

uniform vec4 u_explosions[16];

uniform int u_FrameNr;
out float longlen;
const int lo =720;
const int la =360;

float hash(float n) { return fract(sin(n) * 1e4); }
float hash(vec2 p) { return fract(1e4 * sin(17.0 * p.x + p.y * 0.1) * (0.1 + abs(sin(p.y * 13.0 + p.x)))); }

float noise(float x) {
	float i = floor(x);
	float f = fract(x);
	float u = f * f * (3.0 - 2.0 * f);
	return mix(hash(i), hash(i + 1.0), u);
}

float noise(vec2 x) {
	vec2 i = floor(x);
	vec2 f = fract(x);

	// Four corners in 2D of a tile
	float a = hash(i);
	float b = hash(i + vec2(1.0, 0.0));
	float c = hash(i + vec2(0.0, 1.0));
	float d = hash(i + vec2(1.0, 1.0));

	// Simple 2D lerp using smoothstep envelope between the values.
	// return vec3(mix(mix(a, b, smoothstep(0.0, 1.0, f.x)),
	//			mix(c, d, smoothstep(0.0, 1.0, f.x)),
	//			smoothstep(0.0, 1.0, f.y)));

	// Same code, with the clamps in smoothstep and common subexpressions
	// optimized away.
	vec2 u = f * f * (3.0 - 2.0 * f);
	return mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

vec3 euclidian(vec3 polar){
	return vec3(
			polar.x*cos(polar.y)*sin(polar.z),
			polar.x*cos(polar.z),
	
			polar.x*sin(polar.y)*sin(polar.z));
			
}

float explosiongraph[25];

void main(){
	explosiongraph[0] =.01 *3 ;
	explosiongraph[1] =.012*3;
	explosiongraph[2] =.018*3;
	explosiongraph[3] =.03 *3;
	explosiongraph[4] =.05 *3;
	explosiongraph[5] =.03 *3;
	explosiongraph[6] =.025*3;
	explosiongraph[7] =.025*3;
	explosiongraph[8] =.02 *3;
	explosiongraph[9] =.02 *3;
	explosiongraph[10]=.016*3;
	explosiongraph[11]=.015*3;
	explosiongraph[12]=.014*3;
	explosiongraph[13]=.013*3;
	explosiongraph[14]=.012*3;
	explosiongraph[15]=.011*3;
	explosiongraph[16]=.01*3;
	explosiongraph[17]=.009*3;
	explosiongraph[18]=.008*3;
	explosiongraph[19]=.007*3;
	explosiongraph[20]=.005*3;
	explosiongraph[21]=.004*3;
	explosiongraph[22]=.001*3;
	explosiongraph[23]=.0001*3;
	explosiongraph[24]=.00 *3;
			

	vec3 pos=position.xyz;
	
//	if (asin(dot(vec4(euclidian(position.xyz),1),u_Camera)) >3.2/2)return;
	pos.x=.73;
	gl_Position=vec4(euclidian(pos),1);
	gl_Position=u_Projection*gl_Position;
	gl_Position.z+=u_Scale-1;

	ice=0;
	float pi =3.1415;
	float equator=2*pi;
	float longlen = (sin(position.z+pi))*2*pi;
	float ratio = longlen/equator;
	
	color2=vec4(0,0,0,1);
	float x1=.721+(position.x-.721)/30;
	//if (x1>7.22)
	//x1+=noise(position.yz*50)/10/50;
		
	vec3 spec=normalize((normalize(u_Light)+normalize(u_Camera.xyz))/2);


	pos=position.xyz;
	pos.x=.7+(position.x-.7)*u_Shine;
	if (pos.x>1){
	//pos.x=.721+(position.x-.721)/(1+(pos.x-1)*5);
	}
	vec2 wind=vec2(.0004,.0008);
	if(pos.x<=.722){
		//pos.x+=noise((pos.yz+wind*u_FrameNr)*100.0)/50.0;
	}
	gl_Position=vec4(euclidian(pos),1);
	float angle = asin( 
		dot(
			normalize(vec4(-u_Light,1)),
			normalize(gl_Position) 
		)  
	);
	float specAngle=asin( 
		dot(
			normalize(vec4(-spec,1)),
			normalize(gl_Position)
			)  
	);
	
	
	
	
	if(angle<.0000001)angle=.0000001;
	color2.rgb*=vec3(.05/(angle*angle));
	
float specfactor=.6;	
if(position.x<=.721){
	specfactor=1;
	color2.rgb*=vec3(.1/(angle));
}

	pos.x=1;
	gl_Position=vec4(euclidian(pos),1);
	

  vec3 specularReflection=vec3(1);
  //if (dot(gl_Position.xyz, u_Light) < 0.0) // light source on the wrong side?
  //  {
  //    specularReflection = vec3(0.0, 0.0, 0.0); // no specular reflection
  //  }
  //else
    {
      specularReflection = vec3(specfactor*5)
	* pow(max(0.0, dot(reflect(-u_Light, gl_Position.xyz), u_Camera.xyz)),
	      8);
    }
    color2.rgb += specularReflection*vec3(.8,.8,1);
	for (int i=0; i< 16;i++){	
		float d = length(euclidian(vec3(.721,position.yz)) - u_explosions[i].xyz);
		
		float angle = asin( 
			dot(
				normalize(vec4(-u_explosions[i].xyz,1)),
				normalize(gl_Position)
			)  
		);
    	
		//color2.rgb *= max(1,explosiongraph[int(u_explosions[i].a)]/2/(angle));

		
		color2.rgb +=
						explosiongraph[int(u_explosions[i].a)] 
						/ pow(d,1.5)
						*vec3(1,.7,.7) / 50;
		




	}

	//if (color2.r+color2.g+color2.b>2)color2=vec4(1,1,1,1);
	//else color2=vec4(0,0,0,1);
    //gl_Position.z+=u_Scale-1;
	color2 = vec4(.8,.8,.8,position.x)

	gl_Position=vec4(euclidian(vec3(1,position.yz)),1);
	gl_Position=u_Projection*gl_Position;


	gl_Position=u_ModelView*gl_Position;
	gl_Position.x=(gl_Position.x-.77)*(1080.0/1920);
}
