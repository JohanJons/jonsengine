#include "../../Include/Render/RenderManagerImpl.h"

namespace JonsEngine
{

	RenderManagerImpl::RenderManagerImpl(ILogManager* logger) : mLog(logger), mRunning(false), 
																mScreenWidth(320), mScreenHeight(480)
	{
	}

	RenderManagerImpl::~RenderManagerImpl()
	{
		Destroy();
	}

	int32_t RenderManagerImpl::Init()
	{
		if (mLog)
			return INIT_OK;
		else
			return INIT_NOK;
	}

	bool RenderManagerImpl::Destroy()
	{
		bool res = true;

		if (mRunning)
			res &= Stop();

		res &= !mRunning;

		return res;
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
