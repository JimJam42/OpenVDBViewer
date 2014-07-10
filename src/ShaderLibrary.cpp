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

#include "ShaderLibrary.h"
#include "Utilities.h"

#include <iostream>

ShaderLibrary::ShaderLibrary()
{
    // clear all vectors
    m_shaderFamilies.resize(0);
    m_shaders.resize(0);
    m_shaderMap.resize(0);
    m_familyMap.resize(0);

    // create the null shader family ready for use
    m_nullFamily = new ShaderFamily("NULL");
    m_activeShader="NULL";
    // add this shader family to the vector so it can be returned
    m_shaderFamilies.push_back(m_nullFamily);
    NameID temp;
    temp.a_name = "NULL";
    temp.a_id = 0;
    m_familyMap.push_back(temp);
}

ShaderLibrary::~ShaderLibrary()
{
    // remove the null family
    delete m_nullFamily;
}

void ShaderLibrary::createShaderFamily(std::string _name)
{
    // craete a new shader family - attach the postfix Shader to keep to naming conventions
    std::string finalName = _name+"Shader";
    // add family to vector
    m_shaderFamilies.push_back(new ShaderFamily(finalName));
    NameID temp;
    temp.a_name = finalName;
    temp.a_id = m_shaderFamilies.size()-1;
    m_familyMap.push_back(temp);
}

void ShaderLibrary::createVertFragShaderFamily(std::string _name)
{
    // create a family and automatically create a vertex and fragment shader to go with it
    std::string finalName = _name+"Shader";
#ifdef DEBUG
    std::cout<<"\nRenamed shader family "<<_name<<" to "<<finalName<<std::endl;
#endif
    m_shaderFamilies.push_back(new ShaderFamily(finalName));
    NameID temp;
    temp.a_name = finalName;
    temp.a_id = m_shaderFamilies.size()-1;
    m_familyMap.push_back(temp);
#ifdef DEBUG
    std::cout<<"Automatically creating a Vertex and Fragment Shader for "<<finalName<<", ("<<_name<<"Vertex and "<<_name<<"Fragment)"<<std::endl;
#endif
    // create the vertex and fragment shader
    createVertFragShader(_name);
}

void ShaderLibrary::createVertFragShader(std::string _name)
{
    // create the vertex shader with the postfix Vertex for naming conventions
    createShader(_name+"Vertex",VERTEX);
#ifdef DEBUG
    std::cout<<"Created shader "<<_name+"Vertex"<<std::endl;
#endif
    // create the fragment shader with the postfix Fragment for naming conventions
    createShader(_name+"Fragment",FRAGMENT);
#ifdef DEBUG
    std::cout<<"Created shader "<<_name+"Fragment"<<std::endl;
#endif
}

void ShaderLibrary::createShader(std::string _name, SHADERTYPE _type)
{
    // create a new shadr and add to the vector. Do nothing with it
    m_shaders.push_back(new Shader(_name,_type));
    NameID temp;
    temp.a_name = _name;
    temp.a_id = m_shaders.size()-1;
    m_shaderMap.push_back(temp);
}

void ShaderLibrary::addShader(std::string _familyName, SHADERTYPE _type, std::string _file)
{
    // this is the shebang
    // create a shader, load it from file, compile it and link it all in one!
    std::string name = _familyName+suffixFromType(_type);
    createShader(name,_type);
    loadShaderFile(name,_file);
    compileShader(name);
    linkShaderToFamily(name,_familyName);
}

void ShaderLibrary::addShader(std::string _familyName, SHADERTYPE _type, char *_string)
{
    // this is the shebang
    // create a shader, load it from source, compile it and link it all in one!
    std::string name = _familyName+suffixFromType(_type);
    createShader(name,_type);
    loadShaderSource(name,_string);
    compileShader(name);
    linkShaderToFamily(name,_familyName);
}

void ShaderLibrary::linkShaderToFamily(std::string _shader, std::string _family)
{
    // link a specified shader to a family
    std::string familyFull = _family + "Shader";
    // find the shadr index
    int shaderIndex = findShader(_shader);
    // find the family index
    int familyIndex = findShaderFamily(familyFull);

    if(shaderIndex != -1 && familyIndex != -1)
    {
        // attach
        m_shaderFamilies[familyIndex]->attachShader(m_shaders[shaderIndex]);
    }
    else
    {
        std::cerr<<"Failed to attach "<<_shader<<" to "<<familyFull<<std::endl;
    }
}

GLuint ShaderLibrary::getID(std::string _name)
{
    std::string finalName = _name+"Shader";
    // find the families index and return its id
    int familyIndex = findShaderFamily(finalName);

    if(familyIndex != -1)
    {
      return m_shaderFamilies[familyIndex]->getID();
    }
    else
    {
      std::cerr<<"Family "<<finalName.c_str()<<" not found"<<std::endl;
      return -1;
    }
}

void ShaderLibrary::compileAllFamiles()
{
    // loop through all families and compile them all
    for (unsigned int i = 0; i < m_shaderFamilies.size(); i++)
    {
        m_shaderFamilies[i]->compile();
    }
}

void ShaderLibrary::compileAll()
{
    // compile all shaders
    for (unsigned int i = 0; i < m_shaders.size(); i++)
    {
        m_shaders[i]->compile();
    }
}

void ShaderLibrary::compileShader(std::string _name)
{
    // find shader to compile
    int index = findShader(_name);

    if (index != -1)
    {
        // if found, compile
        m_shaders[index]->compile();
    }
    else
    {
        std::cerr<<"Shader "<<_name<<" not found for compilation"<<std::endl;
    }
}

void ShaderLibrary::linkAll()
{
    // link all families stored
    for (unsigned int i = 0; i < m_shaderFamilies.size(); i++)
    {
        m_shaderFamilies[i]->link();
    }
}

void ShaderLibrary::linkShader(std::string _name)
{
    std::string fullName = _name+"Shader";
    // find the shader family needed
    int index = findShaderFamily(fullName);
    if (index != -1)
    {
        // if found, link
        m_shaderFamilies[index]->link();
    }
    else
    {
        std::cerr<<"Shader family "<<fullName<<" not found for linking"<<std::endl;
    }
}

void ShaderLibrary::setActive(std::string _name)
{
    std::string fullName = _name+"Shader";
    // find the shader family you wish to set to active
    int index = findShaderFamily(fullName);
    if(index != -1)
    {
        // set the active shader name
      m_activeShader=fullName;
      // bind the currently active shader family
      m_shaderFamilies[index]->bind();
    }
    else
    {
      std::cerr<<"Family "<<fullName.c_str()<<"  not found"<<"\n";
      m_activeShader="NULL";
      glUseProgram(0);
    }
}

ShaderFamily* ShaderLibrary::getFamily(std::string _name)
{
    std::string finalName = _name+"Shader";
    // find family index and return the family at this index
    int index = findShaderFamily(finalName);
    if(index != -1)
    {
        return m_shaderFamilies[index];
    }
    else
    {
        std::cerr<<"Family not found for return"<<std::endl;
        ShaderFamily* nullFamily = new ShaderFamily();
        return nullFamily;
    }
}

GLint ShaderLibrary::getUniformLocation(std::string _shaderName, std::string _uniformName)
{
    GLint loc=0;

    std::string finalName = _shaderName+"Shader";

    int index = findShaderFamily(finalName);

    if(index != -1)
    {

      loc=glGetUniformLocation((long) m_shaderFamilies[index]->getID(),_uniformName.c_str());
    }
    else
    {
      std::cerr <<_uniformName<<" from "<<finalName<<" not found\n";
    }

    return loc;
}

GLint ShaderLibrary::getAttribLocation(std::string _shaderName, std::string _paramName)
{
    GLint attrib=0;

    std::string finalName = _shaderName+"Shader";

    int index = findShaderFamily(finalName);

    if(index != -1)
    {

      attrib=glGetAttribLocation((long) m_shaderFamilies[index]->getID(),_paramName.c_str());
    }
    else
    {
      std::cerr <<_paramName<<" from "<<finalName<<" not found\n";
    }

    return attrib;
}

void ShaderLibrary::loadVertFragShaderFile(std::string _shaderName, std::string _vertex, std::string _frag)
{
    std::string _nameVert = _shaderName+"Vertex";
    std::string _nameFrag = _shaderName+"Fragment";

    // load vertex and fragment shader from file
    loadShaderFile(_nameVert,_vertex);
    loadShaderFile(_nameFrag,_frag);
}

void ShaderLibrary::loadShaderFile(std::string _shaderName, std::string _sourceFile)
{
    // find the shaders index
    int index = findShader(_shaderName);

    if (index != -1)
    {
        // if found, load from file
        m_shaders[index]->loadFromFile(_sourceFile);
    }
    else
    {
        std::cerr<<"Shader "<<_shaderName<<" not found"<<std::endl;
    }
}

void ShaderLibrary::loadShaderSource(std::string &_shaderName, char *_string)
{
    // find the shaders index
    int index = findShader(_shaderName);

    if (index != -1)
    {
        // if found, load from source
        m_shaders[index]->loadFromSource(_string);
    }
    else
    {
        std::cerr<<"Shader "<<_shaderName<<" not found"<<std::endl;
    }
}

void ShaderLibrary::loadVertFragShaderSource(std::string _shaderName, char *_vertex, char *_frag)
{
    // postfix names for naming convention to make finding the shaders easier
    std::string _nameVert = _shaderName+"Vertex";
    std::string _nameFrag = _shaderName+"Fragment";
    // load vertex and fragment shader from file
    loadShaderSource(_nameVert,_vertex);
    loadShaderSource(_nameFrag,_frag);
}

void ShaderLibrary::clean()
{
    for (unsigned int i = 0; i < m_shaderFamilies.size(); i++)
    {
        // delete all families
      delete m_shaderFamilies[i];
    }

    for (unsigned int j = 0; j < m_shaders.size(); j++)
    {
        // delete all shaders
      delete m_shaders[j];
    }
}

void ShaderLibrary::setShaderUniformFromMat3(const std::string &_uniformName, openvdb::Mat3R _m)
{
    int index = findShaderFamily(m_activeShader);

    if (index != -1)
    {
        m_shaderFamilies[index]->setUniformMatrix3fv(_uniformName.c_str(),1,GL_FALSE,(float*)&Utilities::u_Mat3ToFloatArray(_m)[0]);
    }
    else
    {
        std::cerr<<"Shader Family not found to set uniform Matrix 4x4 for"<<std::endl;
    }
}

void ShaderLibrary::setShaderUniformFromMat4(const std::string &_uniformName, openvdb::Mat4s _m)
{
    int index = findShaderFamily(m_activeShader);

    if (index != -1)
    {
        m_shaderFamilies[index]->setUniformMatrix4fv(_uniformName.c_str(),1,GL_FALSE,(float*)&Utilities::u_Mat4ToFloatArray(_m)[0]);
    }
    else
    {
        std::cerr<<"Shader Family not found to set uniform Matrix 4x4 for"<<std::endl;
    }
}

void ShaderLibrary::setShaderParam1f(const std::string &_paramName, float _f)
{
    int index = findShaderFamily(m_activeShader);

    if (index != -1)
    {
        m_shaderFamilies[index]->setUniform1f(_paramName.c_str(),_f);
    }
    else
    {
        std::cerr<<"Shader Family not found to set parameter 1f for"<<std::endl;
    }
}

void ShaderLibrary::setShaderParam1i(const std::string &_paramName, int _i)
{
    int index = findShaderFamily(m_activeShader);

    if (index != -1)
    {
        m_shaderFamilies[index]->setUniform1i(_paramName.c_str(),_i);
    }
    else
    {
        std::cerr<<"Shader Family not found to set parameter 1i for"<<std::endl;
    }
}

void ShaderLibrary::setShaderParam3f(const std::string &_paramName, float _f1, float _f2, float _f3)
{
    int index = findShaderFamily(m_activeShader);

    if (index != -1)
    {
        m_shaderFamilies[index]->setUniform3f(_paramName.c_str(),_f1,_f2,_f3);
    }
    else
    {
        std::cerr<<"Shader Family not found to set parameter 3f for"<<std::endl;
    }
}

void ShaderLibrary::setShaderParam4f(const std::string &_paramName, float _f1, float _f2, float _f3, float _f4)
{
    int index = findShaderFamily(m_activeShader);

    if (index != -1)
    {
        m_shaderFamilies[index]->setUniform4f(_paramName.c_str(),_f1,_f2,_f3,_f4);
    }
    else
    {
        std::cerr<<"Shader Family not found to set parameter 4f for"<<std::endl;
    }
}

void ShaderLibrary::setShaderParam4i(const std::string &_paramName, int _i1, int _i2, int _i3, int _i4)
{
    int index = findShaderFamily(m_activeShader);

    if (index != -1)
    {
        m_shaderFamilies[index]->setUniform4i(_paramName.c_str(),_i1,_i2,_i3,_i4);
    }
    else
    {
        std::cerr<<"Shader Family not found to set parameter 4f for"<<std::endl;
    }
}

int ShaderLibrary::findShader(std::string _name)
{
    bool found = false;
    int size = m_shaderMap.size();
    int count = 0;
    int index = -1;

    // loop through until shader found or reach end of map

    while (!found && count < size)
    {
        if (_name == m_shaderMap[count].a_name)
        {
            // set found to true and store the found index
            index = m_shaderMap[count].a_id;
            found = true;
        }
        count++;
    }
    // return this index
    return index;
}

int ShaderLibrary::findShaderFamily(std::string _name)
{
    bool found = false;
    int size = m_familyMap.size();
    int count = 0;
    int index = -1;

    // loop through until family found or reach end of map
    while (!found && count < size)
    {
        if (_name == m_familyMap[count].a_name)
        {
            // set found to true and store the found index
            index = m_familyMap[count].a_id;
            found = true;
        }
        count++;
    }
    // return this index
    return index;
}

// get the suffix for the naming convention from the shader type passed in
std::string ShaderLibrary::suffixFromType(SHADERTYPE _type)
{
    switch(_type)
    {
        case(VERTEX):
        {
            return "Vertex";
        }break;
        case(FRAGMENT):
        {
            return "Fragment";
        }break;
        case(GEOMETRY):
        {
            return "Geometry";
        }break;
        case(TESSEVAL):
        {
            return "TesseVal";
        }break;
        case(TESSCONTROL):
        {
            return "TessControl";
        }break;
        default:
        {
            return "NO_TYPE";
        }break;
    }
}
