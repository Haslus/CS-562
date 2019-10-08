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

#pragma once

struct Light
{
	Light(Model * model) {
		position = {0,0,0};
		color = { 0,0,0 };

		constant = 0;
		linear = 0;
		quadratic = 0;
		radius = 500;
		this->model = model;
	};

	Light(vec3 pos, vec3 col, float cons, float lin, float quad, Model * model) : position(pos), color(col)
		,constant(cons),linear(lin),quadratic(quad),model(model){
		radius = 500;
		model->transform.SetPosition(pos);
	};

	Model * model;

	vec3 position;
	vec3 color;

	float constant;
	float linear;
	float quadratic;
	float radius;
};