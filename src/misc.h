/*
 * misc.h
 *
 *  Created on: Jan 27, 2018
 *      Author: Nr5
 */

#ifndef MISC_H_
#define MISC_H_
#include <string>
#include <fstream>

#include <sstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>

std::string readfile(std::string filename){
	std::ifstream inFile;
	std::string ret;
	inFile.open(filename);//open the input file
	std::stringstream stream;
	stream << inFile.rdbuf();//read the file
	ret = stream.str();//str holds the content of the file
	inFile.close();

	return ret;
}

static unsigned int compileShader(unsigned int type, const std::string& source){
	unsigned int id= glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id,1,&src,nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id,GL_COMPILE_STATUS, &result);
	if (!result){
		std::cout <<(type==GL_VERTEX_SHADER?"vertex":type==GL_FRAGMENT_SHADER?"fragment":"geometry")<<"\n";
		int length;
		glGetShaderiv(id,GL_INFO_LOG_LENGTH,&length);
		char* message= (char*)alloca(length*sizeof(char));
		glGetShaderInfoLog(id,length,&length,message);
		std::cout <<message<<"\n";
		glDeleteShader(id);
		return 0;
	}
	return id;
}

static unsigned int createShaderProgram(unsigned int vertexshader,unsigned int geometryshader,unsigned int fragmentshader){
	unsigned int program = glCreateProgram();

	glAttachShader(program,vertexshader);
	glAttachShader(program,geometryshader);
	glAttachShader(program,fragmentshader);

	glLinkProgram(program);
	glValidateProgram(program);
/*
	glDeleteShader(vertexshader);
	glDeleteShader(geometryshader);
	glDeleteShader(fragmentshader);
*/
	return program;

}

static unsigned int loadTexture(const char* filename){
	SDL_Surface* img=SDL_LoadBMP(filename);
	unsigned int id;
	glGenTextures(1,&id);
	glBindTexture(GL_TEXTURE_2D,id);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,img->w,img->h,0,GL_RGB,GL_UNSIGNED_SHORT_5_6_5,img->pixels);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	SDL_FreeSurface(img);
	return id;
}
static unsigned int loadTexture3d(const char* filename,int depth){
	SDL_Surface* img=SDL_LoadBMP(filename);

	unsigned int id;
	glGenTextures(1,&id);
	glBindTexture(GL_TEXTURE_3D,id);
	glTexImage3D(GL_TEXTURE_3D,0,GL_RGB,img->w,img->h/depth,depth,0,GL_RGB,GL_UNSIGNED_SHORT_5_6_5,img->pixels);
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glGenerateMipmap(GL_TEXTURE_3D);
	SDL_FreeSurface(img);
	return id;
}


#endif /* MISC_H_ */
