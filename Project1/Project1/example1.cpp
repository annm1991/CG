// ch01.cpp : Defines the entry point for the console application.
//
//////////////////////////////////////////////////////////
//  triangles.cpp  from the OpenGL Red Book   Chapter 1
//////////////////////////////////////////////////////////

#include <iostream>
#include <string>
using namespace std;

#include "vgl.h"
#include "LoadShaders.h"
#define LINE_WIDTH 5.0f

//enum VAO_IDs { Triangles, NumVAOs };
const GLuint Triangles = 0, NumVAOs = 1;
//enum Buffer_IDs { ArrayBuffer, NumBuffers };
const GLuint ArrayBuffer = 0, NumBuffers = 1;
//enum Attrib_IDs { vPosition = 0 };
const GLuint vPosition = 0;
const GLuint VertexPosition = 0;

GLuint VAOs[NumVAOs];

GLuint Buffers[NumBuffers];
GLuint program, program1;

const GLuint NumVertices = 6;

GLuint vaoHandle;
GLuint vboHandles[2];

GLuint vaoHandle_circle[1];
GLuint vboHandle_circle[NumBuffers];

float *circle_coords = NULL;
int steps;

GLfloat rgb[3] = { 0.0f, 0.0f, 1.0f };
GLuint flag_t1 = 1, flag_t2 = 1, flag_c = 0, flag_w = 0;

////////////////////////////////////////////////////////////////////
//	display_circle
////////////////////////////////////////////////////////////////////
void display_circle(void)
{
	if (flag_c == 0){
		glBindVertexArray(0);
		glUseProgram(0);
		glutPostRedisplay();
	}
	else{
		if (flag_w == 0){
			glBindVertexArray(vaoHandle_circle[0]);
			glUseProgram(program);
			GLint loc = glGetUniformLocation(program, "vColor");
			glUniform3fv(loc, 1, rgb);
			glDrawArrays(GL_TRIANGLE_FAN, 0, steps+2);
		}
		else{
			glBindVertexArray(vaoHandle_circle[0]);
			glUseProgram(program);
			glLineWidth(5.0f);
			GLint loc = glGetUniformLocation(program, "vColor");
			glUniform3fv(loc, 1, rgb);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDrawArrays(GL_TRIANGLE_FAN, 0, steps+2);
		}
	}

	glFlush();
}

////////////////////////////////////////////////////////////////////
//	display_shaded
////////////////////////////////////////////////////////////////////
void display_shaded(void)
{
	if (flag_t2 == 0){
		glBindVertexArray(0);
		glUseProgram(0);
		glutPostRedisplay();
	}
	else{
		if (flag_w == 0){
			glBindVertexArray(vaoHandle);
			glUseProgram(program1);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glDrawArrays(GL_TRIANGLES, 0, 3);
		}
		else{
			glBindVertexArray(vaoHandle);
			glUseProgram(program1);
			glLineWidth(LINE_WIDTH);
			glDrawArrays(GL_LINE_LOOP, 0, 3);	/*Can use polygon_mode as well*/
		}
	}

	glFlush();
}

////////////////////////////////////////////////////////////////////
//	display_triangles
////////////////////////////////////////////////////////////////////
void display_triangles(void)
{
	if (flag_t1 == 0){
		glBindVertexArray(0);
		glUseProgram(0);
		glutPostRedisplay();
		//glDrawArrays(GL_POINTS, 0, 0);
	}
	else{
		if (flag_w == 0){
			glBindVertexArray(VAOs[Triangles]);
			glUseProgram(program);
			GLint loc = glGetUniformLocation(program, "vColor");
			glUniform3fv(loc, 1, rgb);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glDrawArrays(GL_TRIANGLES, 0, NumVertices);
		}
		else{
			glBindVertexArray(VAOs[Triangles]);
			glUseProgram(program);
			glLineWidth(LINE_WIDTH);
			GLint loc = glGetUniformLocation(program, "vColor");
			glUniform3fv(loc, 1, rgb);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDrawArrays(GL_TRIANGLES, 0, NumVertices);
		}
	}
	
	glFlush();
}


////////////////////////////////////////////////////////////////////
//	display
////////////////////////////////////////////////////////////////////
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	display_triangles();
	display_shaded();
	display_circle();

	glFlush();
}



/////////////////////////////////////////////////////
//  init
/////////////////////////////////////////////////////
void init(void)
{
	/*glGenVertexArrays(NumVAOs, VAOs);
	glBindVertexArray(VAOs[Triangles]);*/
	//glClear( GL_COLOR_BUFFER_BIT );
	GLfloat vertices[NumVertices][2] = {
		{ -0.90f, -0.9f },	// Triangle 1
		{ 0.85f, -0.9f },
		{ -0.90f, 0.85f },
		{ 0.90f, -0.85f },	// Triangle 2
		{ 0.90f, 0.90f },
		{ -0.85f, 0.90f }
	};

	GLfloat positionData[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f, 0.5f, 0.0f };
	
	GLfloat colorData[] = {
		0.0f, 0.0f, 1.0,
		0.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 0.0f };

	glGenBuffers(NumBuffers, Buffers);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBuffer]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	ShaderInfo  shaders[] = {
		{ GL_VERTEX_SHADER, "triangles.vert" },
		{ GL_FRAGMENT_SHADER, "triangles.frag" },
		{ GL_NONE, NULL }
	};

	program = LoadShaders(shaders);
	/*GLint location = glGetUniformLocation(program, "Color");
	glProgramUniform3fv(program, location, 1, color);*/
	//glUseProgram(program);

	glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(vPosition);

	glDrawArrays( GL_TRIANGLES, 0, NumVertices );
	//glutDisplayFunc( display );

	glGenBuffers(2, vboHandles);
	GLuint positionBufferHandle = vboHandles[0];
	GLuint colorBufferHandle = vboHandles[1];

	// Populate the position buffer
	glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), positionData, GL_STATIC_DRAW);

	// Populate the color buffer
	glBindBuffer(GL_ARRAY_BUFFER, colorBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), colorData, GL_STATIC_DRAW);

	glGenVertexArrays(1, &vaoHandle);
	glBindVertexArray(vaoHandle);

	// Enable the vertex attribute arrays
	glEnableVertexAttribArray(1);  // Vertex position
	glEnableVertexAttribArray(2);  // Vertex color

	// Map index 0 to the position buffer
	glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	// Map index 1 to the color buffer
	glBindBuffer(GL_ARRAY_BUFFER, colorBufferHandle);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	ShaderInfo  shaders1[] = {
		{ GL_VERTEX_SHADER, "shadedtriangle.vert" },
		{ GL_FRAGMENT_SHADER, "shadedtriangle.frag" },
		{ GL_NONE, NULL }
	};

	program1 = LoadShaders(shaders1);
	//glUseProgram(program1);
	//glutDisplayFunc( display1);

}

void create_circle_geometry(float radius, float step_count){
	float angle, inc_angle;
	 steps = (int)step_count;
	angle = 360 / steps;
	inc_angle = 0.0;
	int i, size = (steps + 2) * 2;

	circle_coords = new float[size];
	circle_coords[0] = 0.0;
	circle_coords[1] = 0.0;

	for ( i = 1; i < steps + 1; i++){
		float theta = 2.0f * 3.1415926f * float(i) / float(steps);
		circle_coords[i * 2 + 0] = radius * cos(theta);
		circle_coords[i * 2 + 1] = radius * sin(theta);

		/*circle_coords[i * 2 + 0] = radius * cos(inc_angle * (22 / 7) / 180);
		circle_coords[i * 2 + 1] = radius * sin(inc_angle * (22 / 7 ) / 180);
*/
		cout << "Coordinates at " << inc_angle << ": " << endl;
		inc_angle = inc_angle + angle;

		cout << circle_coords[i * 2 + 0] << " " << circle_coords[i * 2 + 1] << endl;
	}
	
	circle_coords[i * 2 + 0] = circle_coords[2];
	circle_coords[i * 2 + 1] = circle_coords[3];

	cout << circle_coords[5] << " " << circle_coords[i * 2 + 1] << endl;

	glGenBuffers(1, vboHandle_circle);
	glBindBuffer(GL_ARRAY_BUFFER, vboHandle_circle[0]);
	glBufferData(GL_ARRAY_BUFFER, (steps + 2) * 2 * sizeof(float), circle_coords, GL_STATIC_DRAW);

	glGenVertexArrays(1, vaoHandle_circle);
	glBindVertexArray(vaoHandle_circle[0]);

	// Enable the vertex attribute arrays
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vboHandle_circle[0]);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	//cout << angle;
}

////////////////////////////////////////////////////////////////////////
//	toggleKeys
////////////////////////////////////////////////////////////////////////
void toggleKeys(unsigned char key, int x, int y) {
	string value_str;
	string color_str, color, delimiter;
	int pos, index ;
	float geometry[2];
	float angle;

	switch (key){
	case 'q':
	case 27: exit(0);
		break;
	case 'c': //code to change color
		cout << "Enter the color in RGB format:" << endl;
		getline(cin, color_str);
		color_str.append(" ");
		//cout << color_str;
		delimiter = " ";

		pos = 0;
		index = 0;

		while (((pos = color_str.find(delimiter)) != std::string::npos)) {
			color = color_str.substr(0, pos);
			cout << color << endl;
			if (index > 2)
				break;
			rgb[index] = atof(color.c_str());
			index++;
			color_str.erase(0, pos + delimiter.length());
		}
		//cout << color << endl;
		display();
		break;
	case 's': //shaded surface display
		if (flag_w == 1){
			flag_w = 0;
		}
		display();
		break;
	case 'w': //wireframe display
		if (flag_w == 0){
			flag_w = 1;
		}
		display();
		break;
	case 'g': //geometry for circle
		cout << "Enter radius(0.0 to 1.0) and number of steps(integer)" << endl;
		getline(cin, value_str);
		value_str.append(" ");
		delimiter = " ";

		pos = 0;
		index = 0;

		while ((pos = value_str.find(delimiter)) != std::string::npos) {
			color = value_str.substr(0, pos);
			cout << color << endl;
			if (index > 1)
				break;
			geometry[index] = atof(color.c_str());
			index++;
			value_str.erase(0, pos + delimiter.length());
		}

		create_circle_geometry(geometry[0], geometry[1]);
		flag_c = 1;
		display();

		break;
	case 'x': //toggle two triangles
		if (flag_t1 == 1)
		{
			flag_t1 = 0;
		}
		else{
			flag_t1 = 1;	
		}
		/*display_triangles();
		display_shaded();*/
		display();
		break;
	case 'y': //toggle shaded triangle
		if (flag_t2 == 1)
		{
			flag_t2 = 0;
		}
		else{
			flag_t2 = 1;
		}
		/*display_triangles();
		display_shaded();*/
		display();
		break;
	case 'z': //toggle circle
		if (flag_c == 1)
		{
			flag_c = 0;
		}
		else{
			flag_c = 1;
		}
		display();
		break;
	default: break;

	}
}

////////////////////////////////////////////////////////////////////////
//	main
////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(512, 512);
	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);// GLUT_COMPATIBILITY_PROFILE );
	glutCreateWindow(argv[0]);

	glewExperimental = GL_TRUE;	// added for glew to work!
	if (glewInit())
	{
		cerr << "Unable to initialize GLEW ... exiting" << endl;
		exit(EXIT_FAILURE);
	}

	// code from OpenGL 4 Shading Language cookbook, second edition
	const GLubyte *renderer = glGetString(GL_RENDERER);
	const GLubyte *vendor = glGetString(GL_VENDOR);
	const GLubyte *version = glGetString(GL_VERSION);
	const GLubyte *glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
	GLint major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);

	cout << "GL Vendor            :" << vendor << endl;
	cout << "GL Renderer          :" << renderer << endl;
	cout << "GL Version (string)  :" << version << endl;
	cout << "GL Version (integer) :" << major << " " << minor << endl;
	cout << "GLSL Version         :" << glslVersion << endl;
	//
	//GLint nExtensions;
	//glGetIntegerv( GL_NUM_EXTENSIONS, &nExtensions );
	//for ( int i = 0; i < nExtensions; i++ )
	//	cout << glGetStringi( GL_EXTENSIONS, i )  << endl;

	init();
	glutDisplayFunc(display);

	glutKeyboardFunc(toggleKeys);
	glutMainLoop();

	return 0;
}

