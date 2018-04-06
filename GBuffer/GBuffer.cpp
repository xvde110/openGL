// GBuffer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "shader.h"
#include <time.h>

using namespace std;



GLuint VAO, VBO;
GLuint programId;
GLuint TBO, FBO;
GLuint VAO2, VBO2, programId2;
GLuint gPosition, gNormal, gColor;
static const GLfloat vertex_list[][3] = {
	-1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	-1.0f,   1.0f, -1.0f,
	1.0f,   1.0f, -1.0f,
	-1.0f, -1.0f,   1.0f,
	1.0f, -1.0f,   1.0f,
	-1.0f,   1.0f,   1.0f,
	1.0f,   1.0f,   1.0f,
};
// 将要使用的顶点的序号保存到一个数组里面
static const GLint index_list[][4] = {
	0, 2, 3, 1,
	0, 4, 6, 2,
	0, 1, 5, 4,
	4, 5, 7, 6,
	1, 3, 7, 5,
	2, 6, 7, 3,
};
static const GLfloat normal_list[][3] = {
	0,0,-1,
	-1,0,0,
	0,-1,0,
	0,0,1,
	1,0,0,
	0,1,0
};
GLfloat vertices[8 * 4 * 8];
GLfloat vertices2[] = { -1,-1,-1,1,1,1,1,-1 };
void init() {
	//set data
	for (int i = 0; i < 6; ++i)          // 有六个面，循环六次
	{
		for (int j = 0; j < 4; ++j)      // 每个面有四个顶点，循环四次
		{
			//position
			vertices[i * 32 + j * 8 + 0] = vertex_list[index_list[i][j]][0];
			vertices[i * 32 + j * 8 + 1] = vertex_list[index_list[i][j]][1];
			vertices[i * 32 + j * 8 + 2] = vertex_list[index_list[i][j]][2];
			//normal
			vertices[i * 32 + j * 8 + 3] = normal_list[i][0];
			vertices[i * 32 + j * 8 + 4] = normal_list[i][1];
			vertices[i * 32 + j * 8 + 5] = normal_list[i][2];
			//texcoords
			switch (j % 4)
			{
			case 0:vertices[i * 32 + j * 8 + 6] = 1.0; vertices[i * 32 + j * 8 + 7] = 1.0; break;
			case 1:vertices[i * 32 + j * 8 + 6] = 1.0; vertices[i * 32 + j * 8 + 7] = 0.0; break;
			case 2:vertices[i * 32 + j * 8 + 6] = 0.0; vertices[i * 32 + j * 8 + 7] = 0.0; break;
			case 3:vertices[i * 32 + j * 8 + 6] = 0.0; vertices[i * 32 + j * 8 + 7] = 1.0; break;
			}
		}
	}


	//base setting
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glEnable(GL_DEPTH_TEST);

	//VAO VBO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (void*)(sizeof(GLfloat) * 3));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (void*)(sizeof(GLfloat) * 6));
	glEnableVertexAttribArray(2);

	//create shader 
	vector<GLuint> v;
	v.push_back(Shader::createShader(GL_VERTEX_SHADER, "vertexShader.glsl"));
	v.push_back(Shader::createShader(GL_FRAGMENT_SHADER, "fragmentShader.glsl"));
	programId = Shader::createProgram(v);

	//load texture
	Shader::loadBMPFile(&TBO, "wood.bmp");

	//g buffer create
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	//position
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gPosition, 0);

	//normal
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gNormal, 0);

	//color
	glGenTextures(1, &gColor);
	glBindTexture(GL_TEXTURE_2D, gColor);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gColor, 0);

	GLuint attachments[] = { GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1,GL_COLOR_ATTACHMENT2,GL_COLOR_ATTACHMENT3 };
	glDrawBuffers(4, attachments);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//第二次渲染初始化
	glGenVertexArrays(1, &VAO2);
	glBindVertexArray(VAO2);

	glGenBuffers(1, &VBO2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 2, 0);
	glEnableVertexAttribArray(0);

	v.clear();
	v.push_back(Shader::createShader(GL_VERTEX_SHADER, "vertexShader2.glsl"));
	v.push_back(Shader::createShader(GL_FRAGMENT_SHADER, "fragmentShader2.glsl"));
	programId2 = Shader::createProgram(v);
}
void setParameter() {
	GLfloat cameraPos[3] = { 2,2,4 };

	GLfloat mat[16];
	Shader::transformMatrix(0, 0, 0, mat);
	glUniformMatrix4fv(glGetUniformLocation(programId, "modelMatrix"), 1, GL_TRUE, mat);
	Shader::LookAtMatrix(Vector3(cameraPos[0], cameraPos[1], cameraPos[2]), Vector3(0, 0, 0), Vector3(0, 1, 0), mat);
	glUniformMatrix4fv(glGetUniformLocation(programId, "viewMatrix"), 1, GL_FALSE, mat);
	Shader::frustumMatrix(-1, 1, -1, 1, 1, 200, mat);
	glUniformMatrix4fv(glGetUniformLocation(programId, "projectionMatrix"), 1, GL_FALSE, mat);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TBO);
	glUniform1i(glGetUniformLocation(programId, "wood"), 0);

	glUniform3f(glGetUniformLocation(programId, "light.ambient"), 0.4, 0.4, 0.4);
	glUniform3f(glGetUniformLocation(programId, "light.lightPos"), -1, 1, 5);
	glUniform3f(glGetUniformLocation(programId, "light.lightColor"), 0.8, 0.0, 0.0);
	glUniform3f(glGetUniformLocation(programId, "light.direction"), 0, 0, -1);
	glUniform1f(glGetUniformLocation(programId, "light.cutoff"), cos(20.0/180*3.1415926));
	glUniform1f(glGetUniformLocation(programId, "light.outercutoff"), cos(30.0 / 180 * 3.1415926));

	glUniform3f(glGetUniformLocation(programId, "light2.ambient"), 0.4, 0.4, 0.4);
	glUniform3f(glGetUniformLocation(programId, "light2.lightPos"), 1, 1, 5);
	glUniform3f(glGetUniformLocation(programId, "light2.lightColor"), 0.0, 0.8, 0.0);
	glUniform3f(glGetUniformLocation(programId, "light2.direction"), 0, 0, -1);
	glUniform1f(glGetUniformLocation(programId, "light2.cutoff"), cos(20.0 / 180 * 3.1415926));
	glUniform1f(glGetUniformLocation(programId, "light2.outercutoff"), cos(30.0 / 180 * 3.1415926));
}
void setParameter2() {

	//set G buffer
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glUniform1i(glGetUniformLocation(programId2, "gPosition"), 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glUniform1i(glGetUniformLocation(programId2, "gNormal"), 1);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gColor);
	glUniform1i(glGetUniformLocation(programId2, "gColor"), 2);

	glUniform3f(glGetUniformLocation(programId2, "light.ambient"), 0.4, 0.4, 0.4);
	glUniform3f(glGetUniformLocation(programId2, "light.lightPos"), -1, 1, 5);
	glUniform3f(glGetUniformLocation(programId2, "light.lightColor"), 0.8, 0.0, 0.0);
	glUniform3f(glGetUniformLocation(programId2, "light.direction"), 0, 0, -1);
	glUniform1f(glGetUniformLocation(programId2, "light.cutoff"), cos(20.0 / 180 * 3.1415926));
	glUniform1f(glGetUniformLocation(programId2, "light.outercutoff"), cos(30.0 / 180 * 3.1415926));

	glUniform3f(glGetUniformLocation(programId2, "light2.ambient"), 0.4, 0.4, 0.4);
	glUniform3f(glGetUniformLocation(programId2, "light2.lightPos"), 1, 1, 5);
	glUniform3f(glGetUniformLocation(programId2, "light2.lightColor"), 0.0, 0.8, 0.0);
	glUniform3f(glGetUniformLocation(programId2, "light2.direction"), 0, 0, -1);
	glUniform1f(glGetUniformLocation(programId2, "light2.cutoff"), cos(20.0 / 180 * 3.1415926));
	glUniform1f(glGetUniformLocation(programId2, "light2.outercutoff"), cos(30.0 / 180 * 3.1415926));
}
void display() {

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(programId);
	glBindVertexArray(VAO);
	setParameter();
	glDrawArrays(GL_QUADS, 0, 4*6);
	glBindVertexArray(0);


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(programId2);
	glBindVertexArray(VAO2);
	setParameter2();
	glDrawArrays(GL_QUADS, 0, 4);
	glBindVertexArray(0);


	glutSwapBuffers();
}
int main(int argc, char **argv)
{
	srand(time(0));
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(512, 512);
	glutCreateWindow("Triangle demo");

	glewInit();
	init();

	glutDisplayFunc(display);
	glutMainLoop();
	return 0;
}

