// ConsoleApplication10.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#pragma comment(lib, "C:\\Users\\Alex\\source\\repos\\ConsoleApplication10\\ConsoleApplication10\\lib\\SOIL.lib")

#include <iostream>
#include "include/GL/freeglut.h"
#include "include/GL/glut.h"
#include "C:\Program Files (x86)\Windows Kits\10\Include\10.0.18362.0\um\gl\GL.h";
#include "include/glm/vec3.hpp" // glm::vec3
#include "include/glm/vec4.hpp" // glm::vec4
#include "include/glm/mat4x4.hpp" // glm::mat4
#include "include/glm/ext/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale
#include "include/glm/ext/matrix_clip_space.hpp" // glm::perspective
#include "C:\Users\Alex\source\repos\ConsoleApplication10\ConsoleApplication10\OBJ_Loader.h"
#include <thread>
#include <vector>
#include "include/glm/gtc/quaternion.hpp"
#include "include/glm/gtx/rotate_vector.hpp"
#define DebugMessage(str)	OutputDebugString(str)

#include "C:\Users\Alex\source\repos\ConsoleApplication10\ConsoleApplication10\include\GLFW\glfw3.h"
//#include "include/GLFW/glfw3.h"
//#include "include/GLFW/glfw3native.h"
//- L. "C:\Users\Alex\source\repos\ConsoleApplication10\ConsoleApplication10\include\src\libSOIL.a"
using namespace glm;
using namespace std;
using namespace objl;

//#include "include/glm/ext/constants.hpp" // glm::pi
#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include "C:\Users\Alex\source\repos\ConsoleApplication10\ConsoleApplication10\tiny_obj_loader.h"
#include "ConsoleApplication10.h"
objl::Loader loader;

GLfloat delta = 0;

class Camera {
public:
	glm::quat camera_quat;
	GLfloat x = 0.139598891, y = 4, z = -9;
	GLfloat centerx = -0.0174522363, centery = -0.409999996, centerz = 1;
	GLfloat rotx = -0.0348997228, roty = 0, rotz = 1;
};

class Animation {
protected:
	float local_t = 0;
	bool finished = false;
public:
	std::vector <Object3D>* object;
	Animation(std::vector <Object3D>* model, bool started = false) {
		finished = !started; 
	};

	virtual void step(float t) {
	}
	virtual void step() {
	}
	void restart() {
		local_t = 0;
		finished = false;
	}
	bool isFinished() {
		return finished;
	}
};

class Run : public Animation {
public:
	std::vector <Object3D>* object;
	Run(std::vector <Object3D>* model, bool started = false) :Animation(model) {
		object = model;
	};
	void step(float t) {}
	void step() {
		if (!finished) {
			for (int k = 0; k < object->size(); k++)
			{
				for (int i = 0; i < object->at(k).vertex.size(); i++)
				{
					for (int j = 0; j < object->at(k).vertex.at(i).size(); j++)
					{
						glm::mat4 myMatrix = glm::translate(glm::mat4(1.f), glm::vec3(0.0f, 0.0f, .5f));

						glm::vec4 myVector(object->at(k).vertex.at(i).at(j).Position.X, object->at(k).vertex.at(i).at(j).Position.Y, object->at(k).vertex.at(i).at(j).Position.Z, 1.0f);
						glm::vec4 transformedVector = myMatrix * myVector;

						object->at(k).vertex.at(i).at(j).Position.X = transformedVector.x;
						object->at(k).vertex.at(i).at(j).Position.Y = transformedVector.y;
						object->at(k).vertex.at(i).at(j).Position.Z = transformedVector.z;

						if (transformedVector.y <= 0) {
							//finished = true;
						}

					}
				}
			}
			local_t += 1;
		}
		finished = true;
	}
};

class Jump : public Animation {
public:
	std::vector <Object3D>* object;
	Jump(std::vector <Object3D>* model, bool started = false) :Animation(model, started = false) {
		object = model;
	};
	void step(float t) {}
	void step() {
		if (!finished) {
			for (int k = 0; k < object->size(); k++)
			{
				for (int i = 0; i < object->at(k).vertex.size(); i++)
				{
					for (int j = 0; j < object->at(k).vertex.at(i).size(); j++)
					{
						glm::mat4 myMatrix = glm::translate(glm::mat4(1.f), glm::vec3(0.0f, (-pow(local_t - sqrt(1000), 2) / 1000.f + 1) - (-pow((local_t - 1) - sqrt(1000), 2) / 1000.f + 1), 0.0f));

						glm::vec4 myVector(object->at(k).vertex.at(i).at(j).Position.X, object->at(k).vertex.at(i).at(j).Position.Y, object->at(k).vertex.at(i).at(j).Position.Z, 1.0f);
						glm::vec4 transformedVector = myMatrix * myVector;
						//if (finished == false and transformedVector.y > 0) {
						object->at(k).vertex.at(i).at(j).Position.X = transformedVector.x;
						object->at(k).vertex.at(i).at(j).Position.Y = transformedVector.y;
						object->at(k).vertex.at(i).at(j).Position.Z = transformedVector.z;
						//}
						if (transformedVector.y <= 0) {
							finished = true;
						}
					}
				}

			}
			local_t += 3;
		}
	}
};

struct point3d {
	float x, y, z;
};

class Mario {
private:
	std::vector <Object3D>* mesh;
	Animation* run_anim;
	Animation* jmp_anim;
public:
	Mario(std::vector <Object3D>* model) {
		mesh = model;
		run_anim = new Run(model);
		jmp_anim = new Jump(model);
	}
	void step() {
		run_anim->step();
		jmp_anim->step();
	}
	void jump() {
		if (this->isJumped())
			jmp_anim->restart();
	}
	void run() {
			run_anim->restart();
	}
	bool isJumped() {
		return jmp_anim->isFinished();
	}
	bool isRunning() {
		return run_anim->isFinished();
	}
	std::vector <Object3D>* getMesh() {
		return mesh;
	}
};


class Scene {
public:
	Camera* camera = new Camera();
	// some objects, lights, etc
	vector<std::vector <Object3D>*> RenderObjects;
	//vector<PointLight> Pointlights;
};

float speedx = 0;

struct coordinate {
	float x, y, z;
	coordinate(float a, float b, float c) : x(a), y(b), z(c) {};
};

//for faces, it can contain triangles and quads as well, the four variable contain which is that
struct face {
	int facenum;
	bool four;
	int faces[4];
	face(int facen, int f1, int f2, int f3) : facenum(facen) {  //constructor for triangle
		faces[0] = f1;
		faces[1] = f2;
		faces[2] = f3;
		four = false;
	}
	face(int facen, int f1, int f2, int f3, int f4) : facenum(facen) { //overloaded constructor for quad
		faces[0] = f1;
		faces[1] = f2;
		faces[2] = f3;
		faces[3] = f4;
		four = true;
	}
};

float angle = 0.0;        //we rotate or object with angle degrees



Scene* scene = new Scene();

void renderScene() {
	//while (true) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glLoadIdentity();
	//glTranslatef(0.2, 0.0, 1.0);
	//while (true) {
	glBegin(GL_TRIANGLES);
	glVertex3f(-0.5, -0.5, 0.0);
	glVertex3f(0.0, 0.5, 0.0);
	glVertex3f(0.5, -0.5, 0.0);

	glEnd();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(130, 1, 50, 0);

	glutSwapBuffers();
	//}
	//}
}



float offset = 0;
bool loaded = false;

double x, y;

Mario* mario;
std::vector<Object3D> Brick;
std::vector<std::vector<Object3D>*> Road;

void mapGen() {
	for (int z = 0; z < 5; z++)
	{
		std::vector<Object3D>* tmpbrick = new std::vector<Object3D>();
		*tmpbrick = Brick;

		for (int i = 0; i < tmpbrick->size(); i++)
		{
			for (int j = 0; j < tmpbrick->at(i).vertex.size(); j++)
			{
				for (int k = 0; k < tmpbrick->at(i).vertex.at(j).size(); k++)
				{
					glm::mat4 myMatrix = glm::translate(glm::mat4(1.f), glm::vec3(0.0f, 0.0f, 5.0f * z));

					glm::vec4 myVector(tmpbrick->at(i).vertex.at(j).at(k).Position.X, tmpbrick->at(i).vertex.at(j).at(k).Position.Y, tmpbrick->at(i).vertex.at(j).at(k).Position.Z, 1.0f);
					glm::vec4 transformedVector = myMatrix * myVector;

					tmpbrick->at(i).vertex.at(j).at(k).Position.X = transformedVector.x;
					tmpbrick->at(i).vertex.at(j).at(k).Position.Y = transformedVector.y;
					tmpbrick->at(i).vertex.at(j).at(k).Position.Z = transformedVector.z;
					

				}
			}
		}
		scene->RenderObjects.push_back(tmpbrick);
	}
}

void display(void)
{


	if (!loaded) {
		std::vector <Object3D>* tmpobject3D = new std::vector < Object3D>();
		loader.LoadFile("lowmario.obj", tmpobject3D);
		mario = new Mario(tmpobject3D);
		scene->RenderObjects.push_back(mario->getMesh());

		tmpobject3D = new std::vector <Object3D>();
		loader.LoadFile("brick.obj", tmpobject3D);
		Brick = *tmpobject3D;
		//scene->RenderObjects.push_back(&Brick);
		mapGen();


		loaded = true;
	}
	mario->step();
	//scene->Objects.push_back(Object());
	//loader.LoadedVertices.size();

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LESS);




	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();


	int height = glutGet(GLUT_WINDOW_HEIGHT);
	int width = glutGet(GLUT_WINDOW_WIDTH);
	gluPerspective(60.0f, (double)width / (double)height, 1.0f, 300.0f);

	//glMatrixMode(GL_MODELVIEW);
	//glPushMatrix();
	//glLoadMatrixf(transform);
	//glRotatef(deg, x, y, z);
	//glGetFloatv(GL_MODELVIEW_MATRIX, transform);
	//glPopMatrix();


	glm::mat4 myMatrix;
	glm::vec4 myVector;

	//GLfloat* matr = new GLfloat();
	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
	//glPushMatrix(); // put current matrix on stack
	//glTranslatef(scene->camera->rotx, scene->camera->roty, scene->camera->rotz);
	//glRotatef(5, 1.0f, 1.0f, 1.0f); // rotate the robot on its y-axis
	//glPopMatrix();

	//scene->camera->rotx = scene->camera->rotx * sin(10 / 2);


	float key_pitch = 5, key_yaw = 0, key_roll = 0;
	//temporary frame quaternion from pitch,yaw,roll 
//here roll is not used
	glm::quat key_quat = glm::quat(glm::vec3(key_pitch, key_yaw, key_roll));
	//reset values
	key_pitch = key_yaw = key_roll = 0;

	//order matters,update camera_quat
	scene->camera->camera_quat = key_quat * scene->camera->camera_quat;
	scene->camera->camera_quat = glm::normalize(scene->camera->camera_quat);
	glm::mat4 rotate = glm::mat4_cast(scene->camera->camera_quat);

	glm::mat4 translate = glm::mat4(1.0f);
	glm::vec3 eyeVector(0, 0, 0);
	translate = glm::translate(translate, -eyeVector);

	glm::mat4 viewMatrix = rotate * translate;


	//float tmprotx = scene->camera->rotx;
	//float tmproty = cos(glm::radians(5.0f)) * scene->camera->roty - sin(glm::radians(5.0f)) * scene->camera->rotz;
	//float tmprotz = sin(glm::radians(5.0f)) * scene->camera->roty + cos(glm::radians(5.0f)) * scene->camera->rotz;


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(scene->camera->x, scene->camera->y, scene->camera->z, scene->camera->x + scene->camera->rotx, scene->camera->y + scene->camera->roty, scene->camera->z + scene->camera->rotz, 0, 1, 0);



	//offset += 1.0002;
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);



	//glEnable(GL_TEXTURE_2D);
	//glLoadIdentity();
	//glTranslatef(offset, 0.0, 0);
	for (int i = 0; i < scene->RenderObjects.size(); i++)
	{

		for (int g = 0; g < scene->RenderObjects.at(i)->size(); g++)
		{



			for (int k = 0; k < scene->RenderObjects.at(i)->at(g).vertex.size(); k++)
			{
				glMatrixMode(GL_MODELVIEW);

				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, scene->RenderObjects.at(i)->at(g).texture);

				glBegin(GL_POLYGON);

				for (int j = 0; j < scene->RenderObjects.at(i)->at(g).vertex.at(k).size(); j++)
				{
					//if (objects3D->at(i).texture != -1) {

					glTexCoord2d(scene->RenderObjects.at(i)->at(g).vertex.at(k).at(j).TextureCoordinate.X, scene->RenderObjects.at(i)->at(g).vertex.at(k).at(j).TextureCoordinate.Y);

					//}
					//else {
						//glDisable(GL_TEXTURE_2D);
						//glColor3f(255, 255, 255);

					//}
					glVertex3f(scene->RenderObjects.at(i)->at(g).vertex.at(k).at(j).Position.X, scene->RenderObjects.at(i)->at(g).vertex.at(k).at(j).Position.Y, scene->RenderObjects.at(i)->at(g).vertex.at(k).at(j).Position.Z);

					//loadTexture(loader.LoadedMaterials[k].map_Kd.c_str());
					//glLoadIdentity();
					//loadTexture();
				}
				glEnd();
				glDisable(GL_TEXTURE_2D);

			}

		}
		//glBindTexture(GL_TEXTURE_2D, 0); // << default texture object


	}

	glutSwapBuffers();
	glutPostRedisplay();
}

void keyPressed(unsigned char key, int x, int y) {
	if (key == 'w') {
		mario->run();scene->camera->z += 0.5;

		//scene->camera->z += scene->camera->rotz;
		//scene->camera->x += scene->camera->rotx;
	}
	if (key == 'a') {
		//scene->camera->z -= 1 * scene->camera->rotx;
		//scene->camera->x += 1 * scene->camera->rotz;

	}
	if (key == 's') {
		//scene->camera->z -= scene->camera->rotz;
		//scene->camera->x -= scene->camera->rotx;

	}
	if (key == 'd') {
		//scene->camera->z += 1 * scene->camera->rotx;
		//scene->camera->x -= 1 * scene->camera->rotz;
	}
	if (key == 'q') {
		glm::vec3 tmpvec = vec3(scene->camera->rotx, scene->camera->roty, scene->camera->rotz);
		glm::vec3 rotvec = glm::rotateY(tmpvec, glm::radians(2.0f));
		scene->camera->rotx = rotvec.x;
		scene->camera->roty = rotvec.y;
		scene->camera->rotz = rotvec.z;
	}
	if (key == 'e') {
		glm::vec3 tmpvec = vec3(scene->camera->rotx, scene->camera->roty, scene->camera->rotz);
		glm::vec3 rotvec = glm::rotateY(tmpvec, glm::radians(-2.0f));
		scene->camera->rotx = rotvec.x;
		scene->camera->roty = rotvec.y;
		scene->camera->rotz = rotvec.z;
	}
	if (key == 'z') {
		scene->camera->y += 1;
	}
	if (key == 'x') {
		scene->camera->y -= 1;
	}
	if (key == 'c') {
		std::vector <std::vector<Vertex>> LoadedFaces;
		Object3D obj = Object3D(LoadedFaces, 1);
	}
	if (key == 32) {
 		mario->jump();

	}
}

int lastx = -1;;

void My_mouse_routine(int x, int y)
{
	if (lastx == -1) lastx = x;
	//scene->camera->roty = (-(-200+y)/400.0)*4;

	glm::vec3 tmpvec = vec3(scene->camera->rotx, scene->camera->roty, scene->camera->rotz);
	glm::vec3 rotvec = glm::rotateY(tmpvec, glm::radians((float)((lastx - x))));
	//scene->camera->rotx = rotvec.x;
	//scene->camera->roty = rotvec.y;
	//scene->camera->rotz = rotvec.z;
	lastx = x;
}

class Vector3D {
public:
	Vector3D(double x, double y, double z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}

	double dot(const Vector3D& rhs) const {
		return x * rhs.x + y * rhs.y + z * rhs.z;
	}

	Vector3D operator-(const Vector3D& rhs) const {
		return Vector3D(x - rhs.x, y - rhs.y, z - rhs.z);
	}

	Vector3D operator*(double rhs) const {
		return Vector3D(rhs * x, rhs * y, rhs * z);
	}

	friend std::ostream& operator<<(std::ostream&, const Vector3D&);

private:
	double x, y, z;
};

std::ostream& operator<<(std::ostream& os, const Vector3D& f) {
	std::stringstream ss;
	ss << "(" << f.x << ", " << f.y << ", " << f.z << ")";
	return os << ss.str();
}

Vector3D intersectPoint(Vector3D rayVector, Vector3D rayPoint, Vector3D planeNormal, Vector3D planePoint) {
	Vector3D diff = rayPoint - planePoint;
	double prod1 = diff.dot(planeNormal);
	double prod2 = rayVector.dot(planeNormal);
	double prod3 = prod1 / prod2;
	return rayPoint - rayVector * prod3;
}

int test() {
	Vector3D rv = Vector3D(0.0, -1.0, -1.0);
	Vector3D rp = Vector3D(0.0, 0.0, 10.0);
	Vector3D pn = Vector3D(0.0, 0.0, 1.0);
	Vector3D pp = Vector3D(0.0, 0.0, 5.0);
	Vector3D ip = intersectPoint(rv, rp, pn, pp);

	std::cout << "The ray intersects the plane at " << ip << std::endl;

	return 0;
}


int main(int argc, char** argv)
{
	test();


	// инициализация
	glutInit(&argc, argv);
	//glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

	glutInitWindowPosition(100, 100);
	glutInitWindowSize(400, 400);
	glutCreateWindow("rgz");



	// регистрация обратных вызовов
	glutKeyboardFunc(keyPressed);
	glutDisplayFunc(display);

	glutPassiveMotionFunc(My_mouse_routine);

	// Основной цикл GLUT
	glutMainLoop();



}