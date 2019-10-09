/**
* @file		light.h
* @date 	09/21/2019
* @author	Asier Bilbao
* @par		Login: asier.b
* @par		Course: CS 562
* @par		Assignment #1
* @brief 	Framework
*
*/
#include "pch.h"
#include "model.h"
#pragma once

struct Light
{
	Light(Model * model) {
		color = { 0,0,0 };

		radius = 75;
		this->model = model;
	};

	Light(vec3 pos, vec3 col, Model * model) : color(col) ,model(model){
		radius = 75;
		model->transform.SetPosition(pos);
	};

	void update(float dt);

	Model * model;
	bool pause = false;
	vec3 color;
	float radius;
	float time = 0;


};