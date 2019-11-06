/**
* @file		transform.h
* @date 	02/08/2019
* @author	Asier Bilbao
* @par		Login: asier.b
* @par		Course: CS 562
* @par		Assignment #1
* @brief 	Framework
*
*/

#pragma once
#include "pch.h"

class Transform
{
public:
	Transform() : Position(glm::vec3{ 0, 0, 0 }), Scale(glm::vec3{ 1,1,1 }), Rotation(glm::vec3{ 0,0,0 })
	{
		RecalculateM2W();
	};
	Transform(glm::vec3 Pos, glm::vec3 Sca, glm::vec3 Rot) : Position(Pos), Scale(Sca), Rotation(Rot)
	{
		RecalculateM2W();
	};

	void RecalculateM2W();

	void AddPosition(glm::vec3);
	void AddScale(glm::vec3);
	void AddRotation(glm::vec3);

	void SetPosition(glm::vec3);
	void SetScale(glm::vec3);
	void SetRotation(glm::vec3);

	glm::vec3* GetPosition();
	glm::vec3* GetScale();
	glm::vec3* GetRotation();

	glm::mat4x4 M2W;

	glm::vec3 Position;
	glm::vec3 Scale;
	glm::vec3 Rotation;

private:


};

class Camera
{
public:
	Camera();
	void RecalculateViewMatrix();

	glm::mat4x4 ViewMatrix;
	vec3 camPos = vec3(0, 15, 20);
	vec3 camFront = vec3(0, 0, -1);
	vec3 camUp = vec3(0, 1, 0);

private:

};
