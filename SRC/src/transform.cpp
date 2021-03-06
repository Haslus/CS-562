/**
* @file		transform.cpp
* @date 	02/08/2019
* @author	Asier Bilbao
* @par		Login: asier.b
* @par		Course: CS 562
* @par		Assignment #1
* @brief 	Framework
*
*/
#include "pch.h"
#include "transform.h"

/**
* @brief 	Recalculate the M2W matrix
*/
void Transform::RecalculateM2W()
{
	glm::mat4x4 T = glm::translate(Position);
	glm::mat4x4 S = glm::scale(Scale);
	glm::mat4x4 RX = glm::rotate(glm::radians(Rotation.x), glm::vec3(1, 0, 0));
	glm::mat4x4 RY = glm::rotate(glm::radians(Rotation.y), glm::vec3(0, 1, 0));
	glm::mat4x4 RZ = glm::rotate(glm::radians(Rotation.z), glm::vec3(0, 0, 1));

	M2W = T * (RX * RY * RZ) * S;
}
/**
* @brief 	Add position
* @param	newPos
*/
void Transform::AddPosition(glm::vec3 newPos)
{
	Position += newPos;
	RecalculateM2W();
}
/**
* @brief 	Add scale
* @param	newSca
*/
void Transform::AddScale(glm::vec3 newSca)
{
	Scale += newSca;
	RecalculateM2W();
}
/**
* @brief 	Add rotation
* @param	newRot
*/
void Transform::AddRotation(glm::vec3 newRot)
{
	Rotation += newRot;
	RecalculateM2W();
}
/**
* @brief 	Set position
* @param	newPos
*/
void Transform::SetPosition(glm::vec3 newPos)
{
	Position = newPos;
	RecalculateM2W();
}
/**
* @brief 	Set scale
* @param	newSca
*/
void Transform::SetScale(glm::vec3 newSca)
{
	Scale = newSca;
	RecalculateM2W();
}
/**
* @brief 	Set rotation
* @param	newRot
*/
void Transform::SetRotation(glm::vec3 newRot)
{
	Rotation = newRot;
}
/**
* @brief 	Get position
*/
glm::vec3 * Transform::GetPosition()
{
	return &Position;
}
/**
* @brief 	Get scale
*/
glm::vec3 * Transform::GetScale()
{
	return &Scale;
}
/**
* @brief 	Get rotation
*/
glm::vec3 * Transform::GetRotation()
{
	return &Rotation;
}
/**
* @brief 	Default constructor
*/
Camera::Camera()
{
	RecalculateViewMatrix();
}
/**
* @brief 	Recalculate view matrix
*/
void Camera::RecalculateViewMatrix()
{

	if (orbital)
	{
		camPos = glm::vec3(glm::sin(AngleX) * glm::cos(AngleY) * Radius, glm::sin(AngleY) * Radius, glm::cos(AngleX) * glm::cos(AngleY) * Radius);

		glm::vec3 fwdVec = glm::normalize(camPos);
		ViewMatrix = glm::lookAt(camPos, vec3(0,0,0), camUp);
	}
	else
	{
		glm::quat front = quaternion * glm::quat(0, 0, 0, -1) * glm::conjugate(quaternion);
		camFront = { front.x,front.y,front.z };
		glm::quat right = quaternion * glm::quat(0, 1, 0, 0) * glm::conjugate(quaternion);
		camRight = { right.x,right.y,right.z };

		glm::quat inverse = glm::conjugate(quaternion);
		glm::mat4 transInverse = glm::translate(-camPos);


		ViewMatrix = glm::toMat4(inverse) * transInverse;

	}

}
