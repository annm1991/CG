#include <iostream>
#include <string> 
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include <fstream>
#include <vector>
using namespace std;

#include "vgl.h"
#include "LoadShaders.h"

const GLuint vPosition = 1, vColor = 2, vNormal = 3, vAmbient = 4;
GLuint program;
GLuint *vaoHandle;
GLuint *vertexBufferID, *indexBufferID, *colorBufferID, *normalBufferID, *ambientBufferID;

GLfloat rgb[3] = { 0.9f, 0.9f, 0.9f };
GLdouble xyz[3] = { 0.0f, 0.0f, 0.0f };
GLfloat ambient[3] = { 0.1f, 0.1f, 0.1f };
GLuint flag_w = 0, flag_compute_normals = 0;
GLfloat min_x = 0.0f, max_x = 0.0f, min_y = 0.0f, max_y = 0.0f, min_z = 0.0f, max_z = 0.0f;
GLfloat radius;
GLfloat step = 0;
string filepath;
double x_range, y_range, z_range;
//float vertices[];
//GLushort indices[];

/*Vertex buffers for each object*/
vector<glm::vec3> vec_verts;
vector<glm::vec3> vec_colors;
vector<glm::vec3> vec_normals;
vector<glm::vec3> vec_ambients;
vector<glm::vec3> vec_vns;
vector<int> vec_indices;
glm::vec3 min_x_vector, max_x_vector, min_y_vector, max_y_vector, min_z_vector, max_z_vector;

/*Final buffers used for drawing*/
vector< vector<glm::vec3> > vertex_buffers;
vector< vector<glm::vec3> > color_buffers;
vector< vector<glm::vec3> > ambient_buffers;
vector< vector<glm::vec3> > vertex_normals;
vector< vector<int> > index_buffers;

/*MVP*/
vector<glm::mat4> obj_model;
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;

/*Setting up view matrix*/
glm::vec3 cameraPos;
glm::vec3 cameraTarget;
glm::vec3 cameraDirection;
glm::vec3 up;
glm::vec3 cameraRight;
glm::vec3 cameraUp;

/*Light vector*/
glm::vec3 light = glm::vec3(0.0f, 0.0f, 1.0f);

/*Info for view and projection matrices*/
double frustumTop, frustumRight, frustumLeft, frustumBottom;
double nearPlane;
double farPlane;
double eyex, eyey, eyez;
double focalx, focaly, focalz;

////////////////////////////////////////////////////////////////////
//	display_geometry
////////////////////////////////////////////////////////////////////
void display_geometry(void)
{
	glUseProgram(program);
	//GLint uniform_count;
	//glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &uniform_count);
	//cout << uniform_count << endl;
	//GLchar name[256];
	//GLint  size;
	//GLenum type;
	//glGetActiveUniform(program, 0, 255, NULL, &size, &type, name);
	//cout << name << endl;
	//radius = sqrt(pow(fabs(eyex - focalx), 2) + pow(fabs(eyey - focaly), 2));

	cameraPos = glm::vec3(eyex, eyey, eyez);
	cameraTarget = glm::vec3(focalx, focaly, focalz);
	cameraDirection = glm::normalize(cameraPos - cameraTarget);
	//up = glm::vec3(0.0f, 0.0f, 1.0f);
	cameraRight = glm::normalize(glm::cross(up, cameraDirection));
	cameraUp = glm::cross(cameraDirection, cameraRight);

	projection = glm::frustum(frustumLeft, frustumRight, frustumBottom, frustumTop, nearPlane, farPlane);
	view = glm::lookAt(cameraPos, cameraTarget, cameraUp);

	GLuint modelLoc = glGetUniformLocation(program, "model");

	GLuint viewLoc = glGetUniformLocation(program, "view");

	GLuint projLoc = glGetUniformLocation(program, "projection");

	GLuint lightLoc = glGetUniformLocation(program, "light");

	for (int i = 0; i < vertex_buffers.size(); i++){
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(obj_model[i]));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glUniform3fv(lightLoc, 1, glm::value_ptr(light));
		if (flag_w == 0){
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glBindVertexArray(vaoHandle[i]);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID[i]);
			glDrawElements(GL_TRIANGLES, index_buffers[i].size(), GL_UNSIGNED_INT, 0);
		}
		else{
			glBindVertexArray(vaoHandle[i]);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID[i]);
			glDrawElements(GL_TRIANGLES, index_buffers[i].size(), GL_UNSIGNED_INT, 0);
		}
	}

	glFlush();
}

////////////////////////////////////////////////////////////////////
//	display
////////////////////////////////////////////////////////////////////
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	display_geometry();

	glFlush();
}

/////////////////////////////////////////////////////
//  init
/////////////////////////////////////////////////////
void init(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//model = glm::rotate(model, 55.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	//projection = glm::perspective(45.0f, 1.0f, 0.1f, 100.0f);
	frustumTop = 0.1f, frustumRight = 0.1f, frustumLeft = -0.1f, frustumBottom = -0.1f;
	nearPlane = 0.1f;
	farPlane = 10000.0f;
	step = 1;
	
	if (min_x < 0)
		x_range = (max_x - min_x) / 2;
	else
		x_range = (max_x - min_x) / 2;
	if (min_y < 0)
		y_range = (max_y - min_y) / 2;
	else
		y_range = (max_y - min_y) / 2;
	if (min_z < 0)
		z_range = (max_z - min_z) / 2;
	else
		z_range = (max_z - min_z) / 2;
	eyex = 0; eyey = 0; eyez = 0;
	//eyex = (max_x)* 3, eyey = (max_y)* 3, eyez = (max_z)* 1;
	focalx = x_range / 2; focaly = y_range / 2; focalz = z_range / 2;
	radius = sqrt(pow(fabs(eyex - focalx), 2) + pow(fabs(eyey - focaly), 2) + pow(fabs(eyez - focalz), 2));
	
	//eyex = radius * sin(glm::radians(step));
	//eyey = radius * cos(glm::radians(step));

	cameraPos = glm::vec3(eyex, eyey, eyez);
	cameraTarget = glm::vec3(focalx, focaly, focalz);
	cameraDirection = glm::normalize(cameraPos - cameraTarget);
	up = glm::vec3(0.0f, 0.0f, 1.0f);
	cameraRight = glm::normalize(glm::cross(up, cameraDirection));
	cameraUp = glm::cross(cameraDirection, cameraRight);

	vertexBufferID = new GLuint[vertex_buffers.size()];
	colorBufferID = new GLuint[color_buffers.size()];
	normalBufferID = new GLuint[vertex_normals.size()];
	ambientBufferID = new GLuint[ambient_buffers.size()];
	indexBufferID = new GLuint[index_buffers.size()];
	vaoHandle = new GLuint[vertex_buffers.size()];

	glGenBuffers(vertex_buffers.size(), vertexBufferID);
	glGenBuffers(color_buffers.size(), colorBufferID);
	glGenBuffers(vertex_normals.size(), normalBufferID);
	glGenBuffers(ambient_buffers.size(), ambientBufferID);
	glGenBuffers(index_buffers.size(), indexBufferID);
	glGenVertexArrays(vertex_buffers.size(), vaoHandle);

	for (int i = 0; i < vertex_buffers.size(); i++){

		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID[i]);
		glBufferData(GL_ARRAY_BUFFER, vertex_buffers[i].size() * sizeof(glm::vec3), &vertex_buffers[i][0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, colorBufferID[i]);
		glBufferData(GL_ARRAY_BUFFER, color_buffers[i].size() * sizeof(glm::vec3), &color_buffers[i][0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, normalBufferID[i]);
		glBufferData(GL_ARRAY_BUFFER, vertex_normals[i].size() * sizeof(glm::vec3), &vertex_normals[i][0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, ambientBufferID[i]);
		glBufferData(GL_ARRAY_BUFFER, ambient_buffers[i].size() * sizeof(glm::vec3), &ambient_buffers[i][0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID[i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffers[i].size() * sizeof(unsigned int), &index_buffers[i][0], GL_STATIC_DRAW);

		glBindVertexArray(vaoHandle[i]);

		glEnableVertexAttribArray(vPosition);  // Vertex position
		glEnableVertexAttribArray(vColor);  // Vertex Kd
		glEnableVertexAttribArray(vNormal); // Vertex normal
		glEnableVertexAttribArray(vAmbient); // Vertex Ka

		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID[i]);
		glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

		glBindBuffer(GL_ARRAY_BUFFER, colorBufferID[i]);
		glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

		glBindBuffer(GL_ARRAY_BUFFER, normalBufferID[i]);
		glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

		glBindBuffer(GL_ARRAY_BUFFER, ambientBufferID[i]);
		glVertexAttribPointer(vAmbient, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

		ShaderInfo  shaders[] = {
			{ GL_VERTEX_SHADER, "transform.vert" },
			{ GL_FRAGMENT_SHADER, "transform.frag" },
			{ GL_NONE, NULL }
		};

		program = LoadShaders(shaders);
		glUseProgram(program);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID[i]);
		glDrawElements(GL_TRIANGLES, index_buffers[i].size(), GL_UNSIGNED_INT, BUFFER_OFFSET(0));

	}
}

////////////////////////////////////////////////////////////////////////
//	splKeys
////////////////////////////////////////////////////////////////////////
void splKeys(int key, int x, int y){
	glm::vec3 new_pos;
	float incx, incy, incz;
	switch (key){
	case GLUT_KEY_UP:
		//cout << "Up";
		//nearPlane = nearPlane - 0.1f;
		//farPlane = farPlane + 0.1f;
		//new_pos = cameraDirection - cameraPos;
		new_pos = cameraPos - cameraDirection;
		/*cameraDirection = glm::normalize(cameraPos - cameraTarget);
		cameraRight = glm::normalize(glm::cross(up, cameraDirection));
		cameraUp = glm::cross(cameraDirection, cameraRight);*/
		incx = eyex - new_pos.x;
		incy = eyey - new_pos.y;
		incz = eyez - new_pos.z;

		eyex = new_pos.x;
		eyey = new_pos.y;
		eyez = new_pos.z;
		
		focalx = focalx - incx;
		focaly = focaly - incy;
		focalz = focalz - incz;

		radius = sqrt(pow(fabs(eyex - focalx), 2) + pow(fabs(eyey - focaly), 2) + pow(fabs(eyez - focalz), 2));
		//cout << eyex << " " << eyey << " " << eyez << endl;
		//radius = sqrt(pow(fabs(focalx - eyex), 2) + pow(fabs(focaly - eyey), 2));
		//projection = glm::frustum(frustumLeft, frustumRight, frustumBottom, frustumTop, nearPlane, farPlane);
		display();
		break;
	case GLUT_KEY_DOWN:
		//cout << "Down";
		//nearPlane = nearPlane + 0.1f;
		//farPlane = farPlane - 0.1f;
		//new_pos = cameraDirection + cameraPos;
		new_pos = cameraPos + cameraDirection;
		/*cameraDirection = glm::normalize(cameraPos - cameraTarget);
		cameraRight = glm::normalize(glm::cross(up, cameraDirection));
		cameraUp = glm::cross(cameraDirection, cameraRight);*/
		eyex = new_pos.x;
		eyey = new_pos.y;
		eyez = new_pos.z;
		radius = sqrt(pow(fabs(eyex - focalx), 2) + pow(fabs(eyey - focaly), 2) + pow(fabs(eyez - focalz), 2));
		//cout << eyex << " " << eyey << " " << eyez << endl;
		//radius = sqrt(pow(fabs(focalx - eyex), 2) + pow(fabs(focaly - eyey), 2));
		//projection = glm::frustum(frustumLeft, frustumRight, frustumBottom, frustumTop, nearPlane, farPlane);
		display();
		break;
	case GLUT_KEY_LEFT:
		//cout << "Left";
		//radius = sqrt(pow((eyex - focalx), 2) + pow((eyey - focaly), 2));
		//cout << "radius: " << radius << endl;
		step++;
		//radius = sqrt(pow(fabs(eyex - focalx), 2) + pow(fabs(eyey - focaly), 2) + pow(fabs(eyez - focalz), 2));
		eyex = radius * sin(glm::radians(step));
		eyey = radius * cos(glm::radians(step));
		//cout << eyex << " " << eyey << endl;
		//cameraPos = glm::vec3(eyex, eyey, eyez);
		//cameraDirection = glm::normalize(cameraPos - cameraTarget);
		////up = glm::vec3(0.0f, 0.0f, 1.0f);
		//cameraRight = glm::normalize(glm::cross(up, cameraDirection));
		//cameraUp = glm::cross(cameraDirection, cameraRight);
		display();
		break;
	case GLUT_KEY_RIGHT:
		//cout << "Right";
		step--;
		//radius = sqrt(pow(fabs(focalx - eyex), 2) + pow(fabs(focaly -  eyey), 2));
		//radius = sqrt(pow(fabs(eyex - focalx), 2) + pow(fabs(eyey - focaly), 2) + pow(fabs(eyez - focalz), 2));
		eyex = radius * sin(glm::radians(step));
		eyey = radius * cos(glm::radians(step));
		//cout << eyex << " " << eyey << endl;
		//cameraPos = glm::vec3(eyex, eyey, eyez);
		//cameraDirection = glm::normalize(cameraPos - cameraTarget);
		////up = glm::vec3(0.0f, 0.0f, 1.0f);
		//cameraRight = glm::normalize(glm::cross(up, cameraDirection));
		//cameraUp = glm::cross(cameraDirection, cameraRight);
		display();
		break;
	default:
		break;
	}
}

////////////////////////////////////////////////////////////////////////
//	toggleKeys
////////////////////////////////////////////////////////////////////////
void toggleKeys(unsigned char key, int x, int y) {
	string value_str;
	string color_str, color, delimiter;
	double inc_value;
	int pos, index;
	float geometry[2];
	float angle;

	if (x_range >= y_range && x_range >= z_range)
		inc_value = x_range / 100;
	else if (y_range >= x_range && y_range >= z_range)
		inc_value = y_range / 100;
	else
		inc_value = z_range / 100;

	//glm::mat4 rotationMat = glm::mat4(glm::vec4(cameraDirection.x, 0.0f, 0.0f, 0.0f), glm::vec4(0.0f, cameraDirection.y, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, cameraDirection.z, 0.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	switch (key){
	case 'q':
	case 27: exit(0);
		break;
	case 'r':
		init();
		display();
		break;
	case 'c': //move down the view vector
		eyez = eyez - inc_value;
		//radius = sqrt(pow(fabs(eyex - focalx), 2) + pow(fabs(eyey - focaly), 2) + pow(fabs(eyez - focalz), 2));

		/*cameraPos = glm::vec3(eyex, eyey, eyez);
		cameraDirection = glm::normalize(cameraPos - cameraTarget);
		cameraRight = glm::normalize(glm::cross(up, cameraDirection));
		cameraUp = glm::cross(cameraDirection, cameraRight);*/
		display();
		break;
	case 'v': // move up the view vector
		eyez = eyez + inc_value;
		//radius = sqrt(pow(fabs(eyex - focalx), 2) + pow(fabs(eyey - focaly), 2) + pow(fabs(eyez - focalz), 2));

		/*cameraPos = glm::vec3(eyex, eyey, eyez);
		cameraDirection = glm::normalize(cameraPos - cameraTarget);
		cameraRight = glm::normalize(glm::cross(up, cameraDirection));
		cameraUp = glm::cross(cameraDirection, cameraRight);*/
		display();
		break;
	case 'd': // move focal point opp to view up vector
		focalz = focalz - inc_value;
		radius = sqrt(pow(fabs(eyex - focalx), 2) + pow(fabs(eyey - focaly), 2) + pow(fabs(eyez - focalz), 2));
		/*cameraTarget = glm::vec3(focalx, focaly, focalz);
		cameraDirection = glm::normalize(cameraPos - cameraTarget);
		cameraRight = glm::normalize(glm::cross(up, cameraDirection));
		cameraUp = glm::cross(cameraDirection, cameraRight);*/
		display();
		break;
	case 'f': //movel focal point in direction of view up vector
		focalz = focalz + inc_value;
		radius = sqrt(pow(fabs(eyex - focalx), 2) + pow(fabs(eyey - focaly), 2) + pow(fabs(eyez - focalz), 2));
		/*cameraTarget = glm::vec3(focalx, focaly, focalz);
		cameraDirection = glm::normalize(cameraPos - cameraTarget);
		cameraRight = glm::normalize(glm::cross(up, cameraDirection));
		cameraUp = glm::cross(cameraDirection, cameraRight);*/
		display();
		break;
	case 'z': //rotate view up counter-clockwise
		up = glm::rotate(up, glm::radians(-1.0f), cameraDirection);
		//cameraUp = glm::rotate(cameraUp, glm::radians(-1.0f), cameraDirection);
		display();
		break;
	case 'x': //rotate view up clockwise
		//cout << "rotate" << endl;
		up = glm::rotate(up, glm::radians(1.0f), cameraDirection);
		//cameraUp = glm::rotate(cameraUp, glm::radians(1.0f), cameraDirection);
		display();
		break;
	case 's': //solid display
		flag_w = 0;
		display();
		break;
	case 'w': //wireframe display
		if (flag_w == 0){
			flag_w = 1;
		}
		display();
		break;
	default:
		break;
	}
}

void push_into_vec_color(glm::vec3 insert_what, int index){
	if (vec_colors.size() < index)
		vec_colors.push_back(insert_what);
	else
		vec_colors[index - 1] = insert_what;
}

void push_into_vec_ambients(glm::vec3 insert_what, int index){
	if (vec_ambients.size() < index)
		vec_ambients.push_back(insert_what);
	else
		vec_ambients[index - 1] = insert_what;
}

void push_into_vec_normals(glm::vec3 insert_what, int index){
	if (vec_normals.size() < index){
		vec_normals.resize(index);
		vec_normals[index - 1] = glm::normalize(insert_what);
	}
	else
		vec_normals[index - 1] = glm::normalize(vec_normals[index - 1] + insert_what);
}

////////////////////////////////////////////////////////////////////////
//	parsing faces
////////////////////////////////////////////////////////////////////////
void parse_f(int index, string token, glm::vec3 color, glm::vec3 ambients){
	if (token.find("//") != std::string::npos){
		int norm_index = atoi((token.substr(token.find_last_of("/") + 1)).c_str()) - 1;
		//cout << "vn present"<<endl;
		//cout << "vn val is: " << index << " " << vec_vns[atoi(token.c_str()) - 1].x << endl;
		//cout << index << " " << norm_index;
		glm::vec3 normal = glm::vec3(vec_vns[norm_index].x, vec_vns[norm_index].y, vec_vns[norm_index].z);
		push_into_vec_normals(normal, index + 1);
	}
	else if (token.find("/") != std::string::npos){
		//cout << "vt, vn present"<<endl;
		//cout << "vn value is :" << token.substr(token.find_last_of("/") + 1)  << endl;
		if (token.find_last_of("/") != token.find_first_of("/")){
			int norm_index = atoi((token.substr(token.find_last_of("/") + 1)).c_str()) - 1;
			//cout << index << " " << norm_index;
			glm::vec3 normal = glm::vec3(vec_vns[norm_index].x, vec_vns[norm_index].y, vec_vns[norm_index].z);
			push_into_vec_normals(normal, index + 1);
		}
		else{
			flag_compute_normals = 1;
		}
	}
	else{
		//cout << "have to compute normals" << endl;
		flag_compute_normals = 1;
	}
	vec_indices.push_back(atoi(token.c_str()) - 1);
	push_into_vec_color(color, atoi(token.c_str()));
	push_into_vec_ambients(ambients, atoi(token.c_str()));
}

////////////////////////////////////////////////////////////////////////
//	compute normals for faces
////////////////////////////////////////////////////////////////////////
void compute_normals(){

	int i1, i2, i3;	// indices for points on a face
	i1 = vec_indices[vec_indices.size() - 3];
	i2 = vec_indices[vec_indices.size() - 2];
	i3 = vec_indices[vec_indices.size() - 1];

	glm::vec3 p1, p2, p3;	// vertices on a face
	p1 = vec_verts[i1];
	p2 = vec_verts[i2];
	p3 = vec_verts[i3];

	glm::vec3 v1, v2, vn;	//vectors on a face
	v1 = p2 - p1;
	v2 = p3 - p1;
	//cout << glm::cross(v1, v2).z;
	vn = glm::normalize(glm::cross(v1, v2)); // face normal for p1, p2, p3
	push_into_vec_normals(vn, i1 + 1);
	push_into_vec_normals(vn, i2 + 1);
	push_into_vec_normals(vn, i3 + 1);

	//cout << "That's a face"<<endl;
}

////////////////////////////////////////////////////////////////////////
//	finding bounds of geometry
////////////////////////////////////////////////////////////////////////
void find_min_max(){
	/*if (vertex_buffers.size() == 0 && vec_verts.size() == 1){
		min_x = max_x = pos.x;
		min_y = max_y = pos.y;
		min_z = max_z = pos.z;
		}
		else{
		if (pos.x < min_x){
		min_x = pos.x;
		min_x_vector = pos;
		}
		else if (pos.x > max_x){
		max_x = pos.x;
		max_x_vector = pos;
		}
		if (pos.y < min_y){
		min_y = pos.y;
		min_y_vector = pos;
		}
		else if (pos.y > max_y){
		max_y = pos.y;
		max_y_vector = pos;
		}
		if (pos.z < min_z){
		min_z = pos.z;
		min_z_vector = pos;
		}
		else if (pos.z > max_z){
		max_z = pos.z;
		max_z_vector = pos;
		}
		}*/
	for (int i = 0; i < vertex_buffers.size(); i++){
		glm::mat4 model_matrix = obj_model[i];
		vector<glm::vec3> verts = vertex_buffers[i];
		for (int j = 0; j < verts.size(); j++){
			verts[j] = glm::mat3(model_matrix) * verts[j];
			if (verts[j].x < min_x)
				min_x = verts[j].x;
			else if (verts[j].x > max_x)
				max_x = verts[j].x;
			if (verts[j].y < min_y)
				min_y = verts[j].y;
			else if (verts[j].y > max_y)
				max_y = verts[j].y;
			if (verts[j].z < min_z)
				min_z = verts[j].z;
			else if (verts[j].z > max_z)
				max_z = verts[j].z;
		}
	}
}

////////////////////////////////////////////////////////////////////////
//	parsing obj file
////////////////////////////////////////////////////////////////////////
void parseGeometry(string gfname){
	char buf[100], buf1[100], file_name[100];
	string token, token1;
	std::fstream f_geom;
	std::fstream f_col;
	string mtllib;
	string open_mtllib;
	int i;
	//std::fstream f_color;
	filepath = gfname.substr(0, gfname.find_last_of("/"));

	f_geom.open(gfname.c_str(), std::fstream::in);
	if (!f_geom.good()){
		cerr <<"OBJ file not found!" << endl;
		return;
	}

	while (!f_geom.eof()){
		string color;
		f_geom.getline(buf, 100);
		//cout << buf << endl;
		if (strcmp(buf, " ") == 0 || strcmp(buf, "") == 0)
			continue;
		token = strtok(buf, " ");
		if (strcmp(token.c_str(), "mtllib") == 0){
			mtllib = strtok(NULL, " ");
			filepath = filepath + "/" + mtllib;	
			for(i = 0; i < strlen(filepath.c_str()); i++){
				file_name[i] = filepath[i];
			}
		}
		else if (strcmp(token.c_str(), "v") == 0){

			token = strtok(NULL, " ");
			xyz[0] = atof(token.c_str());
			token = strtok(NULL, " ");
			xyz[1] = atof(token.c_str());
			token = strtok(NULL, " ");
			xyz[2] = atof(token.c_str());
			glm::vec3 pos = glm::vec3(xyz[0], xyz[1], xyz[2]);
			vec_verts.push_back(pos);
			//find_min_max(pos);
		}
		else if (strcmp(token.c_str(), "vn") == 0){
			token = strtok(NULL, " ");
			xyz[0] = atof(token.c_str());
			token = strtok(NULL, " ");
			xyz[1] = atof(token.c_str());
			token = strtok(NULL, " ");
			xyz[2] = atof(token.c_str());
			glm::vec3 pos = glm::vec3(xyz[0], xyz[1], xyz[2]);
			vec_vns.push_back(pos);
		}
		else if (strcmp(token.c_str(), "vt") == 0){
			//do nothing for now
		}
		else if (strcmp(token.c_str(), "f") == 0){

			glm::vec3 color = glm::vec3(rgb[0], rgb[1], rgb[2]);
			glm::vec3 ambients = glm::vec3(ambient[0], ambient[1], ambient[2]);
			token = strtok(NULL, " ");
			int index = atoi(token.c_str()) - 1;
			parse_f(index, token, color, ambients);
			token = strtok(NULL, " ");
			index = atoi(token.c_str()) - 1;
			parse_f(index, token, color, ambients);
			token = strtok(NULL, " ");
			index = atoi(token.c_str()) - 1;
			parse_f(index, token, color, ambients);
			if (flag_compute_normals == 1)
				compute_normals();
		}
		else if (strcmp(token.c_str(), "usemtl") == 0){
			
			string new_mtl;
			color = strtok(NULL, " ");
			//string color_compare = color.substr(0, strlen(color.c_str()));
			//cout << color << strlen(color.c_str()) << endl;
			string file_open;
			if(filepath[strlen(filepath.c_str()) - 1] != 'l')
				file_open = filepath.substr(0, strlen(filepath.c_str()) - 1);
			else
				file_open = filepath;
			//cout<<file_open<<endl;
			f_col.open(file_open.c_str(), std::fstream::in);
			if (!f_col.good()){
				cerr << "Material file not found!" << endl;
				//f_geom.close();
				return;
			}

			while (!f_col.eof()){
				f_col.getline(buf1, 100);
				//cout<<buf1<<endl;
				if (strcmp(buf1, "") == 0 || strcmp(buf1, " ") == 0)
					continue;
				token1 = strtok(buf1, " ");
				if (strcmp(token1.c_str(), "newmtl") == 0){
					new_mtl = strtok(NULL, " ");
					//cout << "new_mtl: " << new_mtl << endl;
				}
				else if (strcmp(new_mtl.c_str(), color.c_str()) == 0){
					//cout << "token1: " << token1 << endl;					
					if (strcmp(token1.c_str(), "Kd") == 0){
						rgb[0] = atof(strtok(NULL, " "));
						rgb[1] = atof(strtok(NULL, " "));
						rgb[2] = atof(strtok(NULL, " "));
						//cout << rgb[0] << " " << rgb[1] << " " << rgb[2] << endl;

					}
					if (strcmp(token1.c_str(), "Ka") == 0){
						ambient[0] = atof(strtok(NULL, " "));
						ambient[1] = atof(strtok(NULL, " "));
						ambient[2] = atof(strtok(NULL, " "));
					}
				}
			}
			//cout<<"End of mtl file"<<endl;
			f_col.close();
		}
	}
	//cout<<"End of obj file"<<endl;
	f_geom.close();
}

////////////////////////////////////////////////////////////////////////
//	pushing one object's geometry
////////////////////////////////////////////////////////////////////////
void push_obj_geometry(){
	/*int size = vertex_buffers.size();
	for (int i = 0; i < vec_verts.size(); i++){
	vertex_buffers.push_back(vec_verts[i]);
	vertex_normals.push_back(vec_normals[i]);
	color_buffers.push_back(vec_colors[i]);
	ambient_buffers.push_back(vec_ambients[i]);
	}
	for (int j = 0; j < vec_indices.size(); j++){
	index_buffers.push_back(vec_indices[j] + size);
	}*/
	vertex_buffers.push_back(vec_verts);
	vertex_normals.push_back(vec_normals);
	color_buffers.push_back(vec_colors);
	ambient_buffers.push_back(vec_ambients);
	index_buffers.push_back(vec_indices);
	vec_verts.clear();
	vec_colors.clear();
	vec_ambients.clear();
	vec_normals.clear();
	vec_indices.clear();
}


//void push_into_vec_matrix(glm::mat4 insert_model, int index){
//	if (obj_model.size() < index){
//		obj_model.resize(index);
//		obj_model[index - 1] = insert_model;
//	}
//	else
//		obj_model[index - 1] = obj_model[index - 1] * insert_model;
//}


////////////////////////////////////////////////////////////////////////
//	parsing control file
////////////////////////////////////////////////////////////////////////
void parseControlFile(string ctrlfile){
	string token;
	std::fstream f_ctrl;
	char buf[100];
	int n;
	int index = 0;
	f_ctrl.open(ctrlfile.c_str(), std::fstream::in);
	if (!f_ctrl.good()){
		cerr << "Control file not found!" << endl;
		return;
	}

	while (!f_ctrl.eof()){
		// read an entire line into memory
		f_ctrl.getline(buf, 100);
		//cout << buf << strlen(buf) << endl;
		if (strcmp(buf, "") == 0 || strcmp(buf, " ") == 0)
			continue;
		token = strtok(buf, " ");

		if (strcmp(token.c_str(), "obj") == 0){
			if (vec_verts.size() != 0){
				push_obj_geometry();
				obj_model.push_back(model);
				model = glm::mat4(1.0f);
				//rgb[0] = rgb[1] = rgb[2] = 0.9f;
				//ambient[0] = ambient[1] = ambient[2] = 0.1f;
			}
			index++;
			parseGeometry(strtok(NULL, " "));
			//cout<<"Parsed one geometry"<<endl;
		}
		else if (strcmp(token.c_str(), "t") == 0){
			string tx = strtok(NULL, " ");
			string ty = strtok(NULL, " ");
			string tz = strtok(NULL, " ");
			model = glm::translate(model, glm::vec3(atof(tx.c_str()), atof(ty.c_str()), atof(tz.c_str())));
			//push_into_vec_matrix(&model, index);
		}
		else if (strcmp(token.c_str(), "s") == 0){
			string sx = strtok(NULL, " ");
			string sy = strtok(NULL, " ");
			string sz = strtok(NULL, " ");
			model = glm::scale(model, glm::vec3(atof(sx.c_str()), atof(sy.c_str()), atof(sz.c_str())));
			//push_into_vec_matrix(model, index);
		}
		else if (strcmp(token.c_str(), "rx") == 0 || strcmp(token.c_str(), "ry") == 0 || strcmp(token.c_str(), "rz") == 0){
			while (strcmp(token.c_str(), "") != 0 || strcmp(token.c_str(), " ") != 0){
				string r = strtok(NULL, " ");
				if (strcmp(token.c_str(), "rx") == 0){
					float angle = atof(r.c_str());
					model = glm::rotate(model, angle, glm::vec3(1.0f, 0.0f, 0.0f));
					//push_into_vec_matrix(model, index);
				}
				else if (strcmp(token.c_str(), "ry") == 0){
					float angle = atof(r.c_str());
					model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
					//push_into_vec_matrix(model, index);
				}
				else if (strcmp(token.c_str(), "rz") == 0){
					float angle = atof(r.c_str());
					model = glm::rotate(model, angle, glm::vec3(0.0f, 0.0f, 1.0f));
					//push_into_vec_matrix(model, index);
				}
				if (strtok(NULL, " ") != NULL)
					token = strtok(NULL, " ");
				else
					break;
			}
		}
	}
	//cout<<"End of control file"<<endl;
	push_obj_geometry();	// for the last obj file read
	//push_into_vec_matrix(model, index);
	obj_model.push_back(model);
	find_min_max();
	f_ctrl.close();
}

////////////////////////////////////////////////////////////////////////
//	maintaining aspect ratio when resizing window
////////////////////////////////////////////////////////////////////////
void changeWindowSize(int width, int height) {
	const float ar_origin = (float)GLUT_WINDOW_WIDTH / (float)GLUT_WINDOW_HEIGHT;
	const float ar_new = (float)width / (float)height;

	float scale_w = (float)width / (float)GLUT_WINDOW_WIDTH;
	float scale_h = (float)height / (float)GLUT_WINDOW_HEIGHT;
	if (ar_new > ar_origin) {
		scale_w = scale_h;
	}
	else {
		scale_h = scale_w;
	}

	float margin_x = (width - GLUT_WINDOW_WIDTH * scale_w) / 2;
	float margin_y = (height - GLUT_WINDOW_HEIGHT * scale_h) / 2;

	glViewport(margin_x, margin_y, GLUT_WINDOW_WIDTH * scale_w, GLUT_WINDOW_HEIGHT * scale_h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, GLUT_WINDOW_WIDTH / ar_origin, 0, GLUT_WINDOW_HEIGHT / ar_origin, 0, 1.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

////////////////////////////////////////////////////////////////////////
//	main
////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH);
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

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	//glEnable(GL_NORMALIZE);
	//glViewport(0, 0, 512, 512);
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
	int count;
	string ctrlfile;
	string token;
	//cout << argc << endl;
	if(argc != 3){
		printf("Usage: %s <controlfilename>, where controlfilename is name of the control file\n", argv[0]);
		exit(1);
	}

	for (count = 0; count < argc; count++){
		if (strcmp(argv[count], "-c") == 0){
			if (argv[count + 1] == NULL){
				cerr << "Invalid arguments!" << endl;
				return -1;
			}
			else{
				ctrlfile = argv[count + 1];
			}
		}
	}

	parseControlFile(ctrlfile);
	init();
	
	glutKeyboardFunc(toggleKeys);
	glutSpecialFunc(splKeys);
	glutReshapeFunc(changeWindowSize);
	glutDisplayFunc(display);
	glutMainLoop();

	return 0;
}

