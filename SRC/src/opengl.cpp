#include "pch.h"
#include "opengl.h"

namespace{
	/**
	 * OpenGL callback for debugging
	 * @param source
	 * @param type
	 * @param id
	 * @param severity
	 * @param length
	 * @param message
	 * @param userParam
	 */
	void APIENTRY openglCallbackFunction(GLenum source,
										 GLenum type,
										 GLuint id,
										 GLenum severity,
										 GLsizei length,
										 const GLchar* message,
										 const void* userParam)
	{
		length;
		source;
		userParam;
		if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) return;
		std::cout << "{\n";
		std::cout << "\tmessage: " << message << "\n";
		std::cout << "\ttype: ";
		switch (type){
			case GL_DEBUG_TYPE_ERROR: std::cout << "ERROR";
				break;
			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "DEPRECATED_BEHAVIOR";
				break;
			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: std::cout << "UNDEFINED_BEHAVIOR";
				break;
			case GL_DEBUG_TYPE_PORTABILITY: std::cout << "PORTABILITY";
				break;
			case GL_DEBUG_TYPE_PERFORMANCE: std::cout << "PERFORMANCE";
				break;
			case GL_DEBUG_TYPE_OTHER: std::cout << "OTHER";
				break;
			default:break;
		}
		std::cout << "\n";

		std::cout << "\tid: " << id << "\n";
		std::cout << "\tseverity: ";
		switch (severity){
			case GL_DEBUG_SEVERITY_LOW: std::cout << "LOW";
				break;
			case GL_DEBUG_SEVERITY_MEDIUM: std::cout << "MEDIUM";
				break;
			case GL_DEBUG_SEVERITY_HIGH: std::cout << "HIGH";
				break;
			case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "NOTIFICATION";
				break;
			default:break;
		}
		std::cout << "\n}\n";
		XASSERT(type != GL_DEBUG_TYPE_ERROR);
	}
}

/**
 *
 */
void setup_gl_debug()
{

	// Debug
	GL_CALL(glEnable(GL_DEBUG_OUTPUT));
	GL_CALL(glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS));
	GL_CALL(glDebugMessageCallback(openglCallbackFunction, nullptr));
	GLuint unusedIds = 0;
	GL_CALL(glDebugMessageControl(GL_DONT_CARE,
								  GL_DONT_CARE,
								  GL_DONT_CARE,
								  0,
								  &unusedIds,
								  GL_TRUE));
}