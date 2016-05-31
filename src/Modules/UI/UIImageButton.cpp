//
//  UIImageButton.cpp
//  SDKSamplesApp
//
//  Created by Ali on 5/16/16.
//
//

#include "UIImageButton.h"
#include "RenderingModule.h"
#include "EegeoWorld.h"
#include "Logger.h"

namespace Eegeo
{
    namespace UI
    {
        UIImageButton::UIImageButton(Eegeo::Modules::Core::RenderingModule& p_RenderingModule
                                     , Modules::IPlatformAbstractionModule& p_PlatformAbstractionModule
                                     , const std::string& fileName
                                     , const Eegeo::dv3& p_ecefPosition
                                     , const Eegeo::v2& p_Dimension
                                     , Eegeo::Helpers::ICallback0& p_OnClickedEvent
                                     , const Eegeo::v2& p_uvMin
                                     , const Eegeo::v2& p_uvMax
                                     , const Eegeo::v4& p_initialColor
                                     , const Eegeo::Rendering::LayerIds::Values p_RenderLayer
                                     ) :
          m_OnClickedEvent(p_OnClickedEvent)
        {
            m_ButtonImage = Eegeo_NEW(Eegeo::UI::UIQuad)(p_RenderingModule,
                                                         p_RenderingModule.GetGlBufferPool(),
                                                         p_RenderingModule.GetVertexBindingPool(),
                                                         p_RenderingModule.GetVertexLayoutPool(),
                                                         p_PlatformAbstractionModule.GetTextureFileLoader(),
                                                         p_RenderingModule.GetRenderableFilters(),
                                                         fileName,
                                                         p_ecefPosition,
                                                         p_Dimension,
                                                         p_uvMin,
                                                         p_uvMax,
                                                         p_initialColor,
                                                         p_RenderLayer
                                                         );
            
            m_Radius = (p_Dimension.x > p_Dimension.y ? p_Dimension.x : p_Dimension.y)/2.0f;
        }
        
        void UIImageButton::Update(float dt)
        {
        }
        
        void UIImageButton::OnItemClicked()
        {
            m_OnClickedEvent();
        }
        
        void UIImageButton::OnFocusGained()
        {
            m_ButtonImage->SetScale(Eegeo::v3::One() * 1.1f);
        }
        
        void UIImageButton::OnFocusLost()
        {
            m_ButtonImage->SetScale(Eegeo::v3::One());
        }
        
        UIImageButton::~UIImageButton(){
            Eegeo_DELETE m_ButtonImage;
        }
    }
}
