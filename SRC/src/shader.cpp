/**
* @file		shader.cpp
* @date 	02/08/2019
* @author	Asier Bilbao
* @par		Login: asier.b
* @par		Course: CS 562
* @par		Assignment #1
* @brief 	Framework
*
*/

#include "pch.h"
#include "shader.h"

/**
* @brief 	Custom constructor for a shader, reading two files
* @param	vertex_path
* @param	frag_path
*/
Shader::Shader(const std::string& vertex_path, const std::string& frag_path)
{
	std::string line;
	std::ifstream vertex(vertex_path.c_str());
	if (vertex.is_open())
	{
		while (std::getline(vertex, line))
		{
			vertex_shader_text += line + '\n';
		}
	}
	vertex.close();

	if (vertex_shader_text.empty())
	{
		std::cout << "ERROR LOADING VERTEX SHADER" << std::endl;
	}

	std::ifstream frag(frag_path.c_str());

	if (frag.is_open())
	{
		while (std::getline(frag, line))
		{
			fragment_shader_text += line + '\n';
		}
	}
	frag.close();

	if (fragment_shader_text.empty())
	{
		std::cout << "ERROR LOADING FRAGMENT SHADER" << std::endl;
	}

	InitializeVertShader();
	InitializeFragShader();
	LinkProgram();
	CheckCompileErrors(program, "PROGRAM");
}
Shader::Shader(const std::string* paths)
{
	program = glCreateProgram();


		{
			std::string line1;
			std::ifstream file1(paths[0].c_str());
			if (file1.is_open())
			{
				while (std::getline(file1, line1))
				{
					vertex_shader_text += line1 + '\n';
				}
			}
			file1.close();

			vertex_shader = glCreateShader(GL_VERTEX_SHADER);
			const char * v_code = vertex_shader_text.c_str();
			glShaderSource(vertex_shader, 1, &v_code, NULL);
			glCompileShader(vertex_shader);
			CheckCompileErrors(vertex_shader, "FUR.VERT");
		}

		{
			std::string line3;
			std::ifstream file3(paths[1].c_str());
			if (file3.is_open())
			{
				while (std::getline(file3, line3))
				{
					geo_shader_text += line3 + '\n';
				}
			}
			file3.close();
			geo_shader = glCreateShader(GL_GEOMETRY_SHADER);
			const char * geo_code = geo_shader_text.c_str();
			glShaderSource(geo_shader, 1, &geo_code, NULL);
			glCompileShader(geo_shader);
			CheckCompileErrors(geo_shader, "FUR.GEO");
		}

		{
			std::string line4;
			std::ifstream file4(paths[2].c_str());
			if (file4.is_open())
			{
				while (std::getline(file4, line4))
				{
					fragment_shader_text += line4 + '\n';
				}
			}
			file4.close();
			fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
			const char * f_code = fragment_shader_text.c_str();
			glShaderSource(fragment_shader, 1, &f_code, NULL);
			glCompileShader(fragment_shader);
			CheckCompileErrors(fragment_shader, "FUR.FRAG");
		}

		glAttachShader(program, vertex_shader);
		glAttachShader(program, geo_shader);
		glAttachShader(program, fragment_shader);
		glLinkProgram(program);
		CheckCompileErrors(program, "PROGRAM");
	


	
}

/**
* @brief 	use shader
*/
void Shader::Use()
{
	glUseProgram(program);
}

/**
* @brief 	Set bool value in the Shader
*/
void Shader::SetBool(const std::string& name, bool value)
{
	glUniform1i(glGetUniformLocation(program, name.c_str()),static_cast<int>(value));
}

/**
* @brief 	Set integer value in the Shader
*/
void Shader::SetInt(const std::string& name, int value)
{
	glUniform1i(glGetUniformLocation(program, name.c_str()), value);
}

/**
* @brief 	Set float value in the Shader
*/
void Shader::SetFloat(const std::string& name, float value)
{
	glUniform1f(glGetUniformLocation(program, name.c_str()), value);
}
/**
* @brief 	Set vector of size 2 value in the Shader
*/
void Shader::SetVec2(const std::string & name, vec2 value)
{
	glUniform2fv(glGetUniformLocation(program, name.c_str()), 1, &value[0]);
}
/**
* @brief 	Set vector of size 3 value in the Shader
*/
void Shader::SetVec3(const std::string & name, vec3 value)
{
	glUniform3fv(glGetUniformLocation(program, name.c_str()),1, &value[0]);
}
/**
* @brief 	Set vector of size 4 value in the Shader
*/
void Shader::SetVec4(const std::string & name, vec4 value)
{
	glUniform4fv(glGetUniformLocation(program, name.c_str()), 1, &value[0]);
}
/**
* @brief 	Set 4x4 matrix value in the Shader
*/
void Shader::SetMat4(const std::string & name, mat4 mat)
{
	glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()),1, GL_FALSE, &mat[0][0]);
}

/**
* @brief 	Initialize the vertex shader
*/
void Shader::InitializeVertShader()
{
	const char* text = vertex_shader_text.c_str();

	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &text, NULL);
	glCompileShader(vertex_shader);
	CheckCompileErrors(vertex_shader, "VERTEX");

}
/**
* @brief 	Initialize the fragment shader
*/
void Shader::InitializeFragShader()
{
	const char* text = fragment_shader_text.c_str();

	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &text, NULL);
	glCompileShader(fragment_shader);
	CheckCompileErrors(fragment_shader, "FRAGMENT");
}
/**
* @brief 	Link the program
*/
void Shader::LinkProgram()
{
	program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
}

/**
* @brief 	Check for errors in the sahder
*/
void Shader::CheckCompileErrors(GLuint shader, std::string type)
{
	GLint success;
	GLchar infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
}