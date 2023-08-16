#pragma once
#include <vector>
#include "raylib.h"
#include "WinInc.h"

struct Entity {
	float x = 0, y = 0;
	float rotation = 0;
	float speed = 0;
	unsigned char r = 0, g = 0, b = 0;
	float size = 1;
};

class EntityEditorApp {
public:
	EntityEditorApp(int screenWidth = 800, int screenHeight = 450);
	~EntityEditorApp();

	bool Startup();
	void Shutdown();

	void Update(float deltaTime);
	void Draw();

	// ZORA: Return the unsigned int size of the array's memory allocation, cast to a DWORD object for defining the memory needs of the NSM HANDLE
	DWORD GetArraySize();

	// ZORA: Return the memory address of the first object in the array of Entity objects
	void ArrayOfEntities(Entity* entity);

	unsigned int GetEntityCount();

//protected:
	int m_screenWidth;
	int m_screenHeight;

	// define a block of entities that should be shared
	enum { ENTITY_COUNT = 10 };
	Entity m_entities[ENTITY_COUNT];

	HANDLE h;
};