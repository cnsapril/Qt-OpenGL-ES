#ifndef ME_GL_WINDOW
#define ME_GL_WINDOW

#include <GL\glew.h>
#include <QtOpenGL\QGLWidget>
#include <QtWidgets\qwidget.h>
#include <QtCore\qtimer.h>
#include <glm\glm.hpp>
#include <glm\gtx\transform.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <stb_image.h>
#include <Primitives\ShapeGenerator.h>
#include <fstream>
#include <iostream>

#include <Primitives\Haptic.h>

using namespace std;

class MeGlWindow : public QGLWidget
{
public:
	MeGlWindow();
	~MeGlWindow();
protected:
	void initializeGL();
	void paintGL();
	void resizeGL(int w, int h);
private:
	void sendDataToOpenGL();
	bool checkStatus(GLuint objectID, PFNGLGETSHADERIVPROC objectPropertyGetterFunc, PFNGLGETSHADERINFOLOGPROC getInfoLogFunc, GLenum statusType);
	bool checkShaderStatus(GLuint shaderID);
	bool checkProgramStatus(GLuint programID);
	string readShaderCode(const char * fileName);
	void installShaders();
	void loadTexture(char *imgPath);
	void convertMat4ToDoubleArray(glm::mat4 mat, HDdouble *doubleArray);
	void convertDoubleArrayToMat4(HDdouble *doubleArray, glm::mat4* mat);
	void negateZ(glm::mat4* mat, int column);
	void initTexture();
	QTimer* myTimer;
	void initHD();
	void updateWorkspace();
};

#endif

