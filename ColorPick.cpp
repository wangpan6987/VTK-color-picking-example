// ColorPick.cpp: 定义应用程序的入口点。
//

#include "command.hpp"
#include "MouseInteractorStyle.h"

#include <vtkRenderer.h>
#include <vtkActor.h>
#include <vtkOpenGLPolyDataMapper.h>
#include <vtkPolyData.h>
#include <vtkShaderProperty.h>

#include <vtkConeSource.h>
#include <vtkCubeSource.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkProperty.h>

using namespace std;

void SetActorColor(vtkRenderer* renderer, vtkActor* actor, unsigned int& id)
{
    double r = ((id & 0x000000FF) >> 0) / 255.0;
    double g = ((id & 0x0000FF00) >> 8) / 255.0;
    double b = ((id & 0x00FF0000) >> 16) / 255.0;

    vtkNew<vtkActor> cloneActor;

    // 创建映射器
    vtkNew<vtkOpenGLPolyDataMapper> mapper;
    vtkPolyData* inputData = vtkPolyData::SafeDownCast(actor->GetMapper()->GetInput());
    if (inputData)
        mapper->SetInputData(inputData);
    cloneActor->SetMapper(mapper);
    cloneActor->SetPosition(actor->GetPosition());

    // 创建着色器属性
    vtkShaderProperty* sp = cloneActor->GetShaderProperty();

    sp->SetFragmentShaderCode(
        "//VTK::System::Dec\n" // always start with this line
        "//VTK::Output::Dec\n" // always have this line in your FS
        "uniform vec3 color;\n"
        "out vec4 FragColor;\n"
        "void main () {\n"
        "  FragColor = vec4(color, 1.0);\n"
        "}\n");

    vtkSmartPointer<IShaderCallback> myCallback = vtkSmartPointer<IShaderCallback>::New();
    myCallback->SetParam("color", vtkVector3f(r, g, b));
    mapper->AddObserver(vtkCommand::UpdateShaderEvent, myCallback);

    renderer->AddActor(cloneActor);
    id++;
}

int main()
{
    unsigned int id = 4096;
    
    // 创建normalRenderer和离屏offScreenRenderer
    vtkNew<vtkRenderer> normalRenderer;
    normalRenderer->SetViewport(0, 0.0, 1, 1);
    normalRenderer->SetBackground(0.1, 0.2, 0.4);

    vtkNew<vtkRenderer> offScreenRenderer;
    offScreenRenderer->SetViewport(0.0, 0.0, 1, 1);
    offScreenRenderer->SetBackground(0.0, 0, 0);

    // 创建圆锥数据
    vtkNew<vtkConeSource> cone;
    cone->SetHeight(20);        
    cone->SetRadius(20);
    cone->SetResolution(64);    

    vtkNew<vtkPolyDataMapper> coneMapper;
    coneMapper->SetInputConnection(cone->GetOutputPort());

    vtkNew<vtkActor> coneActor;
    coneActor->SetMapper(coneMapper);
    coneActor->GetProperty()->SetOpacity(0.5);
    // 创建立方体
    vtkNew<vtkCubeSource> cube;
    cube->SetXLength(40);
    cube->SetYLength(50);
    cube->SetZLength(60);
    vtkNew<vtkPolyDataMapper> cubeMapper;
    cubeMapper->SetInputConnection(cube->GetOutputPort());
    vtkNew<vtkActor> cubeActor;
    cubeActor->SetMapper(cubeMapper);
    cubeActor->SetPosition(40, 0, 0);

    //normalRenderer添加物体
    normalRenderer->AddActor(coneActor);
    normalRenderer->AddActor(cubeActor);

    //离屏添加复制物体,并添加颜色ID
    SetActorColor(offScreenRenderer, coneActor, id);
    SetActorColor(offScreenRenderer, cubeActor, id);

    vtkNew<vtkRenderWindow> offScreenWindow;
    offScreenWindow->AddRenderer(offScreenRenderer);
    offScreenWindow->SetMultiSamples(0);

    // 启用离屏渲染
    offScreenWindow->SetUseOffScreenBuffers(true);
    offScreenWindow->SetOffScreenRendering(true);

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(normalRenderer);
    normalRenderer->GetActiveCamera()->ParallelProjectionOn();
    normalRenderer->ResetCamera();
    normalRenderer->SetUseDepthPeeling(true);

    renderWindow->SetSize(800, 600);
    renderWindow->Render();

    offScreenRenderer->SetActiveCamera(normalRenderer->GetActiveCamera());

    offScreenWindow->SetSize(800, 600);
    offScreenWindow->Render();

    // 创建相机观察者
    vtkNew<CameraObserver> cameraObserver;
    cameraObserver->offScreenRenderer = offScreenRenderer;
    cameraObserver->offScreenWindow = offScreenWindow;

    // 将相机观察者添加到 normalRenderer 的相机
    normalRenderer->GetActiveCamera()->AddObserver(vtkCommand::ModifiedEvent, cameraObserver);

    //添加鼠标交互  
    vtkNew<vtkRenderWindowInteractor> interactor;
    interactor->SetRenderWindow(renderWindow);

    vtkNew<MouseInteractorStyle> style;
    style->SetDefaultRenderer(normalRenderer);
    style->SetOffScrrenWindow(offScreenWindow);
    interactor->SetInteractorStyle(style);

    //初始化交互器 并开始执行事件循环  
    interactor->Initialize();
    interactor->Start();

	return 0;
}
