#pragma once

#include "include/Renderer/OpenGL3/ShaderProgram.h"
#include "include/Renderer/OpenGL3/OpenGLUtils.hpp"
#include "include/Core/EngineDefs.h"
#include "include/Core/Logging/Logger.h"

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
        UniformBuffer(const std::string& name, Logger& logger, const GLuint programID, size_t defaultSize = 5) : mName(name), mLogger(logger), mBindingIndex(++gNextUniformBindingIndex), mBlockOffset(0), mMaxSize(defaultSize), mCurrentSize(0)
        {
            GLCALL(glGenBuffers(1, &mBuffer));
            GLCALL(glGenBuffers(1, &mCopyBuffer));

            GLint uniformBufferAlignSize;
            GLCALL(glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &uniformBufferAlignSize));

            mBlockOffset = sizeof(ContentType);
			mBlockOffset += uniformBufferAlignSize - (mBlockOffset % uniformBufferAlignSize);
            Resize(defaultSize);

            GLuint blockIndex = glGetUniformBlockIndex(programID, mName.c_str());
            GLCALL(glUniformBlockBinding(programID, blockIndex, mBindingIndex));
        }

        ~UniformBuffer()
        {
            GLCALL(glDeleteBuffers(1, &mBuffer));
            GLCALL(glDeleteBuffers(1, &mCopyBuffer));
        }


        void ClearData()
        {
            GLCALL(glBindBuffer(GL_UNIFORM_BUFFER, mBuffer));
            GLCALL(glBufferData(GL_UNIFORM_BUFFER, mMaxSize * mBlockOffset, 0, GL_DYNAMIC_DRAW));
            GLCALL(glBindBufferRange(GL_UNIFORM_BUFFER, mBindingIndex, mBuffer, 0, sizeof(ContentType)));
            GLCALL(glBindBuffer(GL_UNIFORM_BUFFER, 0));

            mCurrentSize = 0;
        }

        void AddData(const ContentType& data)
        {

            if (mCurrentSize >= mMaxSize)
                Resize(mMaxSize * 2);

            GLCALL(glBindBuffer(GL_UNIFORM_BUFFER, mBuffer));
            GLCALL(glBufferSubData(GL_UNIFORM_BUFFER, mCurrentSize * mBlockOffset, sizeof(data), &data));
            GLCALL(glBindBuffer(GL_UNIFORM_BUFFER, 0));

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
            GLCALL(glBindBuffer(GL_COPY_READ_BUFFER, mBuffer));
            GLCALL(glBindBuffer(GL_COPY_WRITE_BUFFER, mCopyBuffer));

            GLCALL(glBufferData(GL_COPY_WRITE_BUFFER, mCurrentSize * mBlockOffset, 0, GL_DYNAMIC_COPY));
            GLCALL(glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, mCurrentSize * mBlockOffset));

            GLCALL(glBufferData(GL_COPY_READ_BUFFER, newSize * mBlockOffset, 0, GL_DYNAMIC_DRAW));
            GLCALL(glCopyBufferSubData(GL_COPY_WRITE_BUFFER, GL_COPY_READ_BUFFER, 0, 0, (mCurrentSize <= newSize ? mCurrentSize : newSize) * mBlockOffset));

            GLCALL(glBindBuffer(GL_COPY_READ_BUFFER, 0));
            GLCALL(glBindBuffer(GL_COPY_WRITE_BUFFER, 0));

            mMaxSize = newSize;
        }


        void SetActiveIndex(size_t index)
        {
            if (index <= mCurrentSize) 
            {
                GLCALL(glBindBuffer(GL_UNIFORM_BUFFER, mBuffer));
                GLCALL(glBindBufferRange(GL_UNIFORM_BUFFER, mBindingIndex, mBuffer, index * mBlockOffset, sizeof(ContentType)));
                GLCALL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
            }
        }


    private:
        const std::string mName;
        Logger& mLogger;

        GLuint mBindingIndex;
        GLuint mBuffer;
        GLuint mCopyBuffer;         // used as an intermediate buffer for copying data when resizing
        GLuint mBlockOffset;
        size_t mMaxSize;
        size_t mCurrentSize;
    };
}