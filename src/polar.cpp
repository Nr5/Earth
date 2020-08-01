/*
 * shaders.cpp

 *
 *  Created on: Nov 28, 2017
 *      Author: Nr5
 */
//#define GLM_FORCE_RADIANS

#define GLM_ENABLE_EXPERIMENTAL
#include <iostream>
#include <ostream>
#include <fstream>
#include <string>
#include <sstream>

#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include <ctime>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/noise.hpp>
#include "misc.h"
#include "../../Engine2/src/Skybox.h"
#include "../../Engine2/src/ui/UI.h"

#define PI 3.1415
bool running=1;
int u_model;
int u_projection;
int u_cursor;
int u_spectrum;
int u_alpha;
int u_light ;
int u_camera ;
int u_height;
int u_cursor2;
int u_light2 ;
int u_csize;
int u_csize2;
int u_framenr;

using glm::vec2;
using glm::vec3;
using glm::vec4;

using glm::mat4;
unsigned int city_count;
float* cities;
char*  citynamebuffer;
char** citynames;
vec2  cityname_dims;

uint32_t city_buffer;
uint32_t explosion_buffer;

int hovercity=-1;
unsigned int cityname_texture;
//unsigned int text_texture;
int texture_width;
int texture_height;
short screenshot[61+1920*1080*2] = {
		19778,4218,39,0,0,122,0,108,0,
		1920,0,1080,0,1,32,
		3,0,4096,39,0,0,0,0,0,0,0,0,0,255,-256,0,255,0,0,-256,28192,22377,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10148,-6400
		};

typedef struct rocket{
	uint8_t length;
	float   velocity;
	float   angle;
	float   end_angle;
	float   height;
	vec3 axis;
	vec3 startpos;
		
}rocket;

rocket nukes[16];
vec3 nuke_paths[16][32];


vec4   explosions[16];

int* pixelbuffer=(int*)(screenshot+61);
GLfloat model[16];
mat4 projection(1);
mat4 modelview(1);

float rotx;
float roty;
TTF_Font* font;
#define lo 720
#define la 360
#define VERTEX_COUNT (lo*(la-2)+2)
#define VERTEX_SIZE 5
#define BUFFER_SIZE VERTEX_COUNT*VERTEX_SIZE
//#define TRIANGLE_COUNT ((VERTEX_COUNT-1)*lo)*6
#define TRIANGLE_COUNT (la-2)*lo*6 +lo *6
float *vertices;

unsigned int indices1[TRIANGLE_COUNT];
unsigned int indices2[TRIANGLE_COUNT];

#define WIDTH 1920
#define HEIGHT 1080

int heatshader;
unsigned int framenr;
float heightmap[BUFFER_SIZE]={};

bool rotating=false;
int lastx,lasty;
double mx,my;
float scale=0;
bool mousedown;
GLuint renderBuffer;
GLuint frameBuffer;
GLuint depthRenderBuffer;
GLuint tex;
char spectrumcount;

float spectra[16*4*16];
float spectrum[16*4];

unsigned char colors[8*4]{
	 181,  0,   0,   255,
	 183,  0,   51,  25,
	 183,  0,   153, 127,
	 183,  0,   127, 76,
	 184,  102, 102, 76,
	 185,  127, 127, 102,
	 187,  127, 127, 127,
	 191,  127, 127, 127
};

ColorPicker<float> colorpickers[16];
Slider<float> sliders[16];
Panel spectrumpanels[16];
Panel specselectpanels[16];
Panel cspanel(1920-370-100+2, 320, 100, 500, 0);

char dir;
float cursorsize=.1;
uint8_t drawcities=0;
glm::vec3 cursor;
glm::vec3 cursorp;
GLuint grass;
float* fcursor = (float*) &cursor;
glm::vec3 camera(0,0,-.7);
glm::vec3 lastcursor(.7,0,0);
glm::vec3 rotaxis(0,1,0);
glm::vec3 light(.8,0,-.6);
glm::vec3 orbitaxis(0,1,0);
glm::mat4 sky_Projection = glm::perspective(45.0f, (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
glm::mat4 sky_View       = glm::mat4(50.0f);
glm::mat4 sky_Model      = glm::scale(glm::mat4(1.0f),glm::vec3(50,50,50));
glm::mat4 sky_Matrix     = glm::mat4(1.0f);
float alpha=0;
float beta=0;
unsigned int buffer1;
unsigned int buffer2;

int cityshader;
int explosionshader;
float dist=0;

float roughness  = 0.f;
float shine		 = 0.f;

uint8_t pathcount;

vec3 mesh_dome[40*3];

vec4 mesh_city[(3+1+1)]={
			vec4( 			 0 , .72f, 0, 			  1 ) * 1.01f,
			vec4(cos(0*2*PI/3) , .72f, sin(0*2*PI/3), 1 ) *  .99f,
			vec4(cos(1*2*PI/3) , .72f, sin(1*2*PI/3), 1 ) *  .99f,
			vec4(cos(2*2*PI/3) , .72f, sin(2*2*PI/3), 1 ) *  .99f,
			vec4(cos(0*2*PI/3) , .72f, sin(0*2*PI/3), 1 ) *  .99f
		};

bool close (Window* win){
	std::cout << "close\n";
	win->running=0;
	return 1;
}

glm::vec3 euclidian(glm::vec3 polar){
	return glm::vec3(
			polar.x * cos(polar.y) * sin(polar.z),
			polar.x * cos(polar.z),
			polar.x * sin(polar.y) * sin(polar.z)
			);
}

static float distance(glm::vec3 p1,glm::vec3 p2){
	glm::vec3 diff=p2-p1;
	return sqrt(diff.x*diff.x+diff.y*diff.y+diff.z*diff.z);
}

static void heighten(char dir){
	int cursory=int(cursorp[2]/(PI)*la);
	int cursorx=int(cursorp[1]/(PI)*la);

	int x1=cursory-20;
	if (x1 <0)x1=0;

	int x2=cursory+20;
	if (x2 >lo)x2=lo;

	int y1=cursory-20;
	if (y1 <0)y1=0;

	int y2=cursory+20;
	if (y2 >la)y2=la;

	for (int x=0;x<lo;x++)
		for (int y=y1;y<y2;y++){
		float* vertex=vertices+(x+y*lo)*VERTEX_SIZE;
		float d=distance(euclidian(glm::vec3{.7,vertex[1],vertex[2]} ),cursor );
		if (d<.1){
			if (d<.02)d=.02;
			*vertex+=.00001*dir/d;
			if (*vertex<.7)*vertex=.7;
			//vertex[3]=1.0;

		}
	}
}

static void load_mesh(){
	FILE* f = fopen("../res/dome.stl","r");
	uint32_t size;
	
	fseek(f, 80, SEEK_CUR);
	fread(&size, 1, 4, f);
	
	for(uint32_t i=0;i<size;i++){
		fseek(f, 12, SEEK_CUR);
		fread(mesh_dome + i*3, 3,  sizeof(vec3),f);
		fseek(f, 2,  SEEK_CUR);
	}	
}

static void init(){
	  load_mesh();
	  std::ifstream file ("../res/cities",std::ios_base::in);
	  int citynames_size;
	  if (file.is_open())
	  {
		file.read ((char*)&city_count, 4);
		std::cout <<city_count<<" cities\n";

		file.read ((char*)&citynames_size, 4);
		std::cout <<city_count<<" cities\n";

		cities=new float [ city_count*3 ];
		/*
		((vec4*)cities)[0] = vec4( 			  0 , .72f, 0,			   1 ) * 1.01f;
		((vec4*)cities)[1] = vec4(cos(0*2*PI/3) , .72f, sin(0*2*PI/3), 1 ) *  .99f;
		((vec4*)cities)[2] = vec4(cos(1*2*PI/3) , .72f, sin(1*2*PI/3), 1 ) *  .99f;
		((vec4*)cities)[3] = vec4(cos(2*2*PI/3) , .72f, sin(2*2*PI/3), 1 ) *  .99f;
	    */
		std::cout <<"0\n";

		file.read (((char*)cities) , city_count*4*3);
		std::cout <<"1\n";

		citynamebuffer=new char[citynames_size];
		std::cout <<"textbufferlength:"<<citynames_size<<"\n";

		file.read (citynamebuffer,citynames_size);
		std::cout <<"3\n";

	    file.close();
	  }
	  citynames=new char*[city_count];
	  citynames[0]=citynamebuffer;
	  int i=1;
	  for (char*p=citynamebuffer;i<city_count;p++){
		  if(!(*p)){
			  citynames[i]=p+1;
			  i++;
		  }
	  }
/*
	  for(char* name:citynames){
		  std::cout<<name<<"\n";
	  }
*/

	std::ifstream spectrumfile ("./spectrum",std::ios_base::in);
	if (spectrumfile.is_open()) {
		spectrumfile.read(&spectrumcount,1);
		spectrumfile.read ((char*)spectra, 16*4*sizeof(float)*spectrumcount);
		spectrumfile.close();
		std::memcpy(spectrum, spectra,16*4*sizeof(float));

		std::cout << "spectrum loaded\n";
		cspanel.height=spectrumcount*50;
	}

	glGenRenderbuffers( 1, &renderBuffer );
	glBindRenderbuffer( GL_RENDERBUFFER, renderBuffer );
	glRenderbufferStorage( GL_RENDERBUFFER, GL_RGBA32F, lo, la );

	glGenFramebuffers( 1, &frameBuffer );
	glBindFramebuffer( GL_FRAMEBUFFER, frameBuffer );
	glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderBuffer);
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );

	for(int i=0;i<16;i++){
		nukes[i].startpos = euclidian(vec3(.721,cities[i*8*3+1],cities[i*8*3+2] ));
		vec3 endpos = euclidian(vec3(.721,cities[(city_count-(i*8)-1)*3+1],cities[(city_count-(i*8)-1)*3+2] ));
		
		explosions[i]=vec4(endpos,24);

		nukes[i].axis =cross( nukes[i].startpos,endpos );
		float angle=glm::acos ( glm::dot(glm::normalize(nukes[i].startpos), glm::normalize(endpos)) );
	
		//glColor4f(1.f,1.f,1.f,1.f);
		//glLineWidth(1);
    	//glBegin(GL_LINES);
	
		nukes[i].height 	= 0;
		nukes[i].velocity 	= 49.f*angle/2;
		nukes[i].angle 		= 0;
		nukes[i].end_angle  = angle;	
		nukes[i].length 	= 8;
	}


	std::cout<<"text loaded\n";
}

static void act(Panel *panel){
	glm::mat4 rotationMat = glm::mat4(1);
	if (rotating){
		rotationMat = glm::rotate(glm::mat4(1), .01f, vec3(0,1,0));
		projection=glm::rotate(projection,-.01f,vec3(0,1,0));
		light = rotationMat*glm::vec4(light,1);
		orbitaxis= glm::vec3(glm::mat4(1)/rotationMat*glm::vec4(orbitaxis,1));


		//glm::mat4 RotateX = glm::rotate(glm::mat4(1.0f), alpha, glm::vec3(-1.0f, 0.0f, 0.0f));


	}
	camera = euclidian(glm::vec3(.7,2*PI-acos(0/ sqrt(.7*.7-0) ),asin(0/.7)+PI/2));
	camera = glm::vec3(glm::mat4(1)/projection*glm::vec4(camera,1));

	

}

static bool mousemove(Panel *panel,SDL_Event event,int x,int y,int lastx,int lasty){

	mx=(((x)/540.0f)-1)/scale;
	my=(((y)/540.0f)-1)/scale;

	float l = sqrt(mx*mx+my*my);
	if (l>.69){
		mx=mx*.69/l;
		my=my*.69/l;
	}

	cursorp=glm::vec3(.72,2*PI-acos(mx/ sqrt(.72*.72-my*my) ),asin(my/.72)+PI/2);

	cursor = euclidian(glm::vec3(.72,2*PI-acos(mx/ sqrt(.72*.72-my*my) ),asin(my/.72)+PI/2));
				//camera = euclidian(glm::vec3(.7,2*PI-acos(0/ sqrt(.7*.7-0) ),asin(0/.7)+PI/2));
				//cursor = glm::vec3(glm::mat4(1)/projection*glm::vec4(cursor,1));

	lastcursor=cursor;
				//cursorp = vec3(mat4()/projection*vec4(cursorp,1));


				//camera = glm::vec3(glm::mat4(1)/projection*glm::vec4(camera,1));

		for(int i=0;i<city_count&&drawcities;i++){
			vec3 v=*((vec3*)(cities+i*3));
			//vec3 v=vec3(1,i*2*PI/7,0.1);
			vec3 eucvec=projection*(vec4(euclidian(vec3(.72,v.y,v.z)),1));

			bool hover= glm::length(vec2(cursor)-vec2(eucvec))<.02;
            if (hover && hovercity != i){
			
            	const char* name =(char*)citynames[i];
                if (!*name)name="No Name";
std::cout<<UI::fonts["a"] <<"\n";
				std::cout << i << " " << name << "\n" ;
                SDL_Surface* surface = TTF_RenderUTF8_Blended(UI::fonts["a"], name,SDL_Color {255,255,255}); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first
            	std::cout <<surface <<"\n";

            	glDeleteTextures(1,&cityname_texture);
            	glGenTextures(1, &cityname_texture);
            	glBindTexture(GL_TEXTURE_2D, cityname_texture);
            	glTexImage2D(GL_TEXTURE_2D, 0, 4, surface->w, surface->h, 0,
            									GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

            	cityname_dims= vec2{surface->w, surface->h};
            	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
            	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

            	SDL_FreeSurface(surface);
            	hovercity=i;
            }
            if (hovercity==i && !hover){
            	hovercity=-1;
            }
		}
		return true;
}

static bool mousedrag(Panel *panel,SDL_Event event,int x,int y,int lastx,int lasty){
	mx=(x/540.f -1)/scale;
	my=((y)/540.f -1)/scale;

	float l = sqrt(mx*mx+my*my);
		if (l>.69){
			mx=mx*.69/l;
			my=my*.69/l;
	}
	cursorp=glm::vec3(.7,2*PI-acos(mx/ sqrt(.7*.7-my*my) ),asin(my/.7)+PI/2);

	cursor = euclidian(glm::vec3(.7,2*PI-acos(mx/ sqrt(.7*.7-my*my) ),asin(my/.7)+PI/2));
	camera = euclidian(glm::vec3(.7,2*PI-acos(0/ sqrt(.7*.7-0) ),asin(0/.7)+PI/2));

	if ((SDL_GetModState() & KMOD_LCTRL)){
		glm::vec3 ncursor = glm::normalize(cursor);
		glm::vec3 nlast = glm::normalize(lastcursor);

		glm::vec3 axis =glm::cross(nlast,ncursor);
		axis = glm::vec3(glm::mat4(1)/projection*glm::vec4(axis,1));
		if (axis   != vec3(0,0,0) 					&&
			nlast  != vec3(0,0,0) 					&&
			ncursor!= vec3(0,0,0) 					&&
			!isnan(acos(glm::dot(ncursor,nlast))) 	&& 
			ncursor != nlast)
		{
			projection = glm::rotate(projection,(float)(acos(glm::dot(ncursor,nlast))), axis);
			axis =glm::cross(nlast,ncursor);
			glm::mat4 RotateY = glm::rotate(glm::mat4(1.0f),  -(float)(acos(glm::dot(ncursor,nlast))), axis);

			//sky_Projection = glm::rotate(sky_Projection,(float)(acos(glm::dot(ncursor,nlast))), axis);
			sky_Matrix = sky_Projection * (sky_View *= RotateY);
			//beta-=.01;
		}
	}
				//cursor = glm::vec3(glm::mat4(1)/projection*glm::vec4(cursor,1));

	lastcursor=cursor;
				//cursorp = vec3(mat4()/projection*vec4(cursorp,1));


	camera = glm::vec3(glm::mat4(1)/projection*glm::vec4(camera,1));
	return true;
}

static bool key(Panel *panel,SDL_Event event){
	switch(event.key.keysym.sym){
			case SDLK_ESCAPE:
				panel->running=false;
				break;
			case SDLK_q:
				projection = glm::rotate(projection,1.f, vec3(1,0,0));
				break;
			case SDLK_SPACE:
				std::cout<<"space\n";
				rotating=!rotating;
				break;
			case SDLK_PLUS:
				scale+=.1;
				break;
			case SDLK_MINUS:
				scale-=.1;
				break;
			case SDLK_UP:
				dist-=.01;
			break;
			case SDLK_DOWN:
				dist+=.1;
				break;


	}
	return true;
}

void draw(Panel* panel){
	glUseProgram(0);

	
	glScalef(1080.f/1920.f,1,1);
	glTranslatef(-.77f,0,0);
	//glDisable(GL_DEPTH_TEST);
	glBegin(GL_TRIANGLES);
	float col=0.f;	
		
	/*for (int i=0;i<120;i++){
		glColor3f(col,col,col);
		glVertex3fv((float*) (mesh_dome+i) );
		col+=1.f/120;
	}*/
	
	glEnd();
	for (int i=0;i<16;i++){
		if (explosions[i].a<24)continue;
		if (nukes[i].height < 0) {
			memset(nuke_paths[i],0,32*sizeof(vec3)) ; 
			nukes[i].length   = (int)(nukes[i].end_angle *8);
			nukes[i].angle    = 0;
			nukes[i].height   = 0;
			nukes[i].velocity = 50;
			
			vec3 endpos = euclidian(vec3(.721,cities[(city_count-(i*8)-1)*3+1],cities[(city_count-(i*8)-1)*3+2] ));
			float angle=glm::acos ( glm::dot(glm::normalize(nukes[i].startpos), glm::normalize(endpos)) );
	
			nukes[i].velocity = 49.f*angle/2;
			explosions[i].a=0;
		}	
		glColor3f(1, .9, .9);
		mat4 rotmatrix = glm::rotate(mat4(1), nukes[i].angle, nukes[i].axis );
		vec4 pathvec  = rotmatrix * vec4(nukes[i].startpos * vec3(1+nukes[i].height) , 1 ) ;
		
		
		//memmove(nuke_paths[i], nuke_paths[i]+1,)
		
		if( 
			nukes[i].length > 1 && 
			!( (int)(nukes[i].angle*100+.0001) % (int)(100*.02*4+.0001) ) ){
			nukes[i].length--;
		}
		
		for (int np = 31; np > 0; np--){
			nuke_paths[i][np] = nuke_paths[i][np-1]; //TODO use memmove instead
		}

		nuke_paths[i][0]=vec3(pathvec.x,pathvec.y,pathvec.z);
		
		glLineWidth(3);
    	glBegin(GL_LINES);
		glColor3f(.3f,.3f,.3f);
		vec4 finvec = projection * vec4(nuke_paths[i][0],1);
		glVertex4fv((float*) &finvec);
		
		glColor3f(.8f,.7f,.2f);
		finvec = projection * vec4(nuke_paths[i][1],1);
		glVertex4fv((float*) &finvec);
		
		glEnd();
		glLineWidth(2);
    	glBegin(GL_LINE_STRIP);



		for (int np = 1; np < nukes[i].length  ; np ++ ){
			glColor4f(.5f,1.f,8.f,1.f-(1.f/nukes[i].length)*np);
			if (nuke_paths[i][np].x == 0)	break;
			vec4 finvec = projection * vec4(nuke_paths[i][np],1);

			glVertex4fv((float*) &finvec);
		}
		glEnd();	
	/*	
    	glBegin(GL_LINE_STRIP);
		for (int np = 1; np < 16 ; np ++ ){
	
			glColor4f(.5f,.5f,.5f,1.f-(1.f/16)*(np) );
			vec4 finvec = projection * vec4(nuke_paths[i][np],1);

			glVertex4fv((float*) &finvec);
		}
		glEnd();	
	*/	
		nukes[i].height += .00005f*nukes[i].velocity;
		nukes[i].velocity--;	
		nukes[i].angle += .02f;
	
	}
	for (int i=0;i<16;i++){
		if (explosions[i].a<24)	{	
				explosions[i].a++;
		}
	}	

	glUseProgram(explosionshader);
	glUniform4fv( glGetUniformLocation(explosionshader,"u_explosions" ), 16,(float*) explosions );
	glUniformMatrix4fv( 
					glGetUniformLocation(explosionshader,"u_Projection") , 
					1, 0, (float*) &projection 
					);
	glEnable(GL_DEPTH_TEST);
	
	glBindBuffer(GL_ARRAY_BUFFER, explosion_buffer);
	glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
	
	glDrawArraysInstanced(GL_TRIANGLES, 0,120,16);	
	
	if(!drawcities)return;
	
	glUseProgram(cityshader);
	glUniform3fv( glGetUniformLocation(cityshader,"u_Cities" ), city_count, cities );
	glUniformMatrix4fv( 
					glGetUniformLocation(cityshader,"u_Projection") , 
					1, 0, (float*) &projection 
					);
	glUniform1i( glGetUniformLocation(cityshader,"hover_city_index") , hovercity );
	glEnable(GL_DEPTH_TEST);
	
	glBindBuffer(GL_ARRAY_BUFFER, city_buffer);
	glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
	
	glDrawArraysInstanced(GL_TRIANGLES, 0,120,city_count);	
    
	glUseProgram(0);

	if (hovercity == -1) return;
    glDisable(GL_DEPTH_TEST);
    
    vec3 v=*((vec3*)(cities+hovercity*3));
	vec4 v2=projection*(vec4(euclidian(vec3(.721,v.y,v.z)),1));
    if(v2.z>0)return;

    glColor4f(0,0,0,.5);
	glLineWidth(5);
	float w=(cityname_dims.x + 10)/1080.0*1.5;
	float h=(cityname_dims.y +  3)/1080.0*1.5;
	
	glBegin(GL_QUADS);
		glVertex3f(v2.x + .01,		v2.y + .01		,-.8);
		glVertex3f(v2.x + .01 + w,	v2.y + .01		,-.8);
		glVertex3f(v2.x + .01 + w,	v2.y + .01 + h	,-.8);
		glVertex3f(v2.x + .01,		v2.y + .01 + h	,-.8);
	glEnd();
	
	glLineWidth(.2);
	glColor4f(.5,.5,.5,.3);
	
	glBegin(GL_LINE_LOOP);
		glVertex3f(v2.x + .01 + 	.001,	v2.y + .01 + 	.001,	-.8);
		glVertex3f(v2.x + .01 + w - .001,	v2.y + .01 + 	.001,	-.8);
		glVertex3f(v2.x + .01 + w - .001,	v2.y + .01 + h -.001,	-.8);
		glVertex3f(v2.x + .01 + 	.001,	v2.y + .01 + h -.001,	-.8);
	glEnd();

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,cityname_texture);
	glColor4f(1,1,1,.5f);
	
	glBegin(GL_QUADS);
	
		glVertex3f(v2.x+.01+.01,	v2.y+.01+h-.005    ,-.8);
		glTexCoord2f(1,0);

		glVertex3f(v2.x+.01+w-.01,	v2.y+.01+h-.005 	,-.8);
		glTexCoord2f(1,1);

		glVertex3f(v2.x+.01+w-.01,	v2.y+.01+.005    	,-.8);
		glTexCoord2f(0,1);

		glVertex3f(v2.x+.01+.01,	v2.y+.01+.005    	,-.8);
		glTexCoord2f(0,0);
	
	glEnd();
	
	glDisable(GL_TEXTURE_2D);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);


}

static bool click(Panel *panel,SDL_Event event,int x,int y){
	std::cout << "click\n";
	return 0;
	
}

static void colorlistadd(Panel* p,void* listdata,int index){

    ColorPicker<float>* cp= new ColorPicker<float>(10,10+110*index ,150,100,0);
	for (char i=0;i<12;i++){
		cp->colors[i] = UI::rules["button"][UI::colstrings[i]] ;
		cp->colorBall->colors[i] = UI::rules["button"][UI::colstrings[i]] ;
		cp->brightnessPicker->colors[i] = UI::rules["button"][UI::colstrings[i]] ;
		cp->opacityPicker->colors[i] = UI::rules["button"][UI::colstrings[i]] ;
		cp->preview->colors[i] = UI::rules["button"][UI::colstrings[i]] ;
	}
	cp->color= ((float*)listdata) + 4*index;
	p->add(cp);
}

int main(){
	cursorp=vec3(.7,.1,.1);
	cursor=euclidian(cursorp);
	RenderObj renderObject;
	Skybox ro2;
	
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	UI::init({
		{"close" 		, (void*) close 	},
		{"click" 		, (void*) click 	},
		{"draw" 		, (void*) draw 	},
		{"object"  		, (void*) &ro2 	},
		{"mousemove" 	, (void*) mousemove },
		{"mousedrag" 	, (void*) mousedrag },
		{"keypress" 	, (void*) key 	  },
		{"update" 		, (void*) act 	  },
		{"color" 		, (void*) spectrum },
		{"colorlistadd" , (void*) colorlistadd },
		{"roughness" 	, (void*) &roughness   },
		{"shine"    	, (void*) &shine 	  },
		{"drawcities"  	, (void*) &drawcities  }
	},"style1.style","data.xml");
	init();

	vertices=new float[lo*la*VERTEX_SIZE];
	
	void(*render)(Panel *panel) =[](Panel *panel){
		for (char i=0 ;i<8;i++){
			float* d = (float*)panel->data;
			glColor3fv(d+i*4+1);
			glBegin(GL_QUADS);
			glVertex2f(-.9+ i* (.9f /4),-.8 +( (d[i*4]-.715)*50  ) );
			glVertex2f(-.9+ i* (.9f /4)+(.9f /4),-.8+( (d[i*4]-.715)*50  ));
			glVertex2f(-.9+ i* (.9f /4)+(.9f /4),-.8);
			glVertex2f(-.9+ i* (.9f /4),-.8);
			glEnd();
		}
	};

	srand (time(NULL));
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	for (int y=0;y<la;y++){
		float pi =3.1415;
		float equator=2*.7*pi;
		float longlen = abs((sin(float(y)/la*pi+pi)*.7)*2*pi);
		float ratio=longlen/equator;

		glm::vec2 rep(10000,10000);

		for (int x=0;x<lo;x++){

			float x1=((x-360)*ratio)/90.0;
			float y1=y/45.0;

			float x2=((x+360)*ratio)/90.0;

			float h=(glm::perlin(glm::vec2(x1,y1),rep)+glm::perlin(glm::vec2(x2,y1),rep))/2.0;

			heightmap[x+lo*y] =	abs(h);

		}
	}

	SDL_Surface* img=SDL_LoadBMP("../res/earth.bmp");
	unsigned char* pixels=(unsigned char*)img->pixels;

	for (int i=0;i<(VERTEX_COUNT);i++){
		vertices[i*VERTEX_SIZE]		=.721+pixels[i+1]/256.0;
		vertices[i*VERTEX_SIZE+1]	= PI*2/lo * (i%lo);
		vertices[i*VERTEX_SIZE+2]	= PI*2/lo * ((i/lo)+1);
		vertices[i*VERTEX_SIZE+3]	= .11;
		vertices[i*VERTEX_SIZE+4]	= 0;
	}


	for (int j=0;j<la-2;j++){
		for (int i=0;i<lo;i++){
			indices1[6*lo*j+i*6+0]	=lo*j+(i)+0       +1;
			indices1[6*lo*j+i*6+1]	=lo*j+(i)+lo      +1;
			indices1[6*lo*j+i*6+2]	=lo*j+(i+1)%lo    +1;

			indices1[6*lo*j+i*6+3]	=lo*j+(i+1)%lo    +1;
			indices1[6*lo*j+i*6+4]	=lo*j+(i)+lo      +1;
			indices1[6*lo*j+i*6+5]	=lo*j+(i+1)%lo+lo +1;
		}
	}

	for (int i=0;i<lo;i++){
		indices1[6*(VERTEX_COUNT-2)+0+i*3]	=0;
		indices1[6*(VERTEX_COUNT-2)+1+i*3]	=i+1;
		indices1[6*(VERTEX_COUNT-2)+2+i*3]	=(i+1)%lo+1;
	}
	for (int i=0;i<lo;i++){
		indices1[6*(VERTEX_COUNT-2)+3*lo+0+i*3]	=(VERTEX_COUNT-2)+1;
		indices1[6*(VERTEX_COUNT-2)+3*lo+1+i*3]	=(VERTEX_COUNT-2)-i;
		indices1[6*(VERTEX_COUNT-2)+3*lo+2+i*3]	=(VERTEX_COUNT-2)-(i+1)%lo;
	}


	ShaderProgram polarshader=ShaderProgram(
			compileShader(GL_VERTEX_SHADER,readfile("shaders/basic.vert")),
			0,
			compileShader(GL_FRAGMENT_SHADER,readfile("shaders/basic.frag"))
	);
	cityshader=createShaderProgram(
		compileShader(GL_VERTEX_SHADER,readfile("shaders/city.vert")),
		0,
		compileShader(GL_FRAGMENT_SHADER,readfile("shaders/basic.frag"))
	);
	explosionshader=createShaderProgram(
		compileShader(GL_VERTEX_SHADER,readfile("shaders/explosion.vert")),
		0,
		compileShader(GL_FRAGMENT_SHADER,readfile("shaders/basic.frag"))
	);


	std::cout << "1_a1\n";
	int layout[3]={3,1,1};

	uniform uniforms[12] ={
		uniform {glGetUniformLocation(polarshader.id,"u_Cursor"),	UNIFORM_VEC3,1,(float*)&cursor},
		uniform {glGetUniformLocation(polarshader.id,"u_Camera"),	UNIFORM_VEC4,1,(float*)&camera},
		uniform {glGetUniformLocation(polarshader.id,"u_Csize"),	UNIFORM_FLOAT,1,&cursorsize},
		uniform {glGetUniformLocation(polarshader.id,"u_Scale"),	UNIFORM_FLOAT,1,&scale},
		uniform {glGetUniformLocation(polarshader.id,"u_FrameNr"),	UNIFORM_UINT,1,(float*)&framenr},
		uniform {glGetUniformLocation(polarshader.id,"u_Light"),	UNIFORM_VEC3,1,(float*)&light},
		uniform {glGetUniformLocation(polarshader.id,"u_Projection"),UNIFORM_MAT4,1,(float*)&projection},
		uniform {glGetUniformLocation(polarshader.id,"u_ModelView"),UNIFORM_MAT4,1,(float*)&modelview},
		uniform {glGetUniformLocation(polarshader.id,"u_Roughness"),UNIFORM_FLOAT,1,&roughness},//b1.value},
		uniform {glGetUniformLocation(polarshader.id,"u_Shine")  ,	    UNIFORM_FLOAT,1,&shine},//b2.value},
		uniform {glGetUniformLocation(polarshader.id,"u_Spectrum"),	    UNIFORM_VEC4,16,spectrum},
		uniform {glGetUniformLocation(polarshader.id,"u_explosions"),	UNIFORM_VEC4,16,(float*)&explosions}
		
	};


	renderObject = RenderObj (
		TRIANGLE_COUNT,indices1,
		VERTEX_COUNT,vertices,
		3,  layout,
		12, uniforms
	);
	renderObject.shader=&polarshader;


	SDL_Surface *xpos = IMG_Load("../res/xpos.png");	SDL_Surface *xneg = IMG_Load("../res/xneg.png");
	SDL_Surface *ypos = IMG_Load("../res/ypos.png");	SDL_Surface *yneg = IMG_Load("../res/yneg.png");
	SDL_Surface *zpos = IMG_Load("../res/zpos.png");	SDL_Surface *zneg = IMG_Load("../res/zneg.png");


	if(!(xpos && ypos && zpos && xneg && yneg && zneg)) {
		printf("IMG_Load: %s\n", IMG_GetError());
		// handle error
	}


	ro2 =Skybox (
					(float*)&sky_Matrix,xpos->w,
					xpos->pixels,  ypos->pixels,
					zpos->pixels,  xneg->pixels,
					yneg->pixels,  zneg->pixels
				);

	ro2.children.push_back(renderObject);

	scale=1;

	glGenBuffers(1, &city_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, city_buffer);
	glBufferData(GL_ARRAY_BUFFER, (40*3*3) *sizeof(float), mesh_dome, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &explosion_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, explosion_buffer);
	glBufferData(GL_ARRAY_BUFFER, (40*3*3) *sizeof(float), mesh_dome, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	UI::run();
	delete vertices;
	return 0;

}

