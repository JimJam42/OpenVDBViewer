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


#ifndef __VAO_H__
#define __VAO_H__

#ifndef DARWIN
    #include <GL/glew.h>
#else
    #include <OpenGL/gl3.h>
#endif

#include <vector>
#include <string>

/// @file VAO.h
/// @brief VAO class to handle drawing of objects and VAO meshes and their upload to the GPU
/// @author Callum James
/// @version 1.0
/// @date 12/02/2014
/// Revision History:
/// Initial Version 05/01/2014
/// @class VAO
/// @brief VAO class handles the drawing of VAO meshes and Vertex Array pointers. Also handles
/// the switch between unindexed and indexed data for drawing. Used in all classes with a draw
/// capability
class VAO
{
public:
    /// @brief Constructor for VAO class
    /// @param [in] _drawMode GLenum - draw mode for the VAO to use when drawing
    VAO(GLenum _drawMode=GL_TRIANGLE_STRIP);
    /// @brief Destructor for VAO class
    ~VAO();
    /// @brief Create the VAO
    void create();
    /// @brief Bind the VAO
    void bind();
    /// @brief Unbind the VAO
    void unbind();
    /// @brief Draw the VAO
    void draw();
    /// @brief Draw the VAO
    /// @param [in] _mode GLenum - draw mode
    void draw(GLenum _mode);
    /// @brief Set the data in the VAO
    /// @param [in] _size int - size of data being uploaded
    /// @param [in] _data GLfloat - data to upload
    /// @param [in] _mode GLenum - draw mode
    void setData(int _size, GLfloat &_data, GLenum _mode=GL_STATIC_DRAW);
    /// @brief Set the data indexed in the VAO
    /// @param [in] _size int - size of data being uploaded
    /// @param [in] _data GLfloat - data to upload
    /// @param [in] _indexSize unsigned int - size of index
    /// @param [in] _indexData const GLvoid* - index data
    /// @param [in] _indexType GLenum - the type of the index data being uploaded
    /// @param [in] _mode GLenum - draw mode
    void setIndexedData(unsigned int _size,const GLfloat &_data,unsigned int _indexSize,const GLvoid *_indexData,GLenum _indexType,GLenum _mode=GL_STATIC_DRAW);
    /// @brief Update indexed data when changed
    /// @param [in] _size unsigned int - size of data
    /// @param [in] _data const GLfloat - data to upload
    /// @param [in] _mode GLenum - draw mode
    void updateIndexedData(unsigned int _size, const GLfloat &_data,GLenum _mode);
    /// @brief Set the Vertex Attribute Pointer
    /// @param [in] _loc GLint - location of Vertex Attribute Pointer
    /// @param [in] _size GLint - size of data
    /// @param [in] _type GLenum - data type
    /// @param [in] _stride GLsizei - stride of data
    /// @param [in] _offset int - offset of this data
    /// @param [in] _normalise bool - normalise the data when uploading
    void vertexAttribPointer(GLint _loc, GLint _size, GLenum _type, GLsizei _stride, int _offset, bool _normalise=GL_FALSE);
    /// @brief Set the draw mode
    /// @param [in] _drawMode GLenum - draw mode to set
    inline void setDrawMode(GLenum _drawMode) {m_drawMode = _drawMode;}
    /// @brief Get the Vertext Array pointer - returns GLuint
    inline GLuint getVAPointer() {return m_vaPointer;}
    /// @brief Return if bound - returns true or false
    inline bool bound() {return m_arraysBound;}
    /// @brief Set the number of indices to draw
    /// @param [in] _count int - number to draw
    inline void setIndicesCount(int _count) {m_indicesCount = _count;}
    /// @brief Return the number of indices to draw - return int
    inline int indicesCount() const {return m_indicesCount;}
    /// @brief Remove the VAO
    void remove();

private:
    /// @brief The draw mode for this VAO
    GLenum m_drawMode;
    /// @brief Number of indices to draw
    int m_indicesCount;
    /// @brief The vertex array pointer
    GLuint m_vaPointer;

    // as there can be multiple vbo attached to one vao need a vector these
    /// @brief Array of Vertex Buffer pointers
    std::vector<GLuint> m_vboPointers;
    /// @brief Boolean of if the arrays are bound or not
    bool m_arraysBound;
    /// @brief Boolean of if the VAO is indexed or not
    bool m_indexed;
    /// @brief The index type
    GLenum m_indexType;
    /// @brief IBO ID (indexed buffer)
    GLuint m_iboID;
    /// @brief Return the VBO pointer at the specified index
    /// @param [in] _index unsigned int - infex to return the VBO id
    GLuint VBOid(unsigned int _index);
};

#endif /* __VAO_H__ */
