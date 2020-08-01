#version 410 core
in vec3 position;
out vec4 color2;
uniform mat4 u_Projection;
uniform mat4 u_Modelview;
uniform vec4 u_explosions[16];
uniform int city_index;
uniform int hover_city_index;
uniform mat4 rmat;
float explosiongraph[25];
mat4 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    
    return mat4(
			oc * axis.x * axis.x + c,  
			oc * axis.x * axis.y + axis.z * s,  
            oc * axis.z * axis.x - axis.y * s,  
			0.0,
			
			
			oc * axis.x * axis.y - axis.z * s, 
			oc * axis.y * axis.y + c, 
			oc * axis.y * axis.z + axis.x * s, 
			0.0,
			
			oc * axis.z * axis.x + axis.y * s,  
			oc * axis.y * axis.z - axis.x * s,  
			oc * axis.z * axis.z + c,  
			0.0,
			
			0.0,
			0.0,
			0.0,
			1.0);
}

vec3 euclidian(vec3 polar){
	return vec3(
			polar.x*cos(polar.y)*sin(polar.z),
			polar.x*cos(polar.z),
			polar.x*sin(polar.y)*sin(polar.z));
			
}

void main(){
	explosiongraph[0] =.01 *3 ;
	explosiongraph[1] =.012*3;
	explosiongraph[2] =.018*3;
	explosiongraph[3] =.03 *3;
	explosiongraph[4] =.03 *3;
	explosiongraph[5] =.03 *3;
	explosiongraph[6] =.03*3;
	explosiongraph[7] =.03*3;
	explosiongraph[8] =.04 *3;
	explosiongraph[9] =.04 *3;
	explosiongraph[10]=.04*3;
	explosiongraph[11]=.045*3;
	explosiongraph[12]=.045*3;
	explosiongraph[13]=.043*3;
	explosiongraph[14]=.042*3;
	explosiongraph[15]=.041*3;
	explosiongraph[16]=.04*3;
	explosiongraph[17]=.000*3;
	explosiongraph[18]=.000*3;
	explosiongraph[19]=.000*3;
	explosiongraph[20]=.000*3;
	explosiongraph[21]=.000*3;
	explosiongraph[22]=.000*3;
	explosiongraph[23]=.0000*3;
	explosiongraph[24]=.00 *3;
	vec3 y_axis=vec3(0,1,0);
 	vec3 axis =cross(y_axis,normalize(u_explosions[gl_InstanceID].xyz));

	mat4 rotmatrix = 
		u_Projection * rotationMatrix(axis, acos ( dot(y_axis, normalize(u_explosions[gl_InstanceID].xyz))*1.f ) );

	
	gl_Position= rotmatrix * vec4(
					position.x*( explosiongraph[int(u_explosions[gl_InstanceID].a)]/.075f),
					.71+ (position.y-.71)*( explosiongraph[int(u_explosions[gl_InstanceID].a)]/.075f),
					position.z*( explosiongraph[int(u_explosions[gl_InstanceID].a)]/.075f),
					1) ;

	gl_Position.x=(gl_Position.x-.77)*(1080.0/1920);
 	color2=	vec4(.8, .6, .4, .6);
}
