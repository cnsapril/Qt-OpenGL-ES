#include <MeGlWindow.h>

#define IPHONE_WIDTH 77.9
#define IPHONE_HEIGHT 158.2
#define IPHONE_DEPTH 7.3

#define SCALE_IPHONE_WIDTH() 1.0f / IPHONE_HEIGHT * IPHONE_WIDTH
#define SCALE_IPHONE_HEIGHT() 1.0f
#define SCALE_IPHONE_DEPTH() 1.0f / IPHONE_HEIGHT * IPHONE_DEPTH

GLuint theBufferID;

GLuint cubeNumIndices;
GLuint sphereNumIndices;

GLuint cubeVertexArrayObjectID;
GLuint sphereVertexArrayObjectID;

GLuint cubeIndexByteOffset;
GLuint sphereIndexByteOffset;

GLuint programID;
GLuint sphereProgramID;

GLuint texture2D_front;
GLuint texture2D_back;

using glm::vec3;
using glm::mat4;

void MeGlWindow::sendDataToOpenGL()
{
	ShapeData cube = ShapeGenerator::makeCube();
	ShapeData sphere = ShapeGenerator::makeSphere(40);

	glGenBuffers(1, &theBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, theBufferID);
	glBufferData(GL_ARRAY_BUFFER,
		cube.vertexBufferSize() + cube.indexBufferSize() +
		sphere.vertexBufferSize() + sphere.indexBufferSize(), 0, GL_STATIC_DRAW);

	GLsizeiptr currentOffset = 0;
	glBufferSubData(GL_ARRAY_BUFFER, currentOffset, cube.vertexBufferSize(), cube.vertices);
	currentOffset += cube.vertexBufferSize();
	cubeIndexByteOffset = currentOffset;
	glBufferSubData(GL_ARRAY_BUFFER, currentOffset, cube.indexBufferSize(), cube.indices);
	currentOffset += cube.indexBufferSize();
	glBufferSubData(GL_ARRAY_BUFFER, currentOffset, sphere.vertexBufferSize(), sphere.vertices);
	currentOffset += sphere.vertexBufferSize();
	sphereIndexByteOffset = currentOffset;
	glBufferSubData(GL_ARRAY_BUFFER, currentOffset, sphere.indexBufferSize(), sphere.indices);
	currentOffset += sphere.indexBufferSize();

	cubeNumIndices = cube.numIndices;
	sphereNumIndices = sphere.numIndices;

	glGenVertexArrays(1, &cubeVertexArrayObjectID);
	glGenVertexArrays(1, &sphereVertexArrayObjectID);

	glBindVertexArray(cubeVertexArrayObjectID);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, theBufferID);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)* 8, 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)* 8, (void*)(6*sizeof(float)));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, theBufferID);

	glBindVertexArray(sphereVertexArrayObjectID);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, theBufferID);
	GLuint sphereByteOffset = cube.vertexBufferSize() + cube.indexBufferSize();
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)* 8, (void*)sphereByteOffset);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float)* 8, (void*)(sphereByteOffset + sizeof(float)* 3));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, theBufferID);

	cout << "Number of vertices per Sphere: " << sphere.numVertices;

	cube.cleanup();
	sphere.cleanup();
}

bool MeGlWindow::checkStatus(GLuint objectID,
	PFNGLGETSHADERIVPROC objectPropertyGetterFunc,
	PFNGLGETSHADERINFOLOGPROC getInfoLogFunc,
	GLenum statusType)
{
	GLint status;
	objectPropertyGetterFunc(objectID, statusType,
		&status);
	if (status != GL_TRUE)
	{
		GLint infoLogLength;
		objectPropertyGetterFunc(objectID, GL_INFO_LOG_LENGTH,
			&infoLogLength);
		GLchar* buffer = new GLchar[infoLogLength];

		GLsizei bufferSize;
		getInfoLogFunc(objectID, infoLogLength,
			&bufferSize, buffer);

		cout << buffer << endl;

		delete[] buffer;
		return false;
	}

	return true;
}

bool MeGlWindow::checkShaderStatus(GLuint shaderID)
{
	return checkStatus(shaderID, glGetShaderiv,
		glGetShaderInfoLog, GL_COMPILE_STATUS);
}

bool MeGlWindow::checkProgramStatus(GLuint programID)
{
	return checkStatus(programID, glGetProgramiv,
		glGetProgramInfoLog, GL_LINK_STATUS);
}

string MeGlWindow::readShaderCode(const char* fileName)
{
	ifstream meInput(fileName);
	if (!meInput.good())
	{
		cout << "File failed to load ... " << fileName;
		exit(1);
	}
	return string(
		istreambuf_iterator<char>(meInput),
		istreambuf_iterator<char>());
}

void MeGlWindow::installShaders()
{
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	
	const char* adapter[1];
	string tempVertex = readShaderCode("VertexShaderCode.glsl");
	adapter[0] = tempVertex.c_str();
	glShaderSource(vertexShaderID, 1, adapter, 0);
	string tempFragment = readShaderCode("FragmentShaderCode.glsl");
	adapter[0] = tempFragment.c_str();
	glShaderSource(fragmentShaderID, 1, adapter, 0);

	glCompileShader(vertexShaderID);
	glCompileShader(fragmentShaderID);

	if (!checkShaderStatus(vertexShaderID) || 
		!checkShaderStatus(fragmentShaderID))
	{
		return;
	}

	programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);

	if (!checkProgramStatus(programID))
	{
		return;
	}

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	tempVertex = readShaderCode("SphereVertexShader.glsl");
	adapter[0] = tempVertex.c_str();
	glShaderSource(vertexShaderID, 1, adapter, 0);
	tempFragment = readShaderCode("SphereFragmentShader.glsl");
	adapter[0] = tempFragment.c_str();
	glShaderSource(fragmentShaderID, 1, adapter, 0);

	glCompileShader(vertexShaderID);
	glCompileShader(fragmentShaderID);

	if (!checkShaderStatus(vertexShaderID) || !checkShaderStatus(fragmentShaderID))
		return;

	sphereProgramID = glCreateProgram();
	glAttachShader(sphereProgramID, vertexShaderID);
	glAttachShader(sphereProgramID, fragmentShaderID);

	glLinkProgram(sphereProgramID);

	if (!checkProgramStatus(sphereProgramID))
		return;

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);
}

void MeGlWindow::loadTexture(char *imgPath)
{
	GLuint texture;
	glGenTextures(1, &texture);

	int width, height, comp = 0;
	unsigned char *img = 0;

	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	img = stbi_load(imgPath, &width, &height, &comp, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, 
		GL_UNSIGNED_BYTE, img);

	stbi_image_free(img);
}

MeGlWindow::MeGlWindow()
{
	setMinimumSize(1024, 768);

	myTimer = new QTimer();
	connect(myTimer, SIGNAL(timeout()), this, SLOT(update()));
	myTimer->start(30);
}

MeGlWindow::~MeGlWindow()
{
	glUseProgram(0);
	glDeleteProgram(programID);
	glDeleteProgram(sphereProgramID);
	delete myTimer;
}

void MeGlWindow::initializeGL()
{
	glewInit();
	glEnable(GL_DEPTH_TEST);
	sendDataToOpenGL();
	installShaders();
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
}

void MeGlWindow::paintGL()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, width(), height());

	mat4 projectionMatrix = glm::perspective(60.0f, (float)width() / height(), 0.1f, 10.0f);

	mat4 scalingMatrix =
		glm::scale(vec3(SCALE_IPHONE_WIDTH(), SCALE_IPHONE_HEIGHT(), SCALE_IPHONE_DEPTH()));
	
	// front of the phone
	glUseProgram(programID);
	glBindVertexArray(cubeVertexArrayObjectID);
	
	mat4 translationMatrix = glm::translate(vec3(-0.75f, 0.0f, -2.0f));
	mat4 rotationMatrix = glm::rotate(0.0f, vec3(1.0f, 0.0f, 0.0f));
	mat4 fullTransformMatrix = projectionMatrix * translationMatrix * rotationMatrix * scalingMatrix;

	GLint fullTransformMatrixUniformLocation = glGetUniformLocation(programID, "fullTransformMatrix");
	glUniformMatrix4fv(fullTransformMatrixUniformLocation, 1, GL_FALSE, &fullTransformMatrix[0][0]);

	loadTexture("./Resources/Images/front.png");
	glDrawElements(GL_QUADS, cubeNumIndices, GL_UNSIGNED_SHORT, (void*)cubeIndexByteOffset);
	
	// back of the phone
	translationMatrix = glm::translate(vec3(0.75f, 0.0f, -2.0f));
	rotationMatrix = glm::rotate(0.0f, vec3(0.0f, 1.0f, 0.0f));
	fullTransformMatrix = projectionMatrix * translationMatrix * rotationMatrix * scalingMatrix;

	glUniformMatrix4fv(fullTransformMatrixUniformLocation, 1, GL_FALSE, &fullTransformMatrix[0][0]);

	loadTexture("./Resources/Images/back.png");
	glDrawElements(GL_QUADS, cubeNumIndices, GL_UNSIGNED_SHORT, (void*)cubeIndexByteOffset);

	// visitor sphere
	glUseProgram(sphereProgramID);
	glBindVertexArray(sphereVertexArrayObjectID);

	translationMatrix = glm::translate(vec3(0.0f, 0.0f, -3.0f));
	scalingMatrix = glm::scale(vec3(0.05f, 0.05f, 0.05f));
	fullTransformMatrix = projectionMatrix * translationMatrix * scalingMatrix;

	fullTransformMatrixUniformLocation = glGetUniformLocation(sphereProgramID, "fullMatrix");
	glUniformMatrix4fv(fullTransformMatrixUniformLocation, 1, GL_FALSE, &fullTransformMatrix[0][0]);

	glDrawElements(GL_TRIANGLES, sphereNumIndices, GL_UNSIGNED_SHORT, (void*)sphereIndexByteOffset);
}
