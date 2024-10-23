#include "MouseInteractorStyle.h"
#include <vtkRenderWindowInteractor.h>
#include <vtkObjectFactory.h>
#include <vtkTextureObject.h>
#include <vtkFloatArray.h>
#include <vtkRenderer.h>
#include <vtkMatrix4x4.h>
#include <vtkCamera.h>
#include <vtkRendererCollection.h>

vtkStandardNewMacro(MouseInteractorStyle);

MouseInteractorStyle::MouseInteractorStyle()
{
    _offScreenWindow = nullptr;
}

void MouseInteractorStyle::SetOffScrrenWindow(vtkRenderWindow* win)
{
    _offScreenWindow = win;
}

void MouseInteractorStyle::OnLeftButtonDown()
{
    if (_offScreenWindow)
    {
        // Get the location of the click (in window coordinates).
        int* pos = this->GetInteractor()->GetEventPosition();

        int x = pos[0];
        int y = pos[1];

        // 获取像素数据
        unsigned char* pixelData = _offScreenWindow->GetPixelData(x, y, x, y, 0);

        float depth = _offScreenWindow->GetZbufferDataAtPoint(x, y);

        vtkRenderer* render = _offScreenWindow->GetRenderers()->GetFirstRenderer();
        // 转换屏幕坐标到世界坐标
        double worldPt[4];
        render->SetDisplayPoint(x, y, depth);
        render->DisplayToWorld();
        render->GetWorldPoint(worldPt);

        // 在这里，你可以根据 pixelData 来拾取对象
        unsigned int id = 0;
        id |= pixelData[0] << 0;
        id |= pixelData[1] << 8;
        id |= pixelData[2] << 16;

        if (id != 0)
            std::cout << "id : " << id << " worldPt :" 
            << worldPt[0] / worldPt[3]  << " " 
            << worldPt[1] / worldPt[3] << " " 
            << worldPt[2] / worldPt[3] << " " << std::endl;
        else
            std::cout << "nothing is here!" << std::endl;

        // 释放像素数据
        delete[] pixelData;

        // Forward events.
        vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
    }
}