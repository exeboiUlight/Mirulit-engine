// editor.h
#pragma once

#include "windows/filemenanger.h"
#include <imgui.h>

namespace MirulitGUI {
    class Editor {
    private:
        FileMenanger fileMenanger;
        Inspector inspector;
        SceneHierarchy hierarchy;
        
        bool showFileMenanger = true;
        bool showInspector = true;
        bool showHierarchy = true;
        bool showViewport = true;
        
        ImGuiID dockspace_id;
        
    public:
        void SetupDockSpace() {
            ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->Pos);
            ImGui::SetNextWindowSize(viewport->Size);
            ImGui::SetNextWindowViewport(viewport->ID);
            
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | 
                                           ImGuiWindowFlags_NoDocking |
                                           ImGuiWindowFlags_NoTitleBar |
                                           ImGuiWindowFlags_NoCollapse |
                                           ImGuiWindowFlags_NoResize |
                                           ImGuiWindowFlags_NoMove |
                                           ImGuiWindowFlags_NoBringToFrontOnFocus |
                                           ImGuiWindowFlags_NoNavFocus;
            
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            ImGui::Begin("DockSpace", nullptr, window_flags);
            ImGui::PopStyleVar(2);
            
            dockspace_id = ImGui::GetID("MainDockspace");
            ImGui::DockSpace(dockspace_id);
        }
        
        void Render() {
            SetupDockSpace();
            
            // Menu bar
            if (ImGui::BeginMenuBar()) {
                if (ImGui::BeginMenu("File")) {
                    if (ImGui::MenuItem("New Project")) {}
                    if (ImGui::MenuItem("Open Project")) {}
                    if (ImGui::MenuItem("Save Project")) {}
                    ImGui::Separator();
                    if (ImGui::MenuItem("Exit")) {}
                    ImGui::EndMenu();
                }
                
                if (ImGui::BeginMenu("View")) {
                    ImGui::MenuItem("File Manager", nullptr, &showFileMenanger);
                    ImGui::MenuItem("Inspector", nullptr, &showInspector);
                    ImGui::MenuItem("Hierarchy", nullptr, &showHierarchy);
                    ImGui::MenuItem("Viewport", nullptr, &showViewport);
                    ImGui::EndMenu();
                }
                
                if (ImGui::BeginMenu("Tools")) {
                    if (ImGui::MenuItem("Settings")) {}
                    if (ImGui::MenuItem("Console")) {}
                    ImGui::EndMenu();
                }
                
                ImGui::EndMenuBar();
            }
            
            // Render windows
            if (showFileMenanger) {
                fileMenanger.FileMenangerUpdate();
            }
            
            if (showInspector) {
                inspector.Render(nullptr);
            }
            
            if (showHierarchy) {
                hierarchy.Render();
            }
            
            if (showViewport) {
                ImGui::Begin("Viewport");
                // Здесь можно рендерить сцену в текстуру
                ImVec2 viewportSize = ImGui::GetContentRegionAvail();
                ImGui::Image((ImTextureID)1, viewportSize); // Ваша текстура рендера
                ImGui::End();
            }
            
            ImGui::End(); // DockSpace
        }
    };
}