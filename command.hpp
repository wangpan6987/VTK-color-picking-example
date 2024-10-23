#pragma once
#include <vtkCommand.h>
#include <vtkShaderProgram.h>
#include <vtkVector.h>
#include <map>
#include <variant>
#include <string>
#include <mutex>
#include <iostream>
#include <vtkCamera.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>

using ShaderParam = std::variant<bool, int, float, vtkVector3f>;
using ShaderParamMap = std::map<std::string, ShaderParam>;

class IShaderCallback : public vtkCommand
{
public:
    static IShaderCallback* New()
    {
        return new IShaderCallback();
    }

    vtkTypeMacro(IShaderCallback, vtkCommand);

    virtual void Execute(vtkObject*, unsigned long, void* calldata) override
    {
        std::lock_guard<std::mutex> lock(mutex);
        vtkShaderProgram* program = reinterpret_cast<vtkShaderProgram*>(calldata);
        if (program)
        {
            // 清空临时容器
            keysToRemove.clear();

            for (auto it = params.begin(); it != params.end(); it++)
            {
                const std::string& name = it->first;
                const ShaderParam& value = it->second;
                if (program->IsUniformUsed(name.c_str()))
                {
                    std::visit(
                        [&name, &program](const auto& arg) {
                            using T = std::decay_t<decltype(arg)>;
                            if constexpr (std::is_same_v<T, bool>)
                            {
                                program->SetUniformi(name.c_str(), arg ? 1 : 0);
                            }
                            else if constexpr (std::is_same_v<T, int>)
                            {
                                program->SetUniformi(name.c_str(), arg);
                            }
                            else if constexpr (std::is_same_v<T, float>)
                            {
                                program->SetUniformf(name.c_str(), arg);
                            }
                            else if constexpr (std::is_same_v<T, vtkVector3f>)
                            {
                                program->SetUniform3f(name.c_str(), arg.GetData());
                            }
                        },
                        value
                    );

                    keysToRemove.push_back(name);
                }
                else
                {
                    keysToRemove.push_back(name);
                }
            }
        }
    }

    virtual void SetParam(const std::string& name, const ShaderParam& value)
    {
        std::lock_guard<std::mutex> lock(mutex);
        params[name] = value;
    }

protected:
    IShaderCallback() = default;

    std::mutex mutex;
    ShaderParamMap params;

    std::vector<std::string> keysToRemove; // 临时容器用于存储需要删除的键
};

class CameraObserver : public vtkCommand
{
public:
    static CameraObserver* New()
    {
        return new CameraObserver;
    }

    void Execute(vtkObject* caller, unsigned long eventId, void* callData) override
    {
        if (vtkCamera::SafeDownCast(caller) && offScreenRenderer && offScreenWindow)
        {
            vtkCamera* camera = vtkCamera::SafeDownCast(caller);
            if (camera) {
                offScreenRenderer->SetActiveCamera(camera);
                offScreenRenderer->Render();
                offScreenWindow->Render();
            }
        }
    }

    vtkRenderer* offScreenRenderer = nullptr;
    vtkRenderWindow* offScreenWindow = nullptr;
};