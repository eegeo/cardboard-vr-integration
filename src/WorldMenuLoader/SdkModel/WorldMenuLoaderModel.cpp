// Copyright eeGeo Ltd (2012-2015), All Rights Reserved

#include "WorldMenuLoaderModel.h"
#include "ApplicationConfiguration.h"
#include "Logger.h"

namespace Examples
{
    namespace WorldMenuLoader
    {
        namespace SdkModel
        {
            WorldMenuLoaderModel::WorldMenuLoaderModel(Eegeo::UI::WorldMenu::WorldMenuItemRepository& menuItemRepository,
                                                       ScreenFadeEffect::SdkModel::IScreenFadeEffectController& screenFader,
                                                       const ApplicationConfig::ApplicationConfiguration& appConfig)
            : m_menuItemRepository(menuItemRepository)
            , m_screenFader(screenFader)
            , m_worldMenuItemGazeCallback(this, &WorldMenuLoaderModel::OnWorldMenuItemGazed)
            , m_screenVisibilityChanged(this, &WorldMenuLoaderModel::OnScreenVisiblityChanged)
            , m_locationHasChanged(false)
            {
                m_shouldShowSplash = true;
                m_shouldRunVRSpline = false;
                m_screenFader.RegisterVisibilityChangedCallback(m_screenVisibilityChanged);

                const ApplicationConfig::TWorldLocations& worldLocations = appConfig.GetLocations();

                for (ApplicationConfig::TWorldLocations::const_iterator it = worldLocations.begin(); it != worldLocations.end(); ++it)
                {
                    const ApplicationConfig::WorldLocationData& locData = it->second;
                    int rightMargin = 0;
                    int leftMargin = 0;
                    
                    if(locData.GetLocationID()==0)
                    {
                        m_selectedLocation = it->first;
                        m_lastMenuItemGazedId = it->second.GetLocationID();
                        rightMargin = 15;
                    }
                    
                    Eegeo::UI::WorldMenu::WorldMenuItem* menuItem =  Eegeo_NEW(Eegeo::UI::WorldMenu::WorldMenuItem)(locData.GetLocationID(), locData.GetIconID(), m_worldMenuItemGazeCallback, new std::string(it->first), leftMargin, rightMargin);
                    m_menuItemRepository.AddWorldMenuItem(menuItem);
                    m_pWorldMenuItems.push_back(menuItem);
                }
                
                Eegeo::UI::WorldMenu::WorldMenuItem* menuItem =  Eegeo_NEW(Eegeo::UI::WorldMenu::WorldMenuItem)(4, 16, m_worldMenuItemGazeCallback,  NULL, 15);
                m_menuItemRepository.AddWorldMenuItem(menuItem);
                m_pWorldMenuItems.push_back(menuItem);

            }

            WorldMenuLoaderModel::~WorldMenuLoaderModel()
            {
                while(m_pWorldMenuItems.size()>0)
                {
                    Eegeo::UI::WorldMenu::WorldMenuItem* menuItem = *m_pWorldMenuItems.begin();
                    m_menuItemRepository.RemoveWorldMenuItem(menuItem);
                    m_pWorldMenuItems.erase(m_pWorldMenuItems.begin());
                    Eegeo_DELETE menuItem;
                }

                m_screenFader.UnregisterVisibilityChangedCallback(m_screenVisibilityChanged);
            }

            const std::string& WorldMenuLoaderModel::GetCurrentSelectedLocation() const
            {
                return m_selectedLocation;
            }

            void WorldMenuLoaderModel::OnWorldMenuItemGazed(Eegeo::UI::WorldMenu::WorldMenuItem &menuItem)
            {
                
                if(m_lastMenuItemGazedId==menuItem.GetId())
                    return;
                
                m_lastMenuItemGazedId = menuItem.GetId();
                
                if (menuItem.GetUserData() != NULL)
                {
                    m_shouldRunVRSpline = false;
                    const std::string *str = static_cast<const std::string *>(menuItem.GetUserData());
                    m_selectedLocation = *str;
                    m_screenFader.SetShouldFadeToBlack(true);
                    m_locationHasChanged = true;
                    m_shouldShowSplash = (menuItem.GetId()==0);
                }
                else if (menuItem.GetId()==4)
                {
                    m_shouldRunVRSpline = true;
                    m_screenFader.SetShouldFadeToBlack(true);
                    m_locationHasChanged = true;
                }
            }

            void WorldMenuLoaderModel::OnScreenVisiblityChanged(ScreenFadeEffect::SdkModel::IScreenFadeEffectController::VisibilityState &visbilityState)
            {
                if (visbilityState == ScreenFadeEffect::SdkModel::IScreenFadeEffectController::VisibilityState::FullyFaded && m_locationHasChanged)
                {
                    NotifyLocationChange(m_selectedLocation);
                    m_screenFader.SetShouldFadeToBlack(false);
                    m_locationHasChanged = false;
                }
            }
            
            void WorldMenuLoaderModel::OnPlayButtonGazed()
            {
                m_shouldRunVRSpline = false;
                m_shouldShowSplash = false;
                m_lastMenuItemGazedId = 1;
                m_selectedLocation="Dundee";
                m_locationHasChanged = true;
                m_screenFader.SetShouldFadeToBlack(true);
            }

            void WorldMenuLoaderModel::NotifyLocationChange(std::string& location)
            {
                m_locationChangedCallbacks.ExecuteCallbacks(location);
            }

            void WorldMenuLoaderModel::RegisterLocationChangedCallback(Eegeo::Helpers::ICallback1<std::string&>& callback)
            {
                m_locationChangedCallbacks.AddCallback(callback);
            }

            void WorldMenuLoaderModel::UnregisterLocationChangedCallback(Eegeo::Helpers::ICallback1<std::string&>& callback)
            {
                m_locationChangedCallbacks.RemoveCallback(callback);
            }
        }
    }
}
