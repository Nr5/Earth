#version 410 core
in vec3 position;
out vec4 color2;
uniform mat4 u_Projection;
uniform mat4 u_Modelview;
uniform vec3 u_Cities[358];
uniform int city_index;
uniform int hover_city_index;
uniform mat4 rmat;
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
	vec3 eucvec=euclidian(vec3(1,u_Cities[gl_InstanceID].yz));
	vec3 y_axis=vec3(0,1,0);
 	vec3 axis =cross(y_axis,eucvec);

	mat4 rotmatrix = 
		u_Projection * rotationMatrix(axis, acos ( dot(y_axis, eucvec)*1.f ) );

	
	gl_Position= rotmatrix * vec4(
					position.x*(.5+ u_Cities[gl_InstanceID].x/10000000.0f),
					position.y,
					position.z*(.5+ u_Cities[gl_InstanceID].x/10000000.0f),
					1) ;

	gl_Position.x=(gl_Position.x-.77)*(1080.0/1920);
	
	if (gl_InstanceID == hover_city_index ){
		color2 = vec4(.7,.7,0,1);
	} else {
		color2 = vec4(1,0,0,1);
	}
}
