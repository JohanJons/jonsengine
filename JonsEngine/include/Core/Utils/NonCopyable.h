#pragma once

namespace JonsEngine
{
    class NonCopyable
    {
    public:
        NonCopyable(const NonCopyable&) = delete;
        NonCopyable& operator=(const NonCopyable&) = delete;


    protected:
        NonCopyable()
        {
        }

        ~NonCopyable()
        {
        }
    };
}