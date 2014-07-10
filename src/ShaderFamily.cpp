/*
  Copyright (C) 2014 Callum James

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdlib.h>
#include <iostream>
#include <string.h>

#include "ShaderFamily.h"

ShaderFamily::ShaderFamily()
{
#ifdef DEBUG
    std::cerr<<"A name is needed for this shader family - no name provided"<<std::endl;
    std::cerr<<"Setting shader family ID to NULL"<<std::endl;
#endif
    // default values
    m_familyID = 0;
    m_link=false;
    m_active=false;
}

ShaderFamily::ShaderFamily(std::string _name)
{
    // create a new shader program and store its handle
    m_familyID = glCreateProgram();
    m_familyName = _name;
    m_link=false;
    m_active=false;
}

ShaderFamily::~ShaderFamily()
{
    // delete the shader program
    glDeleteProgram(m_familyID);
}

void ShaderFamily::bind()
{
    // bind
    glUseProgram(m_familyID);
    m_active = true;
}

void ShaderFamily::unbind()
{// unbind this family
    glUseProgram(0);
    m_active = false;
}

void ShaderFamily::attachShader(Shader *_shader)
{
    // add a new shader to the shader vector and then attach a shader in gl
    m_shaders.push_back(_shader);
    glAttachShader(m_familyID, _shader->getShaderHandle());
}

void ShaderFamily::bindAttrib(GLuint _index, const std::string &_attribName)
{
    // bind an attribute location
    glBindAttribLocation(m_familyID,_index,_attribName.c_str());
}

void ShaderFamily::compile()
{
    std::vector<Shader*>::iterator it;

    // loop through all shaders and compile all
    for (it = m_shaders.begin(); it!= m_shaders.end(); it++)
    {
        if (!(*it)->compiled())
        {
            (*it)->compile();
        }
    }
}

void ShaderFamily::link()
{
    // link this shader family
    glLinkProgram(m_familyID);
    if (!validateProgram(m_familyID))
    {
        return;
    }
    m_link = true;
}

GLuint ShaderFamily::getUniformLocation(const char *_name)
{
    GLint loc = glGetUniformLocation( m_familyID ,_name);
    if (loc == -1)
    {
      std::cerr<<"Uniform \""<<_name<<"\" not found in GL Program \""<<m_familyName<<"\"\n";
    }
    return loc;
}

void ShaderFamily::enableVertAttribArray(GLuint _loc)
{
    glEnableVertexAttribArray(_loc);
}

void ShaderFamily::disableVertAttribArray(GLuint _loc)
{
    glDisableVertexAttribArray(_loc);
}

// ######################################################################################
// following functions are all used to set uniforms and attributes on the shaders of different types
void ShaderFamily::setUniform1f(const char* _var, float _f)
{
    glUniform1f(getUniformLocation(_var),_f);
}

void ShaderFamily::setUniform2f(const char* _var, float _f0, float _f1)
{
    glUniform2f(getUniformLocation(_var),_f0,_f1);
}

void ShaderFamily::setUniform3f(const char* _var, float _f0, float _f1, float _f2)
{
    glUniform3f(getUniformLocation(_var),_f0,_f1,_f2);
}

void ShaderFamily::setUniform4f(const char* _var, float _f0, float _f1, float _f2, float _f3)
{
    glUniform4f(getUniformLocation(_var),_f0,_f1,_f2,_f3);
}

void ShaderFamily::setUniform1fv(const char* _var, size_t _count, const float* _f)
{
    glUniform1fv(getUniformLocation(_var),_count,_f);
}

void ShaderFamily::setUniform2fv(const char* _var,  size_t _count, const float* _f)
{
    glUniform2fv(getUniformLocation(_var),_count,_f);
}

void ShaderFamily::setUniform3fv(const char* _var,size_t _count, const float* _f)
{
    glUniform3fv(getUniformLocation(_var),_count,_f);
}

void ShaderFamily::setUniform4fv(const char* _var, size_t _count,  const float* _f)
{
    glUniform4fv(getUniformLocation(_var),_count,_f);
}

void ShaderFamily::setUniform1i(const char* _var, GLint _i)
{
    glUniform1i(getUniformLocation(_var),_i);
}

void ShaderFamily::setUniform2i(const char* _var, GLint _i0, GLint _i1)
{
    glUniform2i(getUniformLocation(_var),_i0,_i1);
}

void ShaderFamily::setUniform3i(const char* _var, GLint _i0, GLint _i1, GLint _i2)
{
    glUniform3i(getUniformLocation(_var),_i0,_i1,_i2);
}

void ShaderFamily::setUniform4i(const char* _var, GLint _i0, GLint _i1, GLint _i2, GLint _i3)
{
    glUniform4i(getUniformLocation(_var),_i0,_i1,_i2,_i3);
}

void ShaderFamily::setUniform1iv(const char* _var, size_t _count, const GLint* _i)
{
    glUniform1iv(getUniformLocation(_var),_count,_i);
}

void ShaderFamily::setUniform2iv(const char* _var, size_t _count, const GLint* _i)
{
    glUniform2iv(getUniformLocation(_var),_count,_i);
}

void ShaderFamily::setUniform3iv(const char* _var, size_t _count, const GLint* _i)
{
    glUniform3iv(getUniformLocation(_var),_count,_i);
}

void ShaderFamily::setUniform4iv(const char* _var, size_t _count, const GLint* _i)
{
    glUniform4iv(getUniformLocation(_var),_count,_i);
}

void ShaderFamily::setUniformMatrix2fv(const char* _var, size_t _count,  bool _transpose,  const float* _m)
{
    glUniformMatrix2fv(getUniformLocation(_var),_count,_transpose,_m);
}

void ShaderFamily::setUniformMatrix3fv(const char* _var, size_t _count, bool _transpose, const float* _m)
{
    glUniformMatrix3fv(getUniformLocation(_var),_count,_transpose,_m);
}

void ShaderFamily::setUniformMatrix4fv(const char* _var, size_t _count, bool _transpose, const float* _m)
{
    glUniformMatrix4fv(getUniformLocation(_var),_count,_transpose,_m);
}

void ShaderFamily::setUniformMatrix2x3fv(const char* _var, size_t _count, bool _transpose, const float* _m)
{
    glUniformMatrix2x3fv(getUniformLocation(_var),_count,_transpose,_m);
}

void ShaderFamily::setUniformMatrix2x4fv(const char* _var, size_t _count, bool _transpose,  const float* _m)
{
    glUniformMatrix2x4fv(getUniformLocation(_var),_count,_transpose,_m);
}

void ShaderFamily::setUniformMatrix3x2fv(const char* _var, size_t _count, bool _transpose, const float* _m)
{
    glUniformMatrix3x2fv(getUniformLocation(_var),_count,_transpose,_m);
}

void ShaderFamily::setUniformMatrix3x4fv(const char* _var, size_t _count, bool _transpose, const float* _m)
{
    glUniformMatrix3x4fv(getUniformLocation(_var),_count,_transpose,_m);
}

void ShaderFamily::setUniformMatrix4x2fv(const char* _var, size_t _count, bool _transpose,  const float* _m)
{
    glUniformMatrix4x2fv(getUniformLocation(_var),_count,_transpose,_m);
}

void ShaderFamily::setUniformMatrix4x3fv(const char* _var, size_t _count, bool _transpose, const float* _m)
{
    glUniformMatrix4x3fv(getUniformLocation(_var),_count,_transpose,_m);
}

// ######################################################################################

void ShaderFamily::getUniformfv(const char* _name, float* o_f )
{
    glGetUniformfv(m_familyID, getUniformLocation(_name),o_f);
}

void ShaderFamily::getUniformiv(const char* _name, int* o_i )
{
    glGetUniformiv(m_familyID,getUniformLocation(_name),o_i);
}

// function taken from one written by Richard Southern in a SDAGE workshop
bool ShaderFamily::validateProgram(GLuint program)
{
    const unsigned int BUFFER_SIZE = 512;
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    GLsizei length = 0;

    memset(buffer, 0, BUFFER_SIZE);
    glGetProgramInfoLog(program, BUFFER_SIZE, &length, buffer);
    if (length > 0)
    {
        std::cerr << "Program " << program << " link error: " << buffer << std::endl;
        return false;
    }

    glValidateProgram(program);
    GLint status;
    glGetProgramiv(program, GL_VALIDATE_STATUS, &status);
    if (status == GL_FALSE) {
        std::cerr << "Error validating shader " << program << std::endl;
        return false;
    }
    return true;
}
