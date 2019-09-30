#pragma once
#include "pch.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define GL_CALL(what) {what; checkGlError();}

inline void checkGlError()
{
	const auto iErr = glGetError();
	if (iErr != GL_NO_ERROR){
		const char* pszError;
		switch (iErr){
			case GL_INVALID_ENUM:{
				pszError = "GL_INVALID_ENUM";
				break;
			}
			case GL_INVALID_VALUE:{
				pszError = "GL_INVALID_VALUE";
				break;
			};
			case GL_INVALID_OPERATION:{
				pszError = "GL_INVALID_OPERATION";
				break;
			};
			case GL_INVALID_FRAMEBUFFER_OPERATION:{
				pszError = "GL_INVALID_FRAMEBUFFER_OPERATION";
				break;
			};
			case GL_OUT_OF_MEMORY:{
				pszError = "GL_OUT_OF_MEMORY";
				break;
			};
			#if defined(GL_STACK_UNDERFLOW) && defined(GL_STACK_OVERFLOW)
			case GL_STACK_UNDERFLOW:{
				pszError = "GL_STACK_UNDERFLOW";
				break;
			};
			case GL_STACK_OVERFLOW:{
				pszError = "GL_STACK_OVERFLOW";
				break;
			};
			#endif
			default:{pszError = "Unknown";}
		}
		XERROR(pszError);
	}
}

std::vector<glm::vec<4, uint8_t>> take_screenshoot(uint32_t width, uint32_t height);
std::vector<glm::vec<4, uint8_t>> save_screenshoot(uint32_t width, uint32_t height, const char* filename);

