#pragma once


#include <string>
#include <vector>
#include <Windows.h>
class Vector3 {
public:
	float x, y, z;
	void set(float dx, float dy, float dz) { x = dx; y = dy; z = dz; }
	void set(Vector3& v) { x = v.x; y = v.y; z = v.z; }
	void normalize()//adjust this vector to unit length
	{
		double sizeSq = x * x + y * y + z * z;
		if (sizeSq < 0.0000001)
		{
			return; // does nothing to zero vectors;
		}
		float scaleFactor = 1.0 / (float)sqrt(sizeSq);
		x *= scaleFactor; y *= scaleFactor; z *= scaleFactor;
	}
	Vector3(float xx, float yy, float zz) { x = xx; y = yy; z = zz; }
	Vector3(Vector3 const & v) { x = v.x; y = v.y; z = v.z; }
	Vector3() { x = y = z = 0; } //default constructor
	Vector3 cross(Vector3 b) //return this cross b
	{
		Vector3 c(y*b.z - z*b.y, z*b.x - x*b.z, x*b.y - y*b.x);
		return c;
	}
	float dot(Vector3 b) // return this dotted with b
	{
		return x * b.x + y * b.y + z * b.z;
	}
};
class Shader {
public:
	static GLuint createShader(GLenum eShaderType, const std::string &strShaderFile);
	static GLuint createShader(GLenum eShaderType, const char* fileName);
	static GLuint createProgram(const std::vector<GLuint> &shaderList);
	//model
	static void transformMatrix(GLfloat x, GLfloat y, GLfloat z, GLfloat *matrix)
	{
		for (int i = 0; i < 16; i++)
			matrix[i] = 0;
		matrix[0] = matrix[5] = matrix[10] = matrix[15] = 1.0;
		matrix[3] = x; matrix[7] = y; matrix[11] = z;
	}
	//view
	static void LookAtMatrix(Vector3 eye, Vector3 look, Vector3 up, float * matrix)
	{
		for (int i = 0; i < 16; i++)
			matrix[i] = 0;
		Vector3 z(eye.x - look.x, eye.y - look.y, eye.z - look.z);
		z.normalize();
		Vector3 x(z.cross(up));
		x.normalize();
		Vector3 y(x.cross(z));
		y.normalize();

		matrix[0] = -x.x;
		matrix[4] = -x.y;
		matrix[8] = -x.z;
		matrix[12] = x.dot(eye);

		matrix[1] = y.x;
		matrix[5] = y.y;
		matrix[9] = y.z;
		matrix[13] = -y.dot(eye);

		matrix[2] = z.x;
		matrix[6] = z.y;
		matrix[10] = z.z;
		matrix[14] = -z.dot(eye);

		matrix[3] = matrix[7] = matrix[11] = 0.0f;
		matrix[15] = 1.0f;
	}
	//projection
	static void frustumMatrix(float l, float r, float b, float t, float zNear, float zFar, float * matrix)
	{
		for (int i = 0; i < 16; i++)
			matrix[i] = 0;
		matrix[0] = 2 * zNear / (r - l);
		matrix[8] = (r + l) / (r - l);
		matrix[5] = 2 * zNear / (t - b);
		matrix[9] = (t + b) / (t - b);
		matrix[10] = -(zFar + zNear) / (zFar - zNear);
		matrix[14] = -(2 * zFar * zNear) / (zFar - zNear);
		matrix[11] = -1;
		matrix[15] = 0;
	}
	//read bmp file
	static void loadBMPFile(GLuint *TBO,char * filename) {
		GLubyte *pImage = NULL;
		GLint  iWidth;
		GLint  iHeight;
		GLint  icomponents;
		GLenum  eFormat;

		glGenTextures(1, TBO);
		glBindTexture(GL_TEXTURE_2D, *TBO);


		pImage = Shader::loadBMPImage(filename, &iWidth, &iHeight, &icomponents, &eFormat);
		glTexImage2D(GL_TEXTURE_2D, 0, icomponents, iWidth, iHeight, 0, eFormat, GL_UNSIGNED_BYTE, pImage);
		glGenerateMipmap(GL_TEXTURE_2D);

		//texture setting
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		free(pImage);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
private:
	static  GLubyte *loadBMPImage(char * filename, GLint * iWidth, GLint * iHeight, GLint * icomponents, GLenum * eFormat)
	{
		FILE *fp = fopen(filename, "rb");
		if (fp == NULL)
			return NULL;
		if (fseek(fp, sizeof(BITMAPFILEHEADER), 0))
			return NULL;
		BITMAPINFOHEADER infoHead;
		fread(&infoHead, sizeof(BITMAPINFOHEADER), 1, fp);
		*iWidth = infoHead.biWidth;
		*iHeight = infoHead.biHeight;
		GLint linebyte = (*iWidth * 24 / 8 + 3) / 4 * 4;
		GLubyte *pBits = new unsigned char[linebyte*(*iHeight)];
		fread(pBits, sizeof(char), linebyte*(*iHeight), fp);
		fclose(fp);

		*icomponents = GL_RGB8;
		*eFormat = GL_BGR_EXT;

		return pBits;
	}
};

