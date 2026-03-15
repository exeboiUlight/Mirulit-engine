#pragma once

#include <imgui.h>
#include <imgui_internal.h>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <ctime>
#include <algorithm>
#include <chrono>
#include <random>

#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#endif

namespace MirulitGUI {

// Структура для информации о проекте
struct ProjectInfo {
    std::string name;
    std::string path;
    std::string lastOpened;
    std::string author;
    std::string createdAt;
};

// Класс для окна выбора папки
class FolderDialog {
private:
    std::string currentPath;
    std::string selectedPath;
    std::vector<std::string> drives;
    std::vector<std::string> folders;
    bool isOpen = false;
    char newFolderName[128] = "";
    bool showNewFolderInput = false;
    
    void RefreshFolders() {
        folders.clear();
        try {
            for (const auto& entry : std::filesystem::directory_iterator(currentPath)) {
                if (std::filesystem::is_directory(entry.path())) {
                    folders.push_back(entry.path().filename().string());
                }
            }
            std::sort(folders.begin(), folders.end());
        } catch (...) {
            // Игнорируем ошибки доступа
        }
    }
    
    void RefreshDrives() {
        drives.clear();
        #ifdef _WIN32
        // Windows drives
        DWORD drivesMask = GetLogicalDrives();
        for (char drive = 'A'; drive <= 'Z'; drive++) {
            if (drivesMask & 1) {
                std::string root = std::string(1, drive) + ":\\";
                drives.push_back(root);
            }
            drivesMask >>= 1;
        }
        #else
        // Unix-like systems
        drives.push_back("/");
        drives.push_back("/home");
        drives.push_back("/usr");
        #endif
    }
    
public:
    FolderDialog() {
        #ifdef _WIN32
        char buffer[MAX_PATH];
        GetCurrentDirectoryA(MAX_PATH, buffer);
        currentPath = buffer;
        #else
        currentPath = std::filesystem::current_path().string();
        #endif
        RefreshDrives();
        RefreshFolders();
    }
    
    void Open() {
        isOpen = true;
        selectedPath = "";
        showNewFolderInput = false;
        newFolderName[0] = '\0';
    }
    
    bool IsOpen() const { return isOpen; }
    
    bool Render(std::string& result) {
        if (!isOpen) return false;
        
        ImGui::OpenPopup("Select Folder");
        ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
        
        bool selected = false;
        
        if (ImGui::BeginPopupModal("Select Folder", &isOpen, ImGuiWindowFlags_NoResize)) {
            // Верхняя панель с навигацией
            ImGui::BeginChild("NavBar", ImVec2(0, 30), false);
            
            // Кнопка "Назад"
            if (!drives.empty()) {
                ImGui::BeginDisabled(currentPath == drives[0]);
                if (ImGui::ArrowButton("##back", ImGuiDir_Left)) {
                    std::filesystem::path parent = std::filesystem::path(currentPath).parent_path();
                    if (parent != currentPath) {
                        currentPath = parent.string();
                        RefreshFolders();
                    }
                }
                ImGui::EndDisabled();
            }
            
            ImGui::SameLine();
            
            // Кнопка "Вверх"
            if (!drives.empty()) {
                ImGui::BeginDisabled(currentPath == drives[0]);
                if (ImGui::Button("Up")) {
                    std::filesystem::path parent = std::filesystem::path(currentPath).parent_path();
                    if (parent != currentPath) {
                        currentPath = parent.string();
                        RefreshFolders();
                    }
                }
                ImGui::EndDisabled();
            }
            
            ImGui::SameLine();
            
            // Текущий путь
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - 100);
            
            std::string displayPath = currentPath;
            ImGui::InputText("##path", (char*)displayPath.c_str(), displayPath.size() + 1, ImGuiInputTextFlags_ReadOnly);
            
            ImGui::PopItemWidth();
            
            ImGui::SameLine();
            
            // Кнопка создания новой папки
            if (ImGui::Button("+ New")) {
                showNewFolderInput = !showNewFolderInput;
            }
            
            ImGui::EndChild();
            
            // Поле для ввода имени новой папки
            if (showNewFolderInput) {
                ImGui::BeginChild("NewFolder", ImVec2(0, 40), true);
                ImGui::SetCursorPosY(8);
                ImGui::Text("Name:");
                ImGui::SameLine();
                ImGui::PushItemWidth(250);
                if (ImGui::InputText("##newfolder", newFolderName, IM_ARRAYSIZE(newFolderName), 
                                     ImGuiInputTextFlags_EnterReturnsTrue)) {
                    if (strlen(newFolderName) > 0) {
                        std::string newPath = currentPath + "/" + newFolderName;
                        try {
                            std::filesystem::create_directory(newPath);
                            RefreshFolders();
                            showNewFolderInput = false;
                            newFolderName[0] = '\0';
                        } catch (...) {}
                    }
                }
                ImGui::PopItemWidth();
                ImGui::SameLine();
                if (ImGui::Button("Cancel")) {
                    showNewFolderInput = false;
                    newFolderName[0] = '\0';
                }
                ImGui::EndChild();
            }
            
            ImGui::Separator();
            
            // Список дисков (для Windows)
            if (drives.size() > 1) {
                ImGui::BeginChild("Drives", ImVec2(0, 40), true);
                float posX = 5;
                for (const auto& drive : drives) {
                    ImGui::SetCursorPosX(posX);
                    if (ImGui::Button(drive.c_str())) {
                        currentPath = drive;
                        RefreshFolders();
                    }
                    posX += ImGui::GetItemRectSize().x + 5;
                }
                ImGui::EndChild();
                ImGui::Separator();
            }
            
            // Список папок
            ImGui::BeginChild("Folders", ImVec2(0, -60), true);
            
            for (const auto& folder : folders) {
                std::string fullPath = currentPath + "/" + folder;
                bool isSelected = (selectedPath == fullPath);
                
                ImGui::PushID(folder.c_str());
                
                ImVec4 color = isSelected ? ImVec4(0.26f, 0.52f, 0.96f, 1.0f) : 
                              (ImGui::IsWindowHovered() ? ImVec4(0.2f, 0.2f, 0.3f, 1.0f) : ImVec4(0.15f, 0.15f, 0.2f, 1.0f));
                
                ImGui::PushStyleColor(ImGuiCol_Button, color);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.4f, 1.0f));
                
                if (ImGui::Button(("📁 " + folder).c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 25))) {
                    if (ImGui::GetIO().KeyCtrl) {
                        // Ctrl+клик - выбрать папку
                        selectedPath = fullPath;
                    } else {
                        // Обычный клик - войти в папку
                        currentPath = fullPath;
                        RefreshFolders();
                    }
                }
                
                ImGui::PopStyleColor(2);
                
                // Подсказка
                if (ImGui::IsItemHovered()) {
                    ImGui::BeginTooltip();
                    ImGui::Text("Click: Open folder");
                    ImGui::Text("Ctrl+Click: Select this folder");
                    ImGui::EndTooltip();
                }
                
                ImGui::PopID();
            }
            
            ImGui::EndChild();
            
            // Нижняя панель с кнопками
            ImGui::BeginChild("BottomBar", ImVec2(0, 40), false);
            
            ImGui::SetCursorPosY(5);
            
            if (!selectedPath.empty()) {
                ImGui::TextColored(ImVec4(0.3f, 0.8f, 0.3f, 1.0f), "Selected: %s", 
                                  std::filesystem::path(selectedPath).filename().string().c_str());
            }
            
            ImGui::SameLine(ImGui::GetContentRegionAvail().x - 160);
            
            if (ImGui::Button("Cancel", ImVec2(70, 25))) {
                isOpen = false;
            }
            
            ImGui::SameLine();
            
            if (ImGui::Button("Select", ImVec2(70, 25))) {
                if (!selectedPath.empty()) {
                    result = selectedPath;
                    selected = true;
                    isOpen = false;
                } else {
                    // Если ничего не выбрано, используем текущую папку
                    result = currentPath;
                    selected = true;
                    isOpen = false;
                }
            }
            
            ImGui::EndChild();
            
            ImGui::EndPopup();
        }
        
        return selected;
    }
};

class Hub {
private:
    enum HubPage {
        PAGE_WELCOME,
        PAGE_NEW_PROJECT,
        PAGE_OPEN_PROJECT,
        PAGE_SETTINGS
    };
    
    HubPage currentPage = PAGE_WELCOME;
    std::vector<ProjectInfo> recentProjects;
    FolderDialog folderDialog;
    
    // Для нового проекта
    char newProjectName[128] = "";
    char newProjectPath[256] = "";
    char newProjectAuthor[64] = "";
    char selectedTemplate[64] = "";  // Добавляем переменную для выбранного шаблона
    
    // Для настроек
    bool darkTheme = true;
    float uiScale = 1.0f;
    bool autoSave = true;
    char defaultAuthor[64] = "User";
    
    // Состояние
    int hoveredProject = -1;
    float loadingProgress = 0.0f;
    bool isLoading = false;
    
    // Для анимации текста
    float timeAccumulator = 0.0f;
    std::vector<std::string> tips = {
        "Did you know? Mirulit Engine supports real-time editing!",
        "Tip: Press F5 to run your project",
        "Tip: Use Ctrl+S to save your files",
        "Did you know? You can create 2D games easily with Mirulit",
        "Tip: Check out the documentation for more features",
        "Did you know? Mirulit uses JIT compilation for fast iteration",
        "Tip: Use GL_TRIANGLES for better performance",
        "Did you know? You can create custom shaders",
        "Tip: Organize your assets in the assets folder",
        "Did you know? Mirulit supports both C and C++"
    };
    int currentTip = 0;
    
    void UpdateThemeColors() {
        ImGuiStyle& style = ImGui::GetStyle();
        
        if (darkTheme) {
            ImVec4* colors = style.Colors;
            colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.08f, 0.12f, 1.0f);
            colors[ImGuiCol_ChildBg] = ImVec4(0.12f, 0.12f, 0.16f, 1.0f);
            colors[ImGuiCol_TitleBgActive] = ImVec4(0.26f, 0.52f, 0.96f, 1.0f);
            colors[ImGuiCol_Text] = ImVec4(0.95f, 0.95f, 0.98f, 1.0f);
            colors[ImGuiCol_TextDisabled] = ImVec4(0.65f, 0.65f, 0.70f, 1.0f);
            colors[ImGuiCol_Border] = ImVec4(0.20f, 0.20f, 0.25f, 1.0f);
            colors[ImGuiCol_Button] = ImVec4(0.26f, 0.52f, 0.96f, 1.0f);
            colors[ImGuiCol_ButtonHovered] = ImVec4(0.36f, 0.62f, 1.0f, 1.0f);
            colors[ImGuiCol_ButtonActive] = ImVec4(0.16f, 0.42f, 0.86f, 1.0f);
            colors[ImGuiCol_FrameBg] = ImVec4(0.12f, 0.12f, 0.16f, 1.0f);
            colors[ImGuiCol_FrameBgHovered] = ImVec4(0.16f, 0.16f, 0.20f, 1.0f);
            colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.20f, 0.24f, 1.0f);
        } else {
            ImVec4* colors = style.Colors;
            colors[ImGuiCol_WindowBg] = ImVec4(0.94f, 0.94f, 0.96f, 1.0f);
            colors[ImGuiCol_ChildBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.0f);
            colors[ImGuiCol_TitleBgActive] = ImVec4(0.20f, 0.44f, 0.86f, 1.0f);
            colors[ImGuiCol_Text] = ImVec4(0.12f, 0.12f, 0.14f, 1.0f);
            colors[ImGuiCol_TextDisabled] = ImVec4(0.40f, 0.40f, 0.45f, 1.0f);
            colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.85f, 1.0f);
            colors[ImGuiCol_Button] = ImVec4(0.20f, 0.44f, 0.86f, 1.0f);
            colors[ImGuiCol_ButtonHovered] = ImVec4(0.30f, 0.54f, 0.96f, 1.0f);
            colors[ImGuiCol_ButtonActive] = ImVec4(0.10f, 0.34f, 0.76f, 1.0f);
            colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.0f);
            colors[ImGuiCol_FrameBgHovered] = ImVec4(0.96f, 0.96f, 0.98f, 1.0f);
            colors[ImGuiCol_FrameBgActive] = ImVec4(0.92f, 0.92f, 0.95f, 1.0f);
        }
        
        style.WindowRounding = 12.0f;
        style.ChildRounding = 10.0f;
        style.FrameRounding = 8.0f;
        style.PopupRounding = 10.0f;
        style.ScrollbarRounding = 8.0f;
        style.GrabRounding = 8.0f;
        style.TabRounding = 8.0f;
        style.WindowBorderSize = 1.0f;
        style.ChildBorderSize = 1.0f;
        style.FrameBorderSize = 0.0f;
        style.PopupBorderSize = 1.0f;
        style.TabBorderSize = 0.0f;
        style.WindowPadding = ImVec2(15, 15);
        style.FramePadding = ImVec2(8, 6);
        style.ItemSpacing = ImVec2(10, 8);
        style.ItemInnerSpacing = ImVec2(8, 6);
    }
    
    void LoadRecentProjects() {
        recentProjects.clear();
        
        std::string configPath;
        #ifdef _WIN32
        char appData[MAX_PATH];
        SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, appData);
        configPath = std::string(appData) + "\\MirulitHub";
        #else
        configPath = std::string(getenv("HOME")) + "/.config/MirulitHub";
        #endif
        
        std::filesystem::create_directories(configPath);
        
        std::string recentFile = configPath + "/recent.txt";
        std::ifstream file(recentFile);
        
        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                std::vector<std::string> parts;
                size_t start = 0;
                size_t end = line.find('|');
                while (end != std::string::npos) {
                    parts.push_back(line.substr(start, end - start));
                    start = end + 1;
                    end = line.find('|', start);
                }
                parts.push_back(line.substr(start));
                
                if (parts.size() >= 4) {
                    ProjectInfo info;
                    info.name = parts[0];
                    info.path = parts[1];
                    info.lastOpened = parts[2];
                    info.author = parts[3];
                    info.createdAt = (parts.size() > 4) ? parts[4] : "Unknown";
                    
                    std::string projFile = info.path + "/" + info.name + ".mirulitproject";
                    if (std::filesystem::exists(projFile)) {
                        recentProjects.push_back(info);
                    }
                }
            }
            file.close();
        }
        
        if (recentProjects.empty()) {
            ProjectInfo example;
            example.name = "Example Project";
            example.path = std::filesystem::current_path().string() + "/projects/Example";
            example.lastOpened = "Never";
            example.author = "System";
            example.createdAt = "2024-01-01 12:00";
            recentProjects.push_back(example);
        }
    }
    
    void SaveRecentProject(const ProjectInfo& info) {
        auto it = std::find_if(recentProjects.begin(), recentProjects.end(),
            [&info](const ProjectInfo& p) { return p.path == info.path; });
        
        if (it != recentProjects.end()) {
            recentProjects.erase(it);
        }
        
        recentProjects.insert(recentProjects.begin(), info);
        
        if (recentProjects.size() > 10) {
            recentProjects.pop_back();
        }
        
        std::string configPath;
        #ifdef _WIN32
        char appData[MAX_PATH];
        SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, appData);
        configPath = std::string(appData) + "\\MirulitHub";
        #else
        configPath = std::string(getenv("HOME")) + "/.config/MirulitHub";
        #endif
        
        std::filesystem::create_directories(configPath);
        
        std::ofstream file(configPath + "/recent.txt");
        if (file.is_open()) {
            for (const auto& proj : recentProjects) {
                file << proj.name << "|" << proj.path << "|" << proj.lastOpened << "|" 
                     << proj.author << "|" << proj.createdAt << "\n";
            }
            file.close();
        }
    }
    
    std::string GetCurrentTime() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        tm ltm;
        #ifdef _WIN32
        localtime_s(&ltm, &time_t);
        #else
        localtime_r(&time_t, &ltm);
        #endif
        
        char buffer[32];
        #ifdef _WIN32
        sprintf_s(buffer, sizeof(buffer), "%04d-%02d-%02d %02d:%02d",
                 1900 + ltm.tm_year, 1 + ltm.tm_mon, ltm.tm_mday,
                 ltm.tm_hour, ltm.tm_min);
        #else
        sprintf(buffer, "%04d-%02d-%02d %02d:%02d",
                1900 + ltm.tm_year, 1 + ltm.tm_mon, ltm.tm_mday,
                ltm.tm_hour, ltm.tm_min);
        #endif
        return std::string(buffer);
    }
    
    void CreateProjectFiles(const std::string& name, const std::string& path) {
        try {
            // Просто копируем шаблон из assets/examples/ если выбран шаблон
            if (strlen(selectedTemplate) > 0) {
                std::string templatePath = std::string("assets/examples/") + selectedTemplate;
                if (std::filesystem::exists(templatePath)) {
                    try {
                        // Копируем все файлы из шаблона
                        for (const auto& entry : std::filesystem::recursive_directory_iterator(templatePath)) {
                            if (std::filesystem::is_regular_file(entry.path())) {
                                std::string relativePath = std::filesystem::relative(entry.path(), templatePath).string();
                                std::string destPath = path + "/" + relativePath;
                                
                                // Создаем директории если нужно
                                std::filesystem::create_directories(std::filesystem::path(destPath).parent_path());
                                
                                // Копируем файл
                                std::filesystem::copy_file(entry.path(), destPath, std::filesystem::copy_options::overwrite_existing);
                            }
                        }
                        printf("Template '%s' copied successfully\n", selectedTemplate);
                    } catch (const std::exception& e) {
                        printf("Error copying template: %s\n", e.what());
                    }
                } else {
                    printf("Template '%s' not found in assets/examples/\n", selectedTemplate);
                }
            }
            
            // Создаём файл проекта с информацией о шаблоне
            std::ofstream projFile(path + "/" + name + ".mirulitproject");
            if (projFile.is_open()) {
                projFile << "[MirulitProject]\n";
                projFile << "Name = " << name << "\n";
                projFile << "Author = " << (newProjectAuthor[0] ? newProjectAuthor : "Unknown") << "\n";
                projFile << "Created = " << GetCurrentTime() << "\n";
                projFile << "LastModified = " << GetCurrentTime() << "\n";
                projFile << "Version = 1.1.0\n";
                projFile << "Template = " << (strlen(selectedTemplate) > 0 ? selectedTemplate : "none") << "\n";
                projFile.close();
            }
            
        } catch (const std::exception& e) {
            printf("Error creating project: %s\n", e.what());
        }
    }
    
    void OpenProject(const ProjectInfo& info) {
        ProjectInfo updated = info;
        updated.lastOpened = GetCurrentTime();
        SaveRecentProject(updated);
        isLoading = true;
        loadingProgress = 0.0f;
    }
    
    void RenderWelcomePage() {
        float windowWidth = ImGui::GetWindowWidth();
        float windowHeight = ImGui::GetWindowHeight();
        
        // Анимированный заголовок
        timeAccumulator += ImGui::GetIO().DeltaTime;
        if (timeAccumulator > 5.0f) {
            timeAccumulator = 0.0f;
            currentTip = (currentTip + 1) % tips.size();
        }
        
        // Заголовок с градиентом
        ImGui::SetCursorPosY(30);
        ImGui::SetCursorPosX((windowWidth - 300) * 0.5f);
        
        ImVec4 color1 = ImVec4(0.26f, 0.52f, 0.96f, 1.0f);
        ImVec4 color2 = ImVec4(0.76f, 0.32f, 0.96f, 1.0f);
        
        for (int i = 0; i < 14; i++) {
            float t = i / 13.0f;
            ImVec4 color;
            color.x = color1.x * (1 - t) + color2.x * t;
            color.y = color1.y * (1 - t) + color2.y * t;
            color.z = color1.z * (1 - t) + color2.z * t;
            color.w = 1.0f;
            
            ImGui::SameLine(0, 0);
            ImGui::TextColored(color, "%c", "Mirulit Engine"[i]);
        }
        
        ImGui::SetCursorPosX((windowWidth - 350) * 0.5f);
        ImGui::TextDisabled("Create · Code · Create");
        
        // Подсказка дня
        ImGui::SetCursorPosX((windowWidth - 400) * 0.5f);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.7f, 1.0f, 1.0f));
        ImGui::Text("💡 %s", tips[currentTip].c_str());
        ImGui::PopStyleColor();
        
        ImGui::Dummy(ImVec2(0, 30));
        
        // Кнопки с иконками
        float buttonWidth = 220;
        float startX = (windowWidth - buttonWidth * 2 - 30) * 0.5f;
        
        ImGui::SetCursorPosX(startX);
        
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.26f, 0.52f, 0.96f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.36f, 0.62f, 1.0f, 1.0f));
        if (ImGui::Button("➕  New Project", ImVec2(buttonWidth, 50))) {
            currentPage = PAGE_NEW_PROJECT;
        }
        ImGui::PopStyleColor(2);
        
        ImGui::SameLine(0, 30);
        
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.45f, 0.45f, 0.50f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.55f, 0.55f, 0.60f, 1.0f));
        if (ImGui::Button("📂  Open Project", ImVec2(buttonWidth, 50))) {
            currentPage = PAGE_OPEN_PROJECT;
        }
        ImGui::PopStyleColor(2);
        
        ImGui::Dummy(ImVec2(0, 30));
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0, 10));
        
        // Заголовок раздела
        ImGui::SetCursorPosX(50);
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.8f, 1.0f), "📌  Recent Projects");
        ImGui::Dummy(ImVec2(0, 10));
        
        // Недавние проекты
        ImGui::BeginChild("RecentProjects", ImVec2(0, 200), false, ImGuiWindowFlags_AlwaysVerticalScrollbar);
        
        for (size_t i = 0; i < recentProjects.size(); i++) {
            ImGui::PushID((int)i);
            
            bool isHovered = (hoveredProject == (int)i);
            ImVec4 cardColor = isHovered ? ImVec4(0.16f, 0.16f, 0.20f, 1.0f) : ImVec4(0.12f, 0.12f, 0.16f, 1.0f);
            
            ImGui::PushStyleColor(ImGuiCol_ChildBg, cardColor);
            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 8.0f);
            
            ImGui::BeginChild(("##proj" + std::to_string(i)).c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 80), true);
            
            if (ImGui::IsWindowHovered()) {
                hoveredProject = (int)i;
            }
            
            // Иконка проекта
            ImGui::SetCursorPos(ImVec2(15, 15));
            ImGui::TextColored(ImVec4(0.26f, 0.52f, 0.96f, 1.0f), "📁");
            
            // Название проекта
            ImGui::SameLine(40);
            ImGui::SetCursorPosY(15);
            ImGui::TextColored(ImVec4(0.9f, 0.9f, 1.0f, 1.0f), "%s", recentProjects[i].name.c_str());
            
            // Путь
            ImGui::SameLine(40);
            ImGui::SetCursorPosY(35);
            ImGui::TextDisabled("%s", recentProjects[i].path.c_str());
            
            // Дата и автор
            ImGui::SameLine(40);
            ImGui::SetCursorPosY(55);
            ImGui::TextDisabled("by %s · %s", 
                               recentProjects[i].author.c_str(),
                               recentProjects[i].lastOpened.c_str());
            
            // Кнопка открыть
            ImGui::SameLine(ImGui::GetContentRegionAvail().x - 90);
            ImGui::SetCursorPosY(25);
            
            if (ImGui::Button("Open", ImVec2(80, 30))) {
                OpenProject(recentProjects[i]);
            }
            
            ImGui::EndChild();
            
            ImGui::PopStyleVar();
            ImGui::PopStyleColor();
            ImGui::PopID();
            
            ImGui::Dummy(ImVec2(0, 5));
        }
        
        ImGui::EndChild();
        ImGui::Dummy(ImVec2(0, 10));
        
        // Нижняя панель
        ImGui::SetCursorPosY(windowHeight - 60);
        ImGui::Separator();
        ImGui::SetCursorPosY(windowHeight - 50);
        ImGui::SetCursorPosX(windowWidth - 200);
        
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.35f, 1.0f));
        if (ImGui::Button("⚙  Settings", ImVec2(150, 35))) {
            currentPage = PAGE_SETTINGS;
        }
        ImGui::PopStyleColor();
    }
    
    void RenderNewProjectPage() {
        float windowWidth = ImGui::GetWindowWidth();
        float windowHeight = ImGui::GetWindowHeight();
        
        // Заголовок
        ImGui::SetCursorPosY(30);
        ImGui::SetCursorPosX(50);
        ImGui::TextColored(ImVec4(0.26f, 0.52f, 0.96f, 1.0f), "✨  Create New Project");
        ImGui::Dummy(ImVec2(0, 20));
        
        ImGui::PushItemWidth(windowWidth - 150);
        
        // Имя проекта
        ImGui::SetCursorPosX(50);
        ImGui::TextColored(ImVec4(0.8f, 0.8f, 1.0f, 1.0f), "Project Name");
        ImGui::SetCursorPosX(50);
        ImGui::InputText("##name", newProjectName, IM_ARRAYSIZE(newProjectName));
        if (strlen(newProjectName) == 0) {
            ImGui::SameLine();
            ImGui::TextDisabled(" (required)");
        }
        ImGui::Dummy(ImVec2(0, 15));
        
        // Путь проекта
        ImGui::SetCursorPosX(50);
        ImGui::TextColored(ImVec4(0.8f, 0.8f, 1.0f, 1.0f), "Location");
        ImGui::SetCursorPosX(50);
        
        float pathWidth = windowWidth - 150 - 100;
        ImGui::PushItemWidth(pathWidth);
        ImGui::InputText("##path", newProjectPath, IM_ARRAYSIZE(newProjectPath));
        ImGui::PopItemWidth();
        
        ImGui::SameLine(0, 10);
        if (ImGui::Button("Browse", ImVec2(80, 30))) {
            folderDialog.Open();
        }
        
        // Обработка диалога выбора папки
        std::string selectedFolder;
        if (folderDialog.Render(selectedFolder)) {
            #ifdef _WIN32
            strcpy_s(newProjectPath, selectedFolder.c_str());
            #else
            strcpy(newProjectPath, selectedFolder.c_str());
            #endif
        }
        
        ImGui::Dummy(ImVec2(0, 15));
        
        // Выбор шаблона
        ImGui::SetCursorPosX(50);
        ImGui::TextColored(ImVec4(0.8f, 0.8f, 1.0f, 1.0f), "Template");
        ImGui::SetCursorPosX(50);
        
        // Получаем список доступных шаблонов
        std::vector<std::string> templates;
        templates.push_back("None");
        
        if (std::filesystem::exists("assets/examples")) {
            for (const auto& entry : std::filesystem::directory_iterator("assets/examples")) {
                if (std::filesystem::is_directory(entry.path())) {
                    templates.push_back(entry.path().filename().string());
                }
            }
        }
        
        // Преобразуем в массив для Combo
        std::vector<const char*> templateItems;
        for (const auto& t : templates) {
            templateItems.push_back(t.c_str());
        }
        
        int currentTemplate = 0;
        for (size_t i = 0; i < templates.size(); i++) {
            if (templates[i] == selectedTemplate) {
                currentTemplate = i;
                break;
            }
        }
        
        ImGui::PushItemWidth(300);
        if (ImGui::Combo("##template", &currentTemplate, templateItems.data(), templateItems.size())) {
            if (currentTemplate == 0) {
                selectedTemplate[0] = '\0';
            } else {
                #ifdef _WIN32
                strcpy_s(selectedTemplate, templates[currentTemplate].c_str());
                #else
                strcpy(selectedTemplate, templates[currentTemplate].c_str());
                #endif
            }
        }
        ImGui::PopItemWidth();
        
        ImGui::Dummy(ImVec2(0, 15));
        
        // Автор
        ImGui::SetCursorPosX(50);
        ImGui::TextColored(ImVec4(0.8f, 0.8f, 1.0f, 1.0f), "Author");
        ImGui::SetCursorPosX(50);
        ImGui::InputText("##author", newProjectAuthor, IM_ARRAYSIZE(newProjectAuthor));
        ImGui::Dummy(ImVec2(0, 15));
        
        ImGui::PopItemWidth();
        
        // Кнопки
        ImGui::SetCursorPosY(windowHeight - 80);
        ImGui::Separator();
        
        ImGui::SetCursorPosY(windowHeight - 55);
        ImGui::SetCursorPosX(50);
        
        if (ImGui::Button("◀  Cancel", ImVec2(120, 35))) {
            currentPage = PAGE_WELCOME;
        }
        
        ImGui::SameLine(windowWidth - 190);
        
        bool canCreate = strlen(newProjectName) > 0 && strlen(newProjectPath) > 0;
        
        if (canCreate) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.26f, 0.52f, 0.96f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.36f, 0.62f, 1.0f, 1.0f));
            if (ImGui::Button("✅  Create Project", ImVec2(150, 35))) {
                std::string fullPath = std::string(newProjectPath) + "/" + newProjectName;
                CreateProjectFiles(newProjectName, fullPath);
                
                ProjectInfo newProject;
                newProject.name = newProjectName;
                newProject.path = fullPath;
                newProject.lastOpened = GetCurrentTime();
                newProject.author = (newProjectAuthor[0] ? newProjectAuthor : defaultAuthor);
                newProject.createdAt = GetCurrentTime();
                
                SaveRecentProject(newProject);
                
                memset(newProjectName, 0, sizeof(newProjectName));
                selectedTemplate[0] = '\0'; // Сбрасываем выбор шаблона
                currentPage = PAGE_WELCOME;
            }
            ImGui::PopStyleColor(2);
        } else {
            ImGui::BeginDisabled();
            ImGui::Button("Create Project", ImVec2(150, 35));
            ImGui::EndDisabled();
        }
    }
    
    void RenderOpenProjectPage() {
        float windowWidth = ImGui::GetWindowWidth();
        float windowHeight = ImGui::GetWindowHeight();
        
        ImGui::SetCursorPosY(30);
        ImGui::SetCursorPosX(50);
        ImGui::TextColored(ImVec4(0.26f, 0.52f, 0.96f, 1.0f), "📂  Open Project");
        ImGui::Dummy(ImVec2(0, 10));
        
        ImGui::SetCursorPosX(50);
        ImGui::TextDisabled("Select a project to open:");
        ImGui::Dummy(ImVec2(0, 10));
        
        ImGui::BeginChild("ProjectsList", ImVec2(windowWidth - 100, windowHeight - 170), true);
        
        for (const auto& project : recentProjects) {
            ImGui::PushID(project.name.c_str());
            
            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.12f, 0.12f, 0.16f, 1.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 8.0f);
            
            ImGui::BeginChild(("##item" + project.name).c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 80), true);
            
            ImGui::SetCursorPos(ImVec2(15, 15));
            ImGui::TextColored(ImVec4(0.26f, 0.52f, 0.96f, 1.0f), "📁");
            
            ImGui::SameLine(40);
            ImGui::SetCursorPosY(15);
            ImGui::TextColored(ImVec4(0.9f, 0.9f, 1.0f, 1.0f), "%s", project.name.c_str());
            
            ImGui::SameLine(40);
            ImGui::SetCursorPosY(35);
            ImGui::TextDisabled("%s", project.path.c_str());
            
            ImGui::SameLine(40);
            ImGui::SetCursorPosY(55);
            ImGui::TextDisabled("by %s · %s", project.author.c_str(), project.lastOpened.c_str());
            
            ImGui::SameLine(ImGui::GetContentRegionAvail().x - 90);
            ImGui::SetCursorPosY(25);
            
            if (ImGui::Button("Open", ImVec2(80, 30))) {
                OpenProject(project);
            }
            
            ImGui::EndChild();
            
            ImGui::PopStyleVar();
            ImGui::PopStyleColor();
            ImGui::PopID();
            
            ImGui::Dummy(ImVec2(0, 5));
        }
        
        ImGui::EndChild();
        
        ImGui::SetCursorPosY(windowHeight - 80);
        ImGui::Separator();
        
        ImGui::SetCursorPosY(windowHeight - 55);
        ImGui::SetCursorPosX(50);
        
        if (ImGui::Button("◀  Back", ImVec2(100, 35))) {
            currentPage = PAGE_WELCOME;
        }
    }
    
    void RenderSettingsPage() {
        float windowWidth = ImGui::GetWindowWidth();
        float windowHeight = ImGui::GetWindowHeight();
        
        ImGui::SetCursorPosY(30);
        ImGui::SetCursorPosX(50);
        ImGui::TextColored(ImVec4(0.26f, 0.52f, 0.96f, 1.0f), "⚙  Settings");
        ImGui::Dummy(ImVec2(0, 20));
        
        ImGui::BeginChild("Settings", ImVec2(windowWidth - 100, windowHeight - 170), false);
        
        // Appearance
        ImGui::SetCursorPosX(50);
        ImGui::TextColored(ImVec4(0.8f, 0.8f, 1.0f, 1.0f), "Appearance");
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0, 10));
        
        ImGui::SetCursorPosX(70);
        ImGui::TextDisabled("Theme:");
        ImGui::SameLine(200);
        
        const char* themes[2] = { "🌙  Dark", "☀️  Light" };
        int currentTheme = darkTheme ? 0 : 1;
        
        if (ImGui::Combo("##theme", &currentTheme, themes, 2)) {
            darkTheme = (currentTheme == 0);
            UpdateThemeColors();
        }
        ImGui::Dummy(ImVec2(0, 10));
        
        ImGui::SetCursorPosX(70);
        ImGui::TextDisabled("UI Scale:");
        ImGui::SameLine(200);
        ImGui::SliderFloat("##scale", &uiScale, 0.8f, 1.5f, "%.1f");
        ImGui::Dummy(ImVec2(0, 20));
        
        // Editor settings
        ImGui::SetCursorPosX(50);
        ImGui::TextColored(ImVec4(0.8f, 0.8f, 1.0f, 1.0f), "Editor");
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0, 10));
        
        ImGui::SetCursorPosX(70);
        ImGui::TextDisabled("Auto-save:");
        ImGui::SameLine(200);
        ImGui::Checkbox("##autosave", &autoSave);
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Automatically save files when running the project");
        }
        ImGui::Dummy(ImVec2(0, 10));
        
        ImGui::SetCursorPosX(70);
        ImGui::TextDisabled("Default Author:");
        ImGui::SameLine(200);
        ImGui::InputText("##author", defaultAuthor, IM_ARRAYSIZE(defaultAuthor));
        ImGui::Dummy(ImVec2(0, 20));
        
        // About
        ImGui::SetCursorPosX(50);
        ImGui::TextColored(ImVec4(0.8f, 0.8f, 1.0f, 1.0f), "About");
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0, 10));
        
        ImGui::SetCursorPosX(70);
        ImGui::TextDisabled("Version:");
        ImGui::SameLine(200);
        ImGui::TextColored(ImVec4(0.3f, 0.8f, 0.3f, 1.0f), "1.0.0");
        
        ImGui::SetCursorPosX(70);
        ImGui::TextDisabled("Build:");
        ImGui::SameLine(200);
        ImGui::Text("2024.03.15");
        
        ImGui::EndChild();
        
        ImGui::SetCursorPosY(windowHeight - 80);
        ImGui::Separator();
        
        ImGui::SetCursorPosY(windowHeight - 55);
        ImGui::SetCursorPosX(50);
        
        if (ImGui::Button("◀  Back", ImVec2(100, 35))) {
            currentPage = PAGE_WELCOME;
        }
    }
    
    void RenderLoadingOverlay() {
        if (!isLoading) return;
        
        ImGui::OpenPopup("Loading");
        ImGui::SetNextWindowSize(ImVec2(300, 120));
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        
        if (ImGui::BeginPopupModal("Loading", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize)) {
            ImGui::SetCursorPosY(30);
            ImGui::SetCursorPosX(90);
            ImGui::TextColored(ImVec4(0.26f, 0.52f, 0.96f, 1.0f), "Opening Project...");
            ImGui::Dummy(ImVec2(0, 10));
            
            ImGui::SetCursorPosX(50);
            ImGui::ProgressBar(loadingProgress, ImVec2(200, 10), "");
            
            loadingProgress += 0.01f;
            if (loadingProgress >= 1.0f) {
                isLoading = false;
                ImGui::CloseCurrentPopup();
            }
            
            ImGui::EndPopup();
        }
    }
    
public:
    Hub() {
        #ifdef _WIN32
        char documents[MAX_PATH];
        SHGetFolderPathA(NULL, CSIDL_MYDOCUMENTS, NULL, 0, documents);
        std::string defaultPath = std::string(documents) + "\\MirulitProjects\\";
        strcpy_s(newProjectPath, defaultPath.c_str());
        strcpy_s(newProjectAuthor, "User");
        strcpy_s(defaultAuthor, "User");
        #else
        strcpy(newProjectPath, "./projects/");
        strcpy(newProjectAuthor, "User");
        strcpy(defaultAuthor, "User");
        #endif
        
        UpdateThemeColors();
        LoadRecentProjects();
        
        // Инициализация случайного совета
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, tips.size() - 1);
        currentTip = dis(gen);
    }
    
    void Render() {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | 
                                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::Begin("Mirulit Hub", nullptr, flags);
        ImGui::PopStyleVar(2);
        
        switch(currentPage) {
            case PAGE_WELCOME: RenderWelcomePage(); break;
            case PAGE_NEW_PROJECT: RenderNewProjectPage(); break;
            case PAGE_OPEN_PROJECT: RenderOpenProjectPage(); break;
            case PAGE_SETTINGS: RenderSettingsPage(); break;
        }
        
        RenderLoadingOverlay();
        ImGui::End();
    }
    
    bool IsLoading() const { return isLoading; }
};

}