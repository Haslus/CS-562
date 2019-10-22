#pragma once
#include "model.h"

class Object
{
public:

	Object(Model * model);
	void Draw(Shader shader, bool tessellation = false);
	Model * model;
private:

};