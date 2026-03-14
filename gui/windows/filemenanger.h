#pragma once

#include <imgui.h>
#include <string>
#include <vector>
#include <filesystem>
#include <stack>
#include <algorithm>
#include <ctime>

namespace MirulitGUI {
    class FileMenanger {
    private:
        std::string currentPath;
        std::string rootPath;
        std::vector<std::string> items;
        std::vector<std::string> directories;
        std::vector<std::string> files;
        std::string selectedItem;
        bool showHidden = false;
        std::stack<std::string> pathHistory;
        std::stack<std::string> forwardHistory;
        
        void RefreshDirectory() {
            items.clear();
            directories.clear();
            files.clear();
            
            try {
                for (const auto& entry : std::filesystem::directory_iterator(currentPath)) {
                    std::string filename = entry.path().filename().string();
                    
                    if (!showHidden && filename[0] == '.')
                        continue;
                    
                    if (std::filesystem::is_directory(entry.path())) {
                        directories.push_back(filename);
                    } else {
                        files.push_back(filename);
                    }
                }
                
                std::sort(directories.begin(), directories.end());
                std::sort(files.begin(), files.end());
                
                items.clear();
                items.insert(items.end(), directories.begin(), directories.end());
                items.insert(items.end(), files.begin(), files.end());
                
            } catch (const std::filesystem::filesystem_error& e) {
                items.clear();
            }
        }
        
        bool IsPathInRoot(const std::string& path) const {
            try {
                std::filesystem::path targetPath = std::filesystem::canonical(path);
                std::filesystem::path rootPath_canonical = std::filesystem::canonical(rootPath);
                
                auto rel = std::filesystem::relative(targetPath, rootPath_canonical);
                return !rel.empty() && rel.native()[0] != '.';
            } catch (...) {
                return false;
            }
        }
        
        void NavigateTo(const std::string& path) {
            try {
                if (std::filesystem::exists(path) && std::filesystem::is_directory(path)) {
                    std::string canonicalPath = std::filesystem::canonical(path).string();
                    
                    if (IsPathInRoot(canonicalPath)) {
                        pathHistory.push(currentPath);
                        while (!forwardHistory.empty()) forwardHistory.pop();
                        
                        currentPath = canonicalPath;
                        RefreshDirectory();
                        selectedItem = ""; // Сбрасываем выделение при переходе
                    }
                }
            } catch (const std::filesystem::filesystem_error& e) {
                // Игнорируем ошибки навигации
            }
        }
        
        std::string FormatTime(const std::filesystem::file_time_type& ft) const {
            auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                ft - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());
            std::time_t tt = std::chrono::system_clock::to_time_t(sctp);
            std::tm* tm = std::localtime(&tt);
            
            char buffer[64];
            std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm);
            return std::string(buffer);
        }
        
    public:
        FileMenanger() {
            std::filesystem::path exePath = std::filesystem::current_path();
            
            std::filesystem::path searchPath = exePath;
            while (searchPath.has_parent_path()) {
                bool hasSrc = std::filesystem::exists(searchPath / "src");
                bool hasAssets = std::filesystem::exists(searchPath / "assets");
                bool hasBin = std::filesystem::exists(searchPath / "bin");
                
                if (hasSrc || hasAssets || hasBin) {
                    rootPath = searchPath.string();
                    break;
                }
                searchPath = searchPath.parent_path();
            }
            
            if (rootPath.empty()) {
                rootPath = exePath.string();
            }
            
            currentPath = rootPath;
            RefreshDirectory();
        }
        
        void FileMenangerUpdate() {
            ImGui::Begin("File Manager", nullptr, ImGuiWindowFlags_MenuBar);
            
            if (ImGui::BeginMenuBar()) {
                if (ImGui::BeginMenu("View")) {
                    ImGui::MenuItem("Show Hidden", nullptr, &showHidden);
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }
            
            // Back button
            ImGui::BeginDisabled(pathHistory.empty());
            if (ImGui::ArrowButton("##back", ImGuiDir_Left)) {
                forwardHistory.push(currentPath);
                currentPath = pathHistory.top();
                pathHistory.pop();
                RefreshDirectory();
                selectedItem = "";
            }
            ImGui::EndDisabled();
            
            ImGui::SameLine();
            
            // Forward button
            ImGui::BeginDisabled(forwardHistory.empty());
            if (ImGui::ArrowButton("##forward", ImGuiDir_Right)) {
                pathHistory.push(currentPath);
                currentPath = forwardHistory.top();
                forwardHistory.pop();
                RefreshDirectory();
                selectedItem = "";
            }
            ImGui::EndDisabled();
            
            ImGui::SameLine();
            
            // Up button
            bool canGoUp = false;
            try {
                std::filesystem::path parent = std::filesystem::path(currentPath).parent_path();
                if (parent != currentPath) {
                    std::string parentStr = parent.string();
                    canGoUp = IsPathInRoot(parentStr);
                }
            } catch (...) {}
            
            ImGui::BeginDisabled(!canGoUp);
            if (ImGui::Button("Up")) {
                std::filesystem::path parent = std::filesystem::path(currentPath).parent_path();
                if (parent != currentPath) {
                    NavigateTo(parent.string());
                }
            }
            ImGui::EndDisabled();
            
            ImGui::SameLine();
            
            // Root button - быстрое возвращение в корень проекта
            if (ImGui::Button("Root")) {
                if (currentPath != rootPath) {
                    NavigateTo(rootPath);
                }
            }
            
            ImGui::SameLine();
            
            // Current path display
            std::string displayPath = currentPath;
            if (displayPath.find(rootPath) == 0) {
                displayPath.replace(0, rootPath.length(), "~");
            }
            ImGui::Text("Path: %s", displayPath.c_str());
            
            // Refresh button
            ImGui::SameLine();
            if (ImGui::Button("Refresh")) {
                RefreshDirectory();
            }
            
            ImGui::Separator();
            
            // Files list
            ImGui::BeginChild("Files", ImVec2(0, -60), true);
            
            if (ImGui::BeginTable("FileTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 80.0f);
                ImGui::TableHeadersRow();
                
                for (const auto& item : items) {
                    std::string fullPath = currentPath + "/" + item;
                    bool isDirectory = std::filesystem::is_directory(fullPath);
                    
                    ImGui::TableNextRow();
                    
                    // Name column with icon
                    ImGui::TableSetColumnIndex(0);
                    
                    if (isDirectory) {
                        ImGui::Text("📁 ");
                    } else {
                        std::string ext = std::filesystem::path(item).extension().string();
                        if (ext == ".cpp" || ext == ".h" || ext == ".c" || ext == ".hpp") {
                            ImGui::Text("📝 ");
                        } else if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".bmp" || ext == ".tga") {
                            ImGui::Text("🖼️ ");
                        } else if (ext == ".ini" || ext == ".cfg" || ext == ".json") {
                            ImGui::Text("⚙️ ");
                        } else if (ext == ".exe" || ext == ".dll" || ext == ".so") {
                            ImGui::Text("⚡ ");
                        } else {
                            ImGui::Text("📄 ");
                        }
                    }
                    ImGui::SameLine();
                    
                    bool isSelected = (selectedItem == item);
                    
                    // Используем pushID для уникальности
                    ImGui::PushID(item.c_str());
                    
                    if (ImGui::Selectable(item.c_str(), &isSelected, ImGuiSelectableFlags_SpanAllColumns)) {
                        selectedItem = item;
                        
                        // Переход по ОДИНАРНОМУ клику для папок
                        if (isDirectory) {
                            if (IsPathInRoot(fullPath)) {
                                NavigateTo(fullPath);
                            }
                        }
                    }
                    
                    ImGui::PopID();
                    
                    // Type column
                    ImGui::TableSetColumnIndex(1);
                    if (isDirectory) {
                        ImGui::Text("Folder");
                    } else {
                        std::string ext = std::filesystem::path(item).extension().string();
                        if (!ext.empty()) {
                            if (ext[0] == '.') ext.erase(0, 1);
                            ImGui::Text("%s", ext.empty() ? "File" : ext.c_str());
                        } else {
                            ImGui::Text("File");
                        }
                    }
                }
                
                ImGui::EndTable();
            }
            
            ImGui::EndChild();
            
            if (!selectedItem.empty()) {
                ImGui::Separator();
                std::string fullPath = currentPath + "/" + selectedItem;
                try {
                    if (std::filesystem::exists(fullPath)) {
                        auto size = std::filesystem::file_size(fullPath);
                        auto lastWrite = std::filesystem::last_write_time(fullPath);
                        
                        ImGui::Text("Selected: %s", selectedItem.c_str());
                        
                        if (size < 1024) {
                            ImGui::Text("Size: %lld B", size);
                        } else if (size < 1024 * 1024) {
                            ImGui::Text("Size: %.2f KB", size / 1024.0f);
                        } else if (size < 1024 * 1024 * 1024) {
                            ImGui::Text("Size: %.2f MB", size / (1024.0f * 1024.0f));
                        } else {
                            ImGui::Text("Size: %.2f GB", size / (1024.0f * 1024.0f * 1024.0f));
                        }
                        
                        ImGui::Text("Modified: %s", FormatTime(lastWrite).c_str());
                        
                        // Добавляем кнопку для открытия папки/файла
                        if (IsSelectedDirectory()) {
                            ImGui::SameLine();
                            if (ImGui::Button("Open")) {
                                NavigateTo(fullPath);
                            }
                        }
                    }
                } catch (const std::exception& e) {
                    ImGui::Text("Selected: %s (Error reading info)", selectedItem.c_str());
                }
            }
            
            ImGui::End();
        }
        
        std::string GetCurrentPath() const {
            return currentPath;
        }
        
        std::string GetRootPath() const {
            return rootPath;
        }
        
        std::string GetSelectedPath() const {
            if (selectedItem.empty()) return "";
            return currentPath + "/" + selectedItem;
        }
        
        bool IsSelectedDirectory() const {
            if (selectedItem.empty()) return false;
            std::string fullPath = currentPath + "/" + selectedItem;
            return std::filesystem::is_directory(fullPath);
        }
        
        bool IsInRoot() const {
            return IsPathInRoot(currentPath);
        }
    };
    
    class Inspector {
    public:
        void Render(void* selectedObject) {
            ImGui::Begin("Inspector");
            
            if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Text("Position: ");
                ImGui::SameLine();
                ImGui::DragFloat2("##pos", position, 0.1f);
                
                ImGui::Text("Scale:    ");
                ImGui::SameLine();
                ImGui::DragFloat2("##scale", scale, 0.1f);
                
                ImGui::Text("Rotation: ");
                ImGui::SameLine();
                ImGui::DragFloat("##rot", &rotation, 0.1f);
            }
            
            ImGui::End();
        }
        
    private:
        float position[2] = {0, 0};
        float scale[2] = {1, 1};
        float rotation = 0;
    };
    
    class SceneHierarchy {
    public:
        void Render() {
            ImGui::Begin("Scene Hierarchy");
            
            static int selected = -1;
            for (int i = 0; i < 10; i++) {
                char label[32];
                sprintf(label, "Entity %d", i);
                
                ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
                if (selected == i)
                    flags |= ImGuiTreeNodeFlags_Selected;
                    
                bool open = ImGui::TreeNodeEx((void*)(intptr_t)i, flags, label);
                if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
                    selected = i;
                    
                if (open) {
                    ImGui::Text("Children...");
                    ImGui::TreePop();
                }
            }
            
            ImGui::End();
        }
    };
}