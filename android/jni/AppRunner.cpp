// Copyright eeGeo Ltd (2012-2014), All Rights Reserved

#include "AppRunner.h"
#include "Graphics.h"
#include "AndroidThreadHelper.h"
#include "Logger.h"

AppRunner::AppRunner
(
    AndroidNativeState* pNativeState
)
	: m_pNativeState(pNativeState)
	, m_pAppHost(NULL)
    , m_updatingNative(true)
	, m_isPaused(false)
{
	Eegeo::Helpers::ThreadHelpers::SetThisThreadAsMainThread();
}

AppRunner::~AppRunner()
{
	bool destroyEGL = true;

	if(m_pAppHost != NULL)
	{
		Eegeo_DELETE(m_pAppHost);
	}

	m_displayService.ReleaseDisplay(destroyEGL);
}

void AppRunner::CreateAppHost()
{
	if(m_pAppHost == NULL && m_displayService.IsDisplayAvailable())
	{
		m_pAppHost = Eegeo_NEW(AppHost)
		             (
		                 *m_pNativeState,
		                 m_displayService.GetDisplayWidth(),
		                 m_displayService.GetDisplayHeight(),
		                 m_displayService.GetDisplay(),
		                 m_displayService.GetSharedSurface(),
		                 m_displayService.GetResourceBuildSharedContext()
		             );
	}
}

void AppRunner::Pause()
{
	if(m_pAppHost != NULL && !m_isPaused)
	{
		m_pAppHost->OnPause();
		m_isPaused = true;
	}

	ReleaseDisplay();
}

void AppRunner::Resume()
{
	if(m_pAppHost != NULL && m_isPaused)
	{
		m_pAppHost->OnResume();
	}

	m_isPaused = false;
}

void AppRunner::ActivateSurface()
{
	Pause();
	bool displayBound = TryBindDisplay();
	Eegeo_ASSERT(displayBound);
	CreateAppHost();
	Resume();
}


void AppRunner::HandleTouchEvent(const Eegeo::Android::Input::TouchInputEvent& event)
{
    if(m_pAppHost != NULL)
    {
        m_pAppHost->HandleTouchInputEvent(event);
    }
}

void AppRunner::UpdateCardboardProfile(const float cardboardProfile[])
{
    if(m_pAppHost != NULL)
    {
        m_pAppHost->UpdateCardboardProfile(cardboardProfile);
    }
}

void AppRunner::MagnetTriggered()
{
    if(m_pAppHost != NULL)
    {
        m_pAppHost->MagnetTriggered();
    }
}

void AppRunner::ToggleNightMode(const bool turnOn)
{
    if(m_pAppHost != NULL)
    {
        m_pAppHost->ToggleNightMode(turnOn);
    }
}

void AppRunner::ReleaseDisplay()
{
	if(m_displayService.IsDisplayAvailable())
	{
		const bool teardownEGL = false;
		m_displayService.ReleaseDisplay(teardownEGL);
	}
}

bool AppRunner::TryBindDisplay()
{
	if(m_displayService.TryBindDisplay(*(m_pNativeState->window)))
	{
        
		if(m_pAppHost != NULL)
		{
			m_pAppHost->SetSharedSurface(m_displayService.GetSharedSurface());
			const Eegeo::Rendering::ScreenProperties& screenProperties = Eegeo::Rendering::ScreenProperties::Make(
					m_displayService.GetDisplayWidth(),
					m_displayService.GetDisplayHeight(),
					1.f,
					m_pNativeState->deviceDpi);
			m_pAppHost->NotifyScreenPropertiesChanged(screenProperties);
			m_pAppHost->SetViewportOffset(0, 0);
		}

		return true;
	}

	return false;
}

void AppRunner::Update(float deltaSeconds, const float headTansform[])
{
	if(m_updatingNative && m_pAppHost != NULL && m_displayService.IsDisplayAvailable())
	{
		m_pAppHost->Update(deltaSeconds, headTansform);
        
        // swap buffers
		Eegeo_GL(eglSwapBuffers(m_displayService.GetDisplay(), m_displayService.GetSurface()));

        
        Eegeo_GL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
        
        // clear buffers
        Eegeo::Helpers::GLHelpers::ClearBuffers();
        
        // engine draw call
        m_pAppHost->Draw(deltaSeconds);
        
        
	}
}

void AppRunner::StopUpdatingNativeBeforeTeardown()
{
    m_updatingNative = false;
}

