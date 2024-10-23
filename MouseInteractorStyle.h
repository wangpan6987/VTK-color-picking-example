#pragma once
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkRenderWindow.h>

class MouseInteractorStyle : public vtkInteractorStyleTrackballCamera
{
public:
    static MouseInteractorStyle* New();
    vtkTypeMacro(MouseInteractorStyle, vtkInteractorStyleTrackballCamera);
    virtual void OnLeftButtonDown() override;

    void SetOffScrrenWindow(vtkRenderWindow* win);

private:
    MouseInteractorStyle();
    vtkRenderWindow* _offScreenWindow;
};