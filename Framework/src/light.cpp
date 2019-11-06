#include "light.h"

void Light::update(float dt)
{
	if (pause)
		return;

	time += dt;
	model->transform.Position.y += cos(time) * dt * 5.0f;
	model->transform.RecalculateM2W();
}
