#include "object.h"

Object::Object(Model * model)
{
	this->model = model;
}

void Object::Draw(Shader shader, bool tessellation)
{
	model->Draw(shader, tessellation);
}