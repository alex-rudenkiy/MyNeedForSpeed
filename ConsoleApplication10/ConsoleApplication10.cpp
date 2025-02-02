#include <iostream>
#include "include/GL/freeglut.h"
//#include "include/GL/glut.h"
#include "GL\GL.h";
#include "include/glm/vec3.hpp" // glm::vec3
#include "include/glm/vec4.hpp" // glm::vec4
#include "include/glm/mat4x4.hpp" // glm::mat4
#include "include/glm/ext/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale
#include "include/glm/ext/matrix_clip_space.hpp" // glm::perspective
#include "ConsoleApplication10\OBJ_Loader.h"
#include <thread>
#include <vector>
#include "include/glm/gtc/quaternion.hpp"
#include "include/glm/gtx/rotate_vector.hpp"
#include "include/clipper.cpp"
//#define DebugMessage(str)	OutputDebugString(str)

#define BOTS_N 80
#define DRAWCOLLISION 0
#define CAR_TYPES 9
#include "ConsoleApplication10\include\GLFW\glfw3.h"
//#include "include/GLFW/glfw3.h"
//#include "include/GLFW/glfw3native.h"
//- L. "C:\Users\Alex\source\repos\ConsoleApplication10\ConsoleApplication10\include\src\libSOIL.a"
using namespace glm;
using namespace std;
using namespace objl;
using namespace ClipperLib;

//#include "include/glm/ext/constants.hpp" // glm::pi
#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include "ConsoleApplication10\tiny_obj_loader.h"
#include "ConsoleApplication10.h"
#include "GrahamScan.h"
#pragma comment( linker, "/subsystem:\"console\" /entry:\"mainCRTStartup\"" ) 

Loader* loader;
Loader* maploader;

//GLfloat delta = 0;

class Camera {
public:
	glm::quat camera_quat;
	GLfloat x = -18.3440723, y = 9, z = -0.546829224;
	GLfloat centerx = -0.0174522363, centery = -0.409999996, centerz = 1;
	GLfloat rotx = 1.00000119, roty = -0.2, rotz = 0.0348996669;
};

//struct CVertex3 {
//	float x, y, z;
//};
struct CTexmap {         // Структура для хранения текстурных координат
	float u, v;
};
//CVertex3 a;


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
class Ride : public Animation {
public:
	std::vector <Object3D>* object;
	Ride(std::vector <Object3D>* model, bool started = false) :Animation(model) {
		object = model;
		finished = !started;
	};
	//void step(float t) {}
	void step(float angle = 0, float speed = 0) {
		if (!finished) {
			for (int k = 0; k < object->size(); k++)
			{
				for (int i = 0; i < object->at(k).vertex.size(); i++)
				{
					for (int j = 0; j < object->at(k).vertex.at(i).size(); j++)
					{
						glm::mat4 myMatrix = glm::translate(glm::mat4(1.f), glm::vec3(speed, 0.0f, angle));

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
		//finished = true;
	}
};
class Scene {
public:
	Camera* camera = new Camera();
	vector<std::vector <Object3D>*> RenderObjects;
	//vector<PointLight> Pointlights;
};
Scene* scene = new Scene();

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


bool loaded = false;


class Car;

float globalspeed = 0;
std::vector<std::vector <Object3D>> bot;
std::vector <Object3D> road_and_city;
std::vector<std::vector<Object3D>*> EnvMap;
vector<Car*> bots;

class Car {
private:
	Camera* camera = nullptr;
	std::vector <Object3D>* mesh;
	Ride* ride_anim;
	float Xangle = 0;
	float speed = 0;
	float maxspeed = 5;
	std::vector <point2d*> top_collision_rectangle;
	vector<point2d> allpoints;


public:
	Car(std::vector <Object3D>* model, Camera* _camera = nullptr) {
		mesh = model;
		ride_anim = new Ride(model, true);
		camera = _camera;

		for (int i = 0; i < mesh->size(); i++)
		{
			for (int k = 0; k < mesh->at(i).vertex.size(); k++)
			{
				for (int j = 0; j < mesh->at(i).vertex.at(k).size(); j++)
				{
					point2d* tmppoint = new point2d();
					tmppoint->x = mesh->at(i).vertex.at(k).at(j).Position.X;
					tmppoint->y = mesh->at(i).vertex.at(k).at(j).Position.Z;
					allpoints.push_back(*tmppoint);
				}
			}
		}

		convex_hull(allpoints);
		printf("\n");
	}

	void drawcollision() {
		//allpoints.clear();
		//for (int i = 0; i < mesh->size(); i++)
		//{
		//	for (int k = 0; k < mesh->at(i).vertex.size(); k++)
		//	{
		//		for (int j = 0; j < mesh->at(i).vertex.at(k).size(); j++)
		//		{
		//			point2d* tmppoint = new point2d();
		//			tmppoint->x = mesh->at(i).vertex.at(k).at(j).Position.X;
		//			tmppoint->y = mesh->at(i).vertex.at(k).at(j).Position.Z;
		//			allpoints.push_back(*tmppoint);

		//			printf("(%f,%f)\n", mesh->at(i).vertex.at(k).at(j).Position.X, mesh->at(i).vertex.at(k).at(j).Position.Z);


		//		}
		//	}
		//}

		//convex_hull(allpoints);

		if (DRAWCOLLISION) {
			glMatrixMode(GL_MODELVIEW);
			glBegin(GL_POLYGON);

			for (int j = 0; j < allpoints.size(); j++)
			{
				glVertex3f(allpoints.at(j).x, 5, allpoints.at(j).y);
			}
			glEnd();
		}

	}

	vector<point2d>* getcollision() {
		return &allpoints;
	};
	void centerwhell() {
		Xangle = 0;
		cout << Xangle;
	};


	bool intersect(vector<point2d>* allbotpoints) {

		ClipperLib::Paths subj(1), clip(1), solution;

		for (int i = 0; i < allpoints.size(); i++)
		{
			//define outer blue 'subject' polygon
			subj[0] << IntPoint(allpoints.at(i).x, allpoints.at(i).y);
		}

		for (int i = 0; i < allbotpoints->size(); i++)
		{
			//define orange 'clipping' polygon
			clip[0] << IntPoint(allbotpoints->at(i).x, allbotpoints->at(i).y);
		}

		//perform intersection ...
		Clipper c;
		c.AddPaths(subj, ptSubject, true);
		c.AddPaths(clip, ptClip, true);
		c.Execute(ctIntersection, solution, pftNonZero, pftNonZero);

		if (solution.size() > 0) {
			glMatrixMode(GL_MODELVIEW);
			glBegin(GL_POLYGON);
			glClearColor(0.24706f, 0.7726f, 1.0f, 0.9412f);

			for (int j = 0; j < solution.size(); j++)
			{
				for (int k = 0; k < solution.at(j).size(); k++)
				{
					glVertex3f(solution.at(j).at(k).X, 7, solution.at(j).at(k).Y);

				}


			}
			glEnd();

			this->brake(1000);
			centerwhell();
			printf("r1 collides with r2\n");
			return true;
		}
		//else {
			//printf("r1 doesnt collide with r2\n");
		return false;
		//}
	}



	void step() {
		float saveangle = Xangle;
		if (allpoints.at(0).y > 2.0f and Xangle > 0) {
			Xangle = 0;
		}

		if (allpoints.at(0).y < -70.5f and Xangle < 0) {
			Xangle = 0;
		}

		ride_anim->step(Xangle, speed);
		scene->camera->z += Xangle;

		for (int i = 0; i < allpoints.size(); i++)
		{
			allpoints.at(i).x += speed;
			allpoints.at(i).y += Xangle;
		}

		if (camera != nullptr) {
			scene->camera->x += speed;


		}

		for (int i = 0; i < bots.size(); i++)
		{
			if (bots.at(i) != this and this->intersect(bots.at(i)->getcollision())) {
				bots.at(i)->brake(1000);
				bots.at(i)->centerwhell();
			}
		}

		Xangle = saveangle;
	}

	void startEngine() {
		ride_anim->restart();
	}
	void gas(float gasspeed) {
		if (speed + gasspeed >= maxspeed) {
			speed = maxspeed;
		}
		else {
			speed += gasspeed;
		}
	}
	void brake(float brakespeed) {
		if (speed - brakespeed <= 0) {
			speed = 0;
		}
		else {
			speed -= brakespeed;
		}
	}

	void rotatewhell(float angle) {
		if (Xangle + angle == 0) {
			Xangle = 0;
		}
		else {
			Xangle = angle;
		}
	}

	bool isRide() {
		return ride_anim->isFinished();
	}

	std::vector <Object3D>* getMesh() {
		return mesh;
	}
};


Car* playerCar;
//std::vector<Object3D> Brick;
//std::vector<std::vector<Object3D>*> Road;

float RandomFloat(float min, float max)
{
	assert(max > min);
	float random = ((float)rand()) / (float)RAND_MAX;
	float range = max - min;
	return (random * range) + min;
}

int RandInt(int nMin, int nMax)
{
	return nMin + (int)((double)rand() / (RAND_MAX + 1) * (nMax - nMin + 1));
}


void mapReGen() {
	if (!loaded) {
		for (int z = 0; z < 2; z++)
		{
			std::vector<Object3D>* tmpmap = new std::vector<Object3D>();
			*tmpmap = road_and_city;

			for (int i = 0; i < tmpmap->size(); i++)
			{
				for (int j = 0; j < tmpmap->at(i).vertex.size(); j++)
				{
					for (int k = 0; k < tmpmap->at(i).vertex.at(j).size(); k++)
					{

						glm::mat4 myMatrix = glm::translate(glm::mat4(1.f), glm::vec3(z * 2000.f, 14.0f, 0.0f));//////////////////////////////////////
						//glm::mat4 myRotMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0, 1.0, 0.0));

						glm::vec4 myVector(tmpmap->at(i).vertex.at(j).at(k).Position.X, tmpmap->at(i).vertex.at(j).at(k).Position.Y, tmpmap->at(i).vertex.at(j).at(k).Position.Z, 1.0f);
						glm::vec4 transformedVector = myMatrix * myVector;

						tmpmap->at(i).vertex.at(j).at(k).Position.X = transformedVector.x;
						tmpmap->at(i).vertex.at(j).at(k).Position.Y = transformedVector.y;
						tmpmap->at(i).vertex.at(j).at(k).Position.Z = transformedVector.z;


					}
				}
			}

			scene->RenderObjects.push_back(tmpmap);
			EnvMap.push_back(tmpmap);
		}







		for (int z = 0; z < BOTS_N; z++)
		{
			std::vector<Object3D>* tmpbot = new std::vector<Object3D>();
			*tmpbot = bot.at(RandInt(0, CAR_TYPES));
			float tmpf = RandomFloat(3, 8);
			for (int i = 0; i < tmpbot->size(); i++)
			{
				for (int j = 0; j < tmpbot->at(i).vertex.size(); j++)
				{
					for (int k = 0; k < tmpbot->at(i).vertex.at(j).size(); k++)
					{

						glm::mat4 myMatrix = glm::translate(glm::mat4(1.f), glm::vec3(-z * 35.f, 0.0f, tmpf * 8.f));
						glm::mat4 myRotMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));

						glm::vec4 myVector(tmpbot->at(i).vertex.at(j).at(k).Position.X, tmpbot->at(i).vertex.at(j).at(k).Position.Y, tmpbot->at(i).vertex.at(j).at(k).Position.Z, 1.0f);
						glm::vec4 transformedVector = myRotMatrix * myMatrix * myVector;

						tmpbot->at(i).vertex.at(j).at(k).Position.X = transformedVector.x;
						tmpbot->at(i).vertex.at(j).at(k).Position.Y = transformedVector.y;
						tmpbot->at(i).vertex.at(j).at(k).Position.Z = transformedVector.z;


					}
				}
			}
			scene->RenderObjects.push_back(tmpbot);

			bots.push_back(new Car(tmpbot));
			bots.at(bots.size() - 1)->gas(-0.5);
		}
	}
	else {
		for (int g = 0; g < EnvMap.size(); g++)
		{


			std::vector<Object3D>* tmpmap = EnvMap.at(g);
			if (tmpmap->at(0).vertex.at(0).at(0).Position.X < playerCar->getMesh()->at(0).vertex.at(0).at(0).Position.X) {//2000.f

				for (int i = 0; i < tmpmap->size(); i++)
				{
					for (int j = 0; j < tmpmap->at(i).vertex.size(); j++)
					{
						for (int k = 0; k < tmpmap->at(i).vertex.at(j).size(); k++)
						{

							glm::mat4 myMatrix = glm::translate(glm::mat4(1.f), glm::vec3(2000.f * 2, 0.0f, 0.0f));//////////////////////////////////////

							glm::vec4 myVector(tmpmap->at(i).vertex.at(j).at(k).Position.X, tmpmap->at(i).vertex.at(j).at(k).Position.Y, tmpmap->at(i).vertex.at(j).at(k).Position.Z, 1.0f);
							glm::vec4 transformedVector = myMatrix * myVector;

							tmpmap->at(i).vertex.at(j).at(k).Position.X = transformedVector.x;
							tmpmap->at(i).vertex.at(j).at(k).Position.Y = transformedVector.y;
							tmpmap->at(i).vertex.at(j).at(k).Position.Z = transformedVector.z;


						}
					}
				}
			}

		}



		for (int z = 0; z < BOTS_N; z++)
		{

			if (bots.at(z)->getMesh()->at(0).vertex.at(0).at(0).Position.X < playerCar->getMesh()->at(0).vertex.at(0).at(0).Position.X - 20) {

				for (int h = 0; h < scene->RenderObjects.size(); h++)
				{
					if (scene->RenderObjects.at(h) == bots.at(z)->getMesh()) {
						scene->RenderObjects.erase(scene->RenderObjects.begin() + h);
						break;
					}
				}
				bots.erase(bots.begin() + z);

				std::vector<Object3D>* tmpbot = new std::vector<Object3D>();
				*tmpbot = bot.at(RandInt(0, CAR_TYPES));;



				float tmpf = RandomFloat(3, 8);
				float tmpf2 = RandomFloat(BOTS_N / 2 * 35, BOTS_N * 35);

				for (int i = 0; i < tmpbot->size(); i++)
				{
					for (int j = 0; j < tmpbot->at(i).vertex.size(); j++)
					{
						for (int k = 0; k < tmpbot->at(i).vertex.at(j).size(); k++)
						{

							glm::mat4 myMatrix = glm::translate(glm::mat4(1.f), glm::vec3(-BOTS_N * 35 - playerCar->getMesh()->at(0).vertex.at(0).at(0).Position.X, 0.0f, tmpf * 8.f));
							glm::mat4 myRotMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));

							glm::vec4 myVector(tmpbot->at(i).vertex.at(j).at(k).Position.X, tmpbot->at(i).vertex.at(j).at(k).Position.Y, tmpbot->at(i).vertex.at(j).at(k).Position.Z, 1.0f);
							glm::vec4 transformedVector = myRotMatrix * myMatrix * myVector;

							tmpbot->at(i).vertex.at(j).at(k).Position.X = transformedVector.x;
							tmpbot->at(i).vertex.at(j).at(k).Position.Y = transformedVector.y;
							tmpbot->at(i).vertex.at(j).at(k).Position.Z = transformedVector.z;


						}
					}
				}
				scene->RenderObjects.push_back(tmpbot);

				bots.push_back(new Car(tmpbot));
				bots.at(bots.size() - 1)->gas(-0.5);
			}
		}
	}
}


void display(void)
{


	if (!loaded) {
		loader = new Loader();
		auto loadCar = [&](const std::string& filename) {
			auto tmpobject3D = new std::vector<Object3D>();
			loader->LoadFile(filename, tmpobject3D);
			bot.push_back(*tmpobject3D);
		};

		// Load player car
		auto tmpobject3D = new std::vector<Object3D>();
		loader->LoadFile("car1.obj", tmpobject3D);
		playerCar = new Car(tmpobject3D, scene->camera);
		playerCar->gas(1);
		scene->RenderObjects.push_back(playerCar->getMesh());
		bot.push_back(*tmpobject3D);

		// Load other cars
		std::vector<std::string> carFiles = {"car2.obj", "car5.obj", "car07.obj", "car09.obj", 
											"dlc_car1.obj", "dlc_car2.obj", "dlc_car3.obj", 
											"dlc_car4.obj", "dlc_car5.obj"};

		for (const auto& file : carFiles) {
			loadCar(file);
		}

		// Load environment
		tmpobject3D = new std::vector<Object3D>();
		loader->LoadFile("superlow.obj", tmpobject3D);
		road_and_city = *tmpobject3D;

		mapReGen();
		loaded = true;
	}





	mapReGen();

	playerCar->step();

	for (int i = 0; i < bots.size(); i++)
	{
		bots.at(i)->step();
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.24706f, 0.7726f, 1.0f, 0.9412f);


	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();


	int height = glutGet(GLUT_WINDOW_HEIGHT);
	int width = glutGet(GLUT_WINDOW_WIDTH);
	gluPerspective(60.0f, (double)width / (double)height, 1.0f, 4800.0f);

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

	auto t1 = std::chrono::high_resolution_clock::now();


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(scene->camera->x, scene->camera->y, scene->camera->z, scene->camera->x + scene->camera->rotx, scene->camera->y + scene->camera->roty, scene->camera->z + scene->camera->rotz, 0, 1, 0);


	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

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
					glTexCoord2d(scene->RenderObjects.at(i)->at(g).vertex.at(k).at(j).TextureCoordinate.X, scene->RenderObjects.at(i)->at(g).vertex.at(k).at(j).TextureCoordinate.Y);
					glVertex3f(scene->RenderObjects.at(i)->at(g).vertex.at(k).at(j).Position.X, scene->RenderObjects.at(i)->at(g).vertex.at(k).at(j).Position.Y, scene->RenderObjects.at(i)->at(g).vertex.at(k).at(j).Position.Z);
				}
				glEnd();
				glDisable(GL_TEXTURE_2D);
			}
		}
	}
	playerCar->drawcollision();
	for (int j = 0; j < bots.size(); j++)
	{
		bots.at(j)->drawcollision();
	}
	

	glutSwapBuffers();
	glutPostRedisplay();
}
int rotval = 0;
int cam_move = 0;

void keyPressed(unsigned char key, int x, int y) {
	if (key == 'w') {
		//mario->run();scene->camera->z += 0.5;

		scene->camera->z += scene->camera->rotz;
		scene->camera->x += scene->camera->rotx;
		//playerCar->rotatewhell(0.1);
	}
	if (key == 'a') {
		if (rotval == 0) {
			playerCar->rotatewhell(--rotval * 0.1);
			scene->camera->z -= 1 * scene->camera->rotx;
			scene->camera->x += 1 * scene->camera->rotz;
			cam_move--;
		}
		else if (rotval > 0) {
			rotval = 0;
			playerCar->rotatewhell(rotval);
			scene->camera->z = playerCar->getcollision()->at(0).y;
		}
		else if (rotval > -4) {
			playerCar->rotatewhell(--rotval * 0.1);
			scene->camera->z -= 1 * scene->camera->rotx;
			scene->camera->x += 1 * scene->camera->rotz;
			cam_move--;

		}

	}
	if (key == 's') {
		scene->camera->z -= scene->camera->rotz;
		scene->camera->x -= scene->camera->rotx;

	}
	if (key == 'd') {
		if (rotval == 0) {
			playerCar->rotatewhell(++rotval * 0.1);
			scene->camera->z += 1 * scene->camera->rotx;
			scene->camera->x -= 1 * scene->camera->rotz;

		}
		else if (rotval < 0) {
			rotval = 0;
			playerCar->rotatewhell(rotval);
			scene->camera->z = playerCar->getcollision()->at(0).y;

		}
		else if (rotval < 4) {
			playerCar->rotatewhell(++rotval * 0.1);
			scene->camera->z += 1 * scene->camera->rotx;
			scene->camera->x -= 1 * scene->camera->rotz;
			cam_move++;
		}


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
		//Object3D obj = Object3D(LoadedFaces, 1);
	}
	if (key == 32) {
		//mario->jump();

	}
}


int main(int argc, char** argv)
{
	// инициализация
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

	glutInitWindowPosition(100, 100);
	glutInitWindowSize(400, 400);
	glutCreateWindow("rgz");


	// регистрация обратных вызовов
	glutKeyboardFunc(keyPressed);
	glutDisplayFunc(display);

	//glutPassiveMotionFunc(My_mouse_routine);

	// Основной цикл GLUT
	glutMainLoop();
}