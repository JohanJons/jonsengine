#pragma once

#include "include/Core/EngineDefs.h"
#include "include/Renderer/OpenGL3/ShaderProgram.h"

#include "GL/glew.h"
#include <string>

namespace JonsEngine
{
    /* Counter of uniform binding index */
    static GLuint gNextUniformBindingIndex = 0;

    /* UniformBuffer definition */
    template <typename ContentType>
    class UniformBuffer
    {
    public:
        UniformBuffer(const std::string& name, const ShaderProgram& program, size_t defaultSize = 5) : mName(name), mBindingIndex(++gNextUniformBindingIndex), mBlockOffset(0), mMaxSize(defaultSize), mCurrentSize(0)
        {
            glGenBuffers(1, &mBuffer);
            glGenBuffers(1, &mCopyBuffer);

            GLint uniformBufferAlignSize;
            glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &uniformBufferAlignSize);

            mBlockOffset = sizeof(ContentType);
			mBlockOffset += uniformBufferAlignSize - (mBlockOffset % uniformBufferAlignSize);
            Resize(defaultSize);

            GLuint blockIndex = glGetUniformBlockIndex(program.GetHandle(), mName.c_str());
            glUniformBlockBinding(program.GetHandle(), blockIndex, mBindingIndex);
        }

        ~UniformBuffer()
        {
            glDeleteBuffers(1, &mBuffer);
            glDeleteBuffers(1, &mCopyBuffer);
        }


        void ClearData()
        {
            glBindBuffer(GL_UNIFORM_BUFFER, mBuffer);
            glBufferData(GL_UNIFORM_BUFFER, mMaxSize * mBlockOffset, NULL, GL_DYNAMIC_DRAW);
            glBindBufferRange(GL_UNIFORM_BUFFER, mBindingIndex, mBuffer, 0, sizeof(ContentType));
            glBindBuffer(GL_UNIFORM_BUFFER, 0);

            mCurrentSize = 0;
        }

        void AddData(const ContentType& data)
        {
            if (mCurrentSize >= mMaxSize)
                Resize(mMaxSize * 2);

            glBindBuffer(GL_UNIFORM_BUFFER, mBuffer);
            glBufferSubData(GL_UNIFORM_BUFFER, mCurrentSize * mBlockOffset, sizeof(data), &data);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);

            mCurrentSize++;
        }

        void SetData(const ContentType& data)
        {
            ClearData();
            AddData(data);
            SetActiveIndex(0);
        }


        size_t Size() const     { return mMaxSize; }

        void Resize(size_t newSize)
        {
            glBindBuffer(GL_COPY_READ_BUFFER, mBuffer);
            glBindBuffer(GL_COPY_WRITE_BUFFER, mCopyBuffer);

            glBufferData(GL_COPY_WRITE_BUFFER, mCurrentSize * mBlockOffset, NULL, GL_DYNAMIC_COPY);
            glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, mCurrentSize * mBlockOffset);

            glBufferData(GL_COPY_READ_BUFFER, newSize * mBlockOffset, NULL, GL_DYNAMIC_DRAW);
            glCopyBufferSubData(GL_COPY_WRITE_BUFFER, GL_COPY_READ_BUFFER, 0, 0, (mCurrentSize <= newSize ? mCurrentSize : newSize) * mBlockOffset);

            glBindBuffer(GL_COPY_READ_BUFFER, 0);
            glBindBuffer(GL_COPY_WRITE_BUFFER, 0);

            mMaxSize = newSize;
        }


        void SetActiveIndex(size_t index)
        {
            if (index <= mCurrentSize) 
            {
                glBindBuffer(GL_UNIFORM_BUFFER, mBuffer);
                glBindBufferRange(GL_UNIFORM_BUFFER, mBindingIndex, mBuffer, index * mBlockOffset, sizeof(ContentType));
                glBindBuffer(GL_UNIFORM_BUFFER, 0);
            }
        }


    private:
        const std::string mName;
        GLuint mBindingIndex;
        GLuint mBuffer;
        GLuint mCopyBuffer;         // used as an intermediate buffer for copying data when resizing
        GLuint mBlockOffset;
        size_t mMaxSize;
        size_t mCurrentSize;
    };
}