#include "light.h"

void Light::update(float dt)
{
	if (pause)
		return;

	model->transform.Position.x = glm::cos(dt) * 50.f;
	model->transform.RecalculateM2W();
}
