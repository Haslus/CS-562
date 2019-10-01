#pragma once
#include "model.h"

class Object
{
public:

	Object(Model * model);
	void Draw(Shader shader);
private:
	Model * model;

};