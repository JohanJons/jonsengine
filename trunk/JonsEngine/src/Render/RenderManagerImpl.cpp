#include "include/Render/RenderManagerImpl.h"

namespace JonsEngine
{

	RenderManagerImpl::RenderManagerImpl(ILogManager& logger, IMemoryAllocator& memAllocator) : mRunning(false), mScreenWidth(320), mScreenHeight(480),
																								mLog(logger), mMemoryAllocator(memAllocator)
	{
	}

	RenderManagerImpl::~RenderManagerImpl()
	{
		if (mInitialized)
			Destroy();
	}

	bool RenderManagerImpl::Init()
	{
		bool ret = true;

		mInitialized = true;

		return ret;
	}

	bool RenderManagerImpl::Destroy()
	{
		if (mInitialized)
		{
			bool res = true;

			if (mRunning)
				res &= Stop();

			res &= !mRunning;

			mInitialized = false;

			return res;
		}
		else
			return false;
	}

	bool RenderManagerImpl::Start()
	{
		bool res = true;

		if (!mRunning)
		{
			glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

			mRunning = true;
		}
		else
			mLog.LogWarn() <<  "RenderManagerImpl::Start(): RenderManager already started!" << std::endl;

		return res;
	}

	bool RenderManagerImpl::Stop()
	{
		bool res = true;

		if (mRunning)
		{
			mRunning = false;

		}
		else
			mLog.LogWarn() <<  "RenderManagerImpl::Stop(): RenderManager already stopped!" << std::endl;

		return res;
	}

	bool RenderManagerImpl::isRunning()
	{

		return mRunning;
	}

	void RenderManagerImpl::Tick()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void RenderManagerImpl::ResizeWindow(int32_t width, int32_t height)
	{
		if (width > 0 && height > 0)
		{
			mScreenWidth = width;
			mScreenHeight = height;
		}
		else
			mLog.LogWarn() << "RenderManagerImpl::ResizeWindow(): Illegal parameters supplied" << std::endl;
	}

	int32_t RenderManagerImpl::GetWindowWidth() const
	{
		return mScreenWidth;
	}

	int32_t RenderManagerImpl::GetWindowHeight() const
	{
		return mScreenHeight;
	}

}
