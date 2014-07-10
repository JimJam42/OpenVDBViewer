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

#include "Shader.h"

#include <iostream>
#include <fstream>

// include <string.h> for use of memset
#include <string.h>

Shader::Shader()
{
#ifdef DEBUG
    std::cerr<<"Please pass a name and shader type to the constructor"<<std::endl;
    std::cerr<<"Shader not built!!"<<std::endl;
#endif
}

Shader::Shader(std::string _name, SHADERTYPE _type)
{
    m_shaderName = _name;
    m_shaderType = _type;

    // switch on the type of shader passed in to then create that shader
    switch (_type)
    {
        case VERTEX:
        {
            m_shaderHandle = glCreateShader(GL_VERTEX_SHADER);
        }break;
        case FRAGMENT:
        {
            m_shaderHandle = glCreateShader(GL_FRAGMENT_SHADER);
        }break;
        case GEOMETRY:
        {
            m_shaderHandle = glCreateShader(GL_GEOMETRY_SHADER);
        }break;
        case TESSCONTROL:
        {
            m_shaderHandle = glCreateShader(GL_TESS_CONTROL_SHADER);
        }break;
        case TESSEVAL:
        {
            m_shaderHandle = glCreateShader(GL_TESS_EVALUATION_SHADER);
        }break;
    }

    m_compiled = false;
    // default path and source
    m_shaderFile = "__file_path_not_set__";
    m_shaderSource = "__source_empty__";
}

Shader::~Shader()
{
    // remove the shander handle
    glDeleteShader(m_shaderHandle);
}

void Shader::compile()
{
    // compile the shader and attempt to vlidate it
    glCompileShader(m_shaderHandle);
    if (!validateShader(m_shaderHandle, m_shaderFile.c_str()))
    {
        return;
    }
    m_compiled = true;
}

void Shader::loadFromFile(std::string _fileName)
{
    // pass in a file path and load from this file
    m_shaderFile = _fileName;
#ifdef DEBUG
    std::cout<<"Shader File name attribute ("<<_fileName<<") set"<<std::endl;
#endif

    // read source from file
    const char* source = textFileRead(_fileName.c_str());

    // craete shader handle
    glShaderSource(m_shaderHandle,1,&source,0);

    m_compiled = false;
}

void Shader::loadFromSource(const char *_shaderContents)
{
#ifdef DEBUG
    std::cout<<"WARNING - Loading shader from source only, file path attribute NOT set!"<<std::endl;
#endif
    // create shader handle straight from source
    glShaderSource(m_shaderHandle,1,&_shaderContents,0);
    m_compiled = false;
}

// ###################################################################################
// Functions taken from inspiration from functions written by Richard Southern in SDAGE Workshops
char* Shader::textFileRead(const char *fileName)
{
    // pass in a file name and read in the contents as a char*
    // pass this char* back to be loaded
    char* text = NULL;

    if (fileName != NULL) {
        FILE *file = fopen(fileName, "rt");

        if (file != NULL) {
            fseek(file, 0, SEEK_END);
            int count = ftell(file);
            rewind(file);


            if (count > 0)
            {
                text = (char*)malloc(sizeof(char) * (count + 1));
                count = fread(text, sizeof(char), count, file);
                text[count] = '\0';
            }
            fclose(file);
        }
    }
    return text;
}

bool Shader::validateShader(GLuint shader, std::string _name)
{
    const unsigned int BUFFER_SIZE = 512;
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    GLsizei length = 0;

    glGetShaderInfoLog(shader, BUFFER_SIZE, &length, buffer);
    if (length > 0)
    {
        std::cerr << "Shader " << shader << " (" << _name << ") compile error: " << buffer << std::endl;
        return false;
    }
    return true;
}
// ###################################################################################
