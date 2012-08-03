#include "../../Include/Render/RenderManagerImpl.h"

namespace JonsEngine
{

	RenderManagerImpl::RenderManagerImpl() :mRunning(false), mScreenWidth(320), mScreenHeight(480), mLog(NULL)
	{
	}

	RenderManagerImpl::~RenderManagerImpl()
	{
		if (mInitialized)
			Destroy();
	}

	bool RenderManagerImpl::Init(ILogManager* const logger, IMemoryManager* const memmgr)
	{
		mLog = logger;
		mMemoryManager = memmgr;

		if (mLog && mMemoryManager)
		{
			mInitialized = true;

			return true;
		}
		else
			return false;
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
			mLog->LogWarn() <<  "RenderManagerImpl::Start(): RenderManager already started!" << std::endl;

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
			mLog->LogWarn() <<  "RenderManagerImpl::Stop(): RenderManager already stopped!" << std::endl;

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
			mLog->LogWarn() << "RenderManagerImpl::ResizeWindow(): Illegal parameters supplied" << std::endl;
	}

	int32_t RenderManagerImpl::GetWindowWidth()
	{
		return mScreenWidth;
	}

	int32_t RenderManagerImpl::GetWindowHeight()
	{
		return mScreenHeight;
	}

}
