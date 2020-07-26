#version 330 core
layout(triangles) in;
layout(triangle_strip,max_vertices = 3) out;
in positions[];

void main(){
	gl_Color=vec4(1,0,0,1);
	//gl_Position=vec4(euclidian(positions.xyz),1);
	//gl_Position=u_Projection*gl_Position;
	gl_Position=vec4(1,0,0,1);
	EmitVertex();
	
	gl_Color=vec4(1,0,0,1);
	//gl_Position=vec4(euclidian(positions.xyz),1);
	//gl_Position=u_Projection*gl_Position;
	gl_Position=vec4(1,1,0,1);
	EmitVertex();
	
	gl_Color=vec4(1,0,0,1);
	//gl_Position=vec4(euclidian(positions.xyz),1);
	//gl_Position=u_Projection*gl_Position;
	gl_Position=vec4(0,1,0,1);
	
	EmitVertex();
	
	EndPrimitive();
	
}
