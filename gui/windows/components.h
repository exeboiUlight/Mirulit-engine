#pragma once

#include <imgui.h>
#include <vector>
#include <string>

namespace MirulitGUI {
    class Console {
    private:
        std::vector<std::string> messages;
        bool autoScroll = true;
        char inputBuffer[256] = "";
        
    public:
        void AddLog(const char* fmt, ...) {
            // Добавление логов
        }
        
        void Render() {
            ImGui::Begin("Console");
            
            // Опции
            if (ImGui::Button("Clear")) messages.clear();
            ImGui::SameLine();
            ImGui::Checkbox("Auto-scroll", &autoScroll);
            
            // Лог область
            ImGui::BeginChild("ScrollingRegion", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()));
            for (const auto& msg : messages) {
                ImGui::TextUnformatted(msg.c_str());
            }
            if (autoScroll) ImGui::SetScrollHereY(1.0f);
            ImGui::EndChild();
            
            // Командная строка
            if (ImGui::InputText("Input", inputBuffer, IM_ARRAYSIZE(inputBuffer),
                                 ImGuiInputTextFlags_EnterReturnsTrue)) {
                // Обработка команды
                AddLog("> %s", inputBuffer);
                inputBuffer[0] = '\0';
            }
            
            ImGui::End();
        }
    };
    
    class AssetBrowser {
    public:
        void Render() {
            ImGui::Begin("Asset Browser");
            
            // Иконки для разных типов ассетов
            static float thumbnailSize = 64.0f;
            ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 32, 128);
            
            float windowWidth = ImGui::GetContentRegionAvail().x;
            int columns = (int)(windowWidth / thumbnailSize);
            if (columns < 1) columns = 1;
            
            ImGui::Columns(columns, nullptr, false);
            
            // Пример ассетов
            for (int i = 0; i < 20; i++) {
                // Кнопка с иконкой
                ImGui::Button("Asset", ImVec2(thumbnailSize, thumbnailSize));
                
                // Текст под иконкой
                char name[32];
                sprintf(name, "asset_%d", i);
                ImGui::TextWrapped("%s", name);
                
                ImGui::NextColumn();
            }
            
            ImGui::Columns(1);
            ImGui::End();
        }
    };
}