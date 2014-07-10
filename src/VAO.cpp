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


#include "VAO.h"
#include "Utilities.h"

#include <iostream>

VAO::VAO(GLenum _drawMode)
{
    // set the draw mode
    m_drawMode = _drawMode;

    // default values
    m_arraysBound=false;
    m_indexed = false;
    m_indexType = GL_UNSIGNED_BYTE;
    m_indicesCount = 0;

    m_vboPointers.resize(0);
}

VAO::~VAO()
{
    // empty destructor
}

void VAO::create()
{
    // when create is called, a vertex array pointer is generated
    glGenVertexArrays(1,&m_vaPointer);
    Utilities::checkGLError();
}

void VAO::bind()
{
    // bind the VA and set bound to true
    glBindVertexArray(m_vaPointer);
    m_arraysBound =  true;
}

void VAO::draw()
{
    if (m_arraysBound) // only draw if bound
    {
        if (!m_indexed)
        {
            // draw without index
            glDrawArrays(m_drawMode,0,m_indicesCount);
        }
        else
        {
            // draw if indexed data
            glDrawElements(m_drawMode,m_indicesCount,m_indexType,(GLvoid*)((char*)NULL));
        }
    }
    else
    {
        std::cerr<<"Attempting to draw without first binding!!"<<std::endl;
    }
}

void VAO::draw(GLenum _mode) // same as other draw, but with a draw mode pased in
{
    if (m_arraysBound)
    {
        if (!m_indexed)
        {
            glDrawArrays(_mode,0,m_indicesCount);
        }
        else
        {
            glDrawElements(_mode,m_indicesCount,m_indexType,(GLvoid*)((char*)NULL));
        }
    }
    else
    {
        std::cerr<<"Attempting to draw without first binding!!"<<std::endl;
    }
}

void VAO::unbind()
{
    // unbind the VA
    glBindVertexArray(0);
    m_arraysBound =  false;
}

void VAO::setData(int _size, GLfloat &_data, GLenum _mode)
{
    // set unindexed data
    if (!m_arraysBound)
    {
        #ifdef DEBUG
            std::cout<<"Performing an automatic binding as VAO was not manually bound before setting data"<<std::endl;
            std::cout<<"A manual unbind will need to be performed!!"<<std::endl;
        #endif
            // if not bound, bind automatically
        bind();
    }

    // set data here
    GLuint vbo;
    // gen vertex buffer object
    glGenBuffers(1,&vbo);

    glBindBuffer(GL_ARRAY_BUFFER,vbo);
    glBufferData(GL_ARRAY_BUFFER,_size,&_data,_mode);
    // store vbo poiter to allow for expansion of more than one
    m_vboPointers.push_back(vbo);
}

void VAO::setIndexedData(unsigned int _size, const GLfloat &_data, unsigned int _indexSize, const GLvoid *_indexData, GLenum _indexType, GLenum _mode)
{
    if (!m_arraysBound)
    {
        #ifdef DEBUG
            std::cout<<"Performing an automatic binding as VAO was not manually bound before setting data"<<std::endl;
            std::cout<<"A manual unbind will need to be performed!!"<<std::endl;
        #endif
        bind();
    }

    GLuint vboID;
    // gen vertex buffer object
    glGenBuffers(1, &vboID);
    // store vbo poiter to allow for expansion of more than one
    m_vboPointers.push_back(vboID);

    glGenBuffers(1, &m_iboID);

    // now we will bind an array buffer to the first one and load the data for the verts
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glBufferData(GL_ARRAY_BUFFER, _size, &_data, _mode);

    int size = 0;
    switch(_indexType)
    { // find type f indexed data
      case GL_UNSIGNED_INT   : size=sizeof(GLuint);   break;
      case GL_UNSIGNED_SHORT : size=sizeof(GLushort); break;
      case GL_UNSIGNED_BYTE  : size=sizeof(GLubyte);  break;
      default : std::cerr<<"Invalid data type sent for data set\n"; break;
    }

    // create an element array to store the index data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iboID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indexSize * size, _indexData, _mode);

    m_indexed=true; // set indexed to true
    m_indexType=_indexType;
}

void VAO::updateIndexedData(unsigned int _size, const GLfloat &_data, GLenum _mode)
{
    if (!m_arraysBound)
    {
        #ifdef DEBUG
            std::cout<<"Performing an automatic binding as VAO was not manually bound before setting data"<<std::endl;
            std::cout<<"A manual unbind will need to be performed!!"<<std::endl;
        #endif
        bind();
    }

    if (m_indexed)
    {
      glBindBuffer(GL_ARRAY_BUFFER, VBOid(0));
      glBufferData(GL_ARRAY_BUFFER, _size, &_data, _mode);
    }
}

void VAO::vertexAttribPointer(GLint _loc, GLint _size, GLenum _type, GLsizei _stride, int _offset,bool _normalise)
{
    glEnableVertexAttribArray(_loc);
    glVertexAttribPointer(_loc,_size,_type,_normalise,_stride,(GLfloat *)NULL + _offset);
}

void VAO::remove()
{
    if (m_arraysBound)
    {
        // first make sure it is unbound
        unbind();
    }

    for (unsigned int i = 0; i< m_vboPointers.size(); i++)
    {
        // for all vbos stored, delete them - at the moment this has little purpose but it gives the room for expansion to multiple vbos
        glDeleteBuffers(1,&m_vboPointers[i]);
    }

    glDeleteVertexArrays(1,&m_vaPointer);
}

GLuint VAO::VBOid(unsigned int _index)
{
    GLuint id=0;

    if(_index<m_vboPointers.size())
    {
      id=m_vboPointers[_index];
    }
    return id;
}
