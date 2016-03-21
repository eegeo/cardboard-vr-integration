// Copyright eeGeo Ltd (2012-2014), All Rights Reserved

#pragma once

#include "IExampleFactory.h"
#include "IExample.h"
#include "EegeoWorld.h"
#include "ScreenPropertiesProvider.h"

namespace Examples
{
    class MeshExampleFactory : public IExampleFactory
    {
        Eegeo::EegeoWorld& m_world;
        DefaultCameraControllerFactory& m_defaultCameraControllerFactory;
        Eegeo::Camera::GlobeCamera::GlobeCameraTouchController& m_globeCameraTouchController;
        
        const IScreenPropertiesProvider& m_screenPropertiesProvider;
        
    public:
        MeshExampleFactory(Eegeo::EegeoWorld& world,
                           DefaultCameraControllerFactory& defaultCameraControllerFactory,
                           Eegeo::Camera::GlobeCamera::GlobeCameraTouchController& globeCameraTouchController,
                           const IScreenPropertiesProvider& screenPropertiesProvider);
        
        std::string ExampleName() const;
        
        IExample* CreateExample() const;
    };
}