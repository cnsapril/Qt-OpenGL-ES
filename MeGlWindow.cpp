#include <MeGlWindow.h>

#define IPHONE_WIDTH 77.9
#define IPHONE_HEIGHT 158.2
#define IPHONE_DEPTH 7.3

#define SCALE_IPHONE_WIDTH() 1.0f / IPHONE_HEIGHT * IPHONE_WIDTH
#define SCALE_IPHONE_HEIGHT() 1.0f
#define SCALE_IPHONE_DEPTH() 1.0f / IPHONE_HEIGHT * IPHONE_DEPTH

using glm::vec3;
using glm::mat4;

GLuint cubeNumIndices;
GLuint sphereNumIndices;

GLuint programID;
GLuint sphereProgramID;

GLuint texture2D_front;
GLuint texture2D_back;

GLuint cubeVertexBuffer;
GLuint cubeIndexBuffer;
GLuint cubeUVBuffer;

GLuint sphereVertexBuffer;
GLuint sphereIndexBuffer;

mat4 projectionMatrix;
mat4 viewMatrix;

void MeGlWindow::sendDataToOpenGL()
{
	ShapeData cube = ShapeGenerator::makeCube();
	ShapeData sphere = ShapeGenerator::makeSphere(40);

	// Generate and bind vertex array object
	GLuint vertexArrayObject;
	glGenVertexArrays(1, &vertexArrayObject);
	glBindVertexArray(vertexArrayObject);

	/* ------------------------ make cube buffers ------------------------ */
	// Generate, bind and send data to cube vertices buffer
	glGenBuffers(1, &cubeVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, cube.vertexBufferSize(), cube.vertices, GL_STATIC_DRAW);

	// Generate, bind, and send data to uv buffer
	glGenBuffers(1, &cubeUVBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, cubeUVBuffer);
	glBufferData(GL_ARRAY_BUFFER, cube.uvBufferSize(), cube.uvs, GL_STATIC_DRAW);

	// Generate, bind and send data to cube indices buffer
	glGenBuffers(1, &cubeIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, cube.indexBufferSize(), cube.indices, GL_STATIC_DRAW);

	// Save the number of indices for later drawing
	cubeNumIndices = cube.numIndices;
	// Clean up the cube ShapeData
	cube.cleanup();

	/* ------------------------ make visitor sphere buffers ------------------------ */
	// Generate, bind and send data to sphere vertices buffer
	glGenBuffers(1, &sphereVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, sphereVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sphere.vertexBufferSize(), sphere.vertices, GL_DYNAMIC_DRAW);

	// Generate, bind and send data to sphere indices buffer (since the sphere doesn't have texture, there's no uv buffer for it)
	glGenBuffers(1, &sphereIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphere.indexBufferSize(), sphere.indices, GL_DYNAMIC_DRAW);

	// Save the number of indices for later drawing
	sphereNumIndices = sphere.numIndices;
	// Clean up the sphere ShapeData
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

	glDetachShader(programID, vertexShaderID);
	glDetachShader(programID, fragmentShaderID);
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

	glDetachShader(sphereProgramID, vertexShaderID);
	glDetachShader(sphereProgramID, fragmentShaderID);
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

void MeGlWindow::convertMat4ToDoubleArray(glm::mat4 mat, HDdouble *doubleArray)
{
	const float *pSource = (const float*)glm::value_ptr(mat);
	for (int i = 0; i < 16; ++i)
	{
		doubleArray[i] = pSource[i];
	}
}

void MeGlWindow::convertDoubleArrayToMat4(HDdouble *doubleArray, glm::mat4* mat)
{
	float pSource[16] = { 0.0 };

	for (int i = 0; i < 16; ++i)
	{
		pSource[i] = (float)doubleArray[i];
	}

	(*mat) = glm::make_mat4(pSource);

}

void MeGlWindow::negateZ(glm::mat4* mat, int column)
{
	for (int i = 0; i < 4; i++)
	{
		(*mat)[i][column] *= -1;
	}
}

HDCallbackCode HDCALLBACK touchScene(void *pUserData)
{
	hdBeginFrame(ghHD);
	hdEndFrame(ghHD);
	return HD_CALLBACK_CONTINUE;
}

HDCallbackCode HDCALLBACK copyHapticDisplayState(void *pUserData)
{
	HapticDisplayState *pState = (HapticDisplayState *)pUserData;

	hdGetDoublev(HD_CURRENT_POSITION, pState->position);
	hdGetDoublev(HD_CURRENT_TRANSFORM, pState->transform);

	return HD_CALLBACK_DONE;
}

void MeGlWindow::initHD()
{
	HDErrorInfo error;
	ghHD = hdInitDevice(HD_DEFAULT_DEVICE);
	if (HD_DEVICE_ERROR(error = hdGetError()))
	{
		hduPrintError(stderr, &error, "Failed to initialize haptic device");
		exit(-1);
	}

	hdEnable(HD_FORCE_OUTPUT);

	hUpdateDeviceCallback = hdScheduleAsynchronous(touchScene, 0, 
		HD_MAX_SCHEDULER_PRIORITY);

	hdStartScheduler();
	if (HD_DEVICE_ERROR(error = hdGetError()))
	{
		hduPrintError(stderr, &error, "Failed to start the scheduler");
		exit(-1);
	}
}

void MeGlWindow::updateWorkspace()
{
	HDdouble screenTworkspace;
	GLdouble modelview[16];
	GLdouble projection[16];
	GLint viewport[4];

	convertMat4ToDoubleArray(viewMatrix, modelview);
	convertMat4ToDoubleArray(projectionMatrix, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);

	hduMapWorkspaceModel(modelview, projection, workspaceModel);

	screenTworkspace = hduScreenToWorkspaceScale(
		modelview, projection, viewport, workspaceModel);

	gCursorScale = kCursorScreenSize * screenTworkspace;
}

MeGlWindow::MeGlWindow()
{
	setMinimumSize(1024, 1024);
	
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
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	sendDataToOpenGL();
	installShaders();
	initHD();
}

void MeGlWindow::paintGL()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	// Common matrices (projection and scaling)
	mat4 scalingMatrix = glm::scale(vec3(SCALE_IPHONE_WIDTH(), SCALE_IPHONE_HEIGHT(), SCALE_IPHONE_DEPTH()));
	
	/* ------------------------ front of the phone ------------------------ */
	// Use the shader for cube rendering with texture
	glUseProgram(programID);
	
	// Individual matrices (translation matrix)
	mat4 translationMatrix = glm::translate(vec3(-0.75f, 0.0f, 0.0f));

	// Send full matrix to vertex shader
	mat4 fullTransformMatrix = projectionMatrix * viewMatrix * translationMatrix * scalingMatrix;
	GLint fullTransformMatrixUniformLocation = glGetUniformLocation(programID, "fullTransformMatrix");
	glUniformMatrix4fv(fullTransformMatrixUniformLocation, 1, GL_FALSE, &fullTransformMatrix[0][0]);
	
	// Load texture for the front of the phone
	loadTexture("./Resources/Images/front.png");

	// Set up array buffer pointer
	glBindBuffer(GL_ARRAY_BUFFER, cubeVertexBuffer);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// Set up UV buffer pointer
	glBindBuffer(GL_ARRAY_BUFFER, cubeUVBuffer);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// Set up index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeIndexBuffer);
	
	// Drawing the front of the phone
	glDrawElements(GL_QUADS, cubeNumIndices, GL_UNSIGNED_SHORT, (void*)0);
	
	/* ------------------------ back of the phone ------------------------*/
	// Individual matrices (translation matrix)
	translationMatrix = glm::translate(vec3(0.75f, 0.0f, 0.0f));
	
	// Send full matrix to the vertex shader
	fullTransformMatrix = projectionMatrix * viewMatrix * translationMatrix * scalingMatrix;
	glUniformMatrix4fv(fullTransformMatrixUniformLocation, 1, GL_FALSE, &fullTransformMatrix[0][0]);

	// Load texture for back of the phone
	loadTexture("./Resources/Images/back.png");
	
	// Draw the back of the phone (vertices, uv and indices have not changed in this case. No need for resetting the buffers)
	glDrawElements(GL_QUADS, cubeNumIndices, GL_UNSIGNED_SHORT, (void*)0);

	// Disable the vertex attribute array location because we are using a different shader for the visitor sphere later
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	/* ------------------------ visitor sphere ------------------------*/
	// Haptics settings first
	HapticDisplayState state;
	hdScheduleSynchronous(copyHapticDisplayState, &state, HD_DEFAULT_SCHEDULER_PRIORITY);

	// Use the shader for sphere rendering without texture
	glUseProgram(sphereProgramID);
	
	// Individual matrices (translation and scale matrix)
	scalingMatrix = glm::scale(vec3(gCursorScale, gCursorScale, gCursorScale));

	glm::mat4 workspaceModelMat = glm::mat4();
	convertDoubleArrayToMat4(workspaceModel, &workspaceModelMat);
	glm::mat4 transformMat = glm::mat4();
	convertDoubleArrayToMat4(state.transform, &transformMat);

	for (int i = 0; i < 3; i++)
	{
		cout << state.position[i] << " ";
	}
	cout << endl;

	// Send full matrix to the vertex shader
	fullTransformMatrix = workspaceModelMat * transformMat * scalingMatrix;
	
	negateZ(&fullTransformMatrix, 2);

	fullTransformMatrixUniformLocation = glGetUniformLocation(sphereProgramID, "fullMatrix");
	glUniformMatrix4fv(fullTransformMatrixUniformLocation, 1, GL_FALSE, &fullTransformMatrix[0][0]);

	// Set up vertices buffer pointer
	glBindBuffer(GL_ARRAY_BUFFER, sphereVertexBuffer);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// Set up indices buffer pointer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereIndexBuffer);

	// Draw the visitor sphere
	glDrawElements(GL_TRIANGLES, sphereNumIndices, GL_UNSIGNED_SHORT, (void*)0);

	// Disable the vertex attribute array location
	glDisableVertexAttribArray(0);
}

void MeGlWindow::resizeGL(int w, int h)
{
	static const double kPI = 3.1415926535897932384626433832795;
	static const double kFovY = 60;

	double nearDist, farDist, aspect;
	glViewport(0, 0, w, h);

	nearDist = 1.0 / tan((kFovY / 2.0) * kPI / 180);
	farDist = nearDist + 2.0f;
	aspect = (GLfloat) w / h;

	cout << "nearDist = " << nearDist << endl;
	cout << "farDist = " << farDist << endl;

	projectionMatrix = glm::perspective(kFovY, aspect, nearDist, farDist);

	viewMatrix = glm::lookAt(
		vec3(0.0f, 0.0f, nearDist + 1.0f),
		vec3(0.0f, 0.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f)
		);

	updateWorkspace();
}