#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <GLFW/glfw3.h>
#include <string>
#include <sstream>
#include <fstream>
#include <iomanip>

#include "DynamicArray.h"
#include "AnimalHashTable.h"
#include "FeedingTree.h"
#include "CircularList.h"
#include "FiltersTree.h"

// --- Глобальные настройки ---

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void SetupImGuiStyle() {
    ImGuiStyle& style = ImGui::GetStyle();

    // Основные цвета фона
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);

    // Основной цвет текста - белый
    style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f); // Белый текст

    // Заголовки окон - темно-зеленый
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.1f, 0.3f, 0.1f, 1.00f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.15f, 0.4f, 0.15f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.1f, 0.25f, 0.1f, 1.00f);

    // Кнопки - зеленые оттенки
    style.Colors[ImGuiCol_Button] = ImVec4(0.2f, 0.5f, 0.2f, 1.00f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.3f, 0.6f, 0.3f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.4f, 0.7f, 0.4f, 1.00f);

    // Заголовки секций - зеленые
    style.Colors[ImGuiCol_Header] = ImVec4(0.2f, 0.5f, 0.2f, 1.00f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.3f, 0.6f, 0.3f, 1.00f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.4f, 0.7f, 0.4f, 1.00f);

    // Скроллбары
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.3f, 0.5f, 0.3f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.4f, 0.6f, 0.4f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.5f, 0.7f, 0.5f, 1.00f);

    // Вкладки
    style.Colors[ImGuiCol_Tab] = ImVec4(0.15f, 0.3f, 0.15f, 1.00f);
    style.Colors[ImGuiCol_TabHovered] = ImVec4(0.25f, 0.4f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_TabActive] = ImVec4(0.2f, 0.5f, 0.2f, 1.00f);
    style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.1f, 0.2f, 0.1f, 1.00f);
    style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.15f, 0.3f, 0.15f, 1.00f);

    // Слайдеры
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.3f, 0.6f, 0.3f, 1.00f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.4f, 0.7f, 0.4f, 1.00f);

    // Геометрия
    style.WindowRounding = 5.0f;
    style.ChildRounding = 5.0f;
    style.FrameRounding = 3.0f;
    style.PopupRounding = 5.0f;
    style.ScrollbarRounding = 3.0f;
    style.GrabRounding = 3.0f;
    style.TabRounding = 3.0f;
    style.WindowPadding = ImVec2(10, 10);
    style.FramePadding = ImVec2(8, 4);
    style.ItemSpacing = ImVec2(8, 6);
    style.ItemInnerSpacing = ImVec2(6, 4);
    style.IndentSpacing = 20.0f;
    style.WindowBorderSize = 1.0f;
    style.ChildBorderSize = 1.0f;
    style.PopupBorderSize = 1.0f;
    style.FrameBorderSize = 1.0f;
}

// --- Вспомогательные функции ---

// Заголовок секции
void SectionHeader(const char* title) {
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.8f, 0.4f, 1.0f));
    ImGui::TextUnformatted(title);
    ImGui::PopStyleColor();
    ImGui::Separator();
    ImGui::Spacing();
}

// Проверка уникальности клички
bool isNicknameUnique(const std::string& nickname, const DynamicArray<Animal>& animals) {
    for (size_t i = 0; i < animals.size(); ++i) {
        if (animals[i].nickname == nickname) {
            return false;
        }
    }
    return true;
}

// Проверка даты в формате DD.MM.YYYY
bool isValidDate(const std::string& date) {
    if (date.length() != 10) return false;
    if (date[2] != '.' || date[5] != '.') return false;
    try {
        int day = std::stoi(date.substr(0, 2));
        int month = std::stoi(date.substr(3, 2));
        int year = std::stoi(date.substr(6, 4));
        if (day < 1 || day > 31 || month < 1 || month > 12 || year < 1900 || year > 2100) return false;
    } catch (...) {
        return false;
    }
    return true;
}

// --- Основная функция ---

int main(int, char**)
{
    system("chcp 65001");
    // ------------------------------
    // Инициализация GLFW
    // ------------------------------
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) return 1;

    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // --- Создание ДВУХ независимых окон ---
    GLFWwindow* animalWindow = glfwCreateWindow(800, 720, "Окно 1: Хеш-таблица - Животные", NULL, NULL);
    GLFWwindow* feedingWindow = glfwCreateWindow(800, 720, "Окно 2: AVL-Дерево - Кормления и Отчеты", NULL, NULL);

    if (animalWindow == NULL || feedingWindow == NULL) return 1;

    // Позиционируем окна рядом
    glfwSetWindowPos(animalWindow, 100, 100);
    glfwSetWindowPos(feedingWindow, 910, 100);

    // ----------------------------------------
    // Инициализация ImGui для КАЖДОГО окна
    // ----------------------------------------

    // --- Контекст для окна животных ---
    glfwMakeContextCurrent(animalWindow);
    IMGUI_CHECKVERSION();
    ImGuiContext* animalContext = ImGui::CreateContext();
    ImGui::SetCurrentContext(animalContext);
    ImGuiIO& animalIO = ImGui::GetIO();
    animalIO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    static const ImWchar ranges[] = { 0x0020, 0x00FF, 0x0400, 0x052F, 0 };
    animalIO.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/arial.ttf", 16.0f, nullptr, ranges);
    SetupImGuiStyle();
    ImGui_ImplGlfw_InitForOpenGL(animalWindow, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // --- Контекст для окна кормлений ---
    glfwMakeContextCurrent(feedingWindow);
    ImGuiContext* feedingContext = ImGui::CreateContext();
    ImGui::SetCurrentContext(feedingContext);
    ImGuiIO& feedingIO = ImGui::GetIO();
    feedingIO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    feedingIO.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/arial.ttf", 16.0f, nullptr, ranges);
    SetupImGuiStyle();
    ImGui_ImplGlfw_InitForOpenGL(feedingWindow, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // ------------------------------
    // ОБЩИЕ Данные и структуры "Зоопарка"
    // ------------------------------
    DynamicArray<Animal> animals;
    DynamicArray<FeedingEntry> feedings;
    AnimalHashTable animalTable(16);
    FeedingTree feedingTree;

    QuantityFiltersTree quantityTree;
    DateFiltersTree dateTree;
    FiltersTree<std::string> speciesTree;

    struct ReportResult { 
        std::string nickname; 
        std::string species; 
        int feedingCount; 
    };
    DynamicArray<ReportResult> reportResults;
    bool reportGenerated = false;

    auto rebuildAllStructures = [&]() {
        animalTable.clear();
        for (int i = 0; i < (int)animals.size(); ++i) animalTable.insert(animals[i].nickname, i);
        speciesTree.clear();
        for (int i = 0; i < (int)animals.size(); ++i) speciesTree.add(animals[i].species, i);
        feedingTree.clear();
        quantityTree.clear();
        dateTree.clear();
        for (int i = 0; i < (int)feedings.size(); ++i) {
            feedingTree.add(feedings[i].nickname, i);
            quantityTree.add(feedings[i].quantity, i);
            dateTree.add(feedings[i].date, i);
        }
    };

    // --- ОБЩИЕ Переменные состояния UI ---
    char animalsFile[256] = "../Lists/animals.txt";
    char feedingsFile[256] = "../Lists/feedings.txt";
    int initialTableSize = 16;

    char newNickname[128] = "";
    char newSpecies[128] = "";
    char newCage[128] = "";
    char feedingNickname[128] = "";
    char feedingFeedType[128] = "";
    int feedingQuantity = 1;
    char feedingDate[64] = "";

    char reportDate[64] = "15.01.2024";
    char reportSpeciesFilter[128] = "";
    int reportQuantity = 0;

    std::ostringstream debugLog;
    std::string statusMessage = "Добро пожаловать в систему управления зоопарком!";
    float statusMessageTime = 0.0f;

    // ------------------------------
    // Главный цикл рендеринга
    // ------------------------------
    while (!glfwWindowShouldClose(animalWindow) && !glfwWindowShouldClose(feedingWindow))
    {
        glfwPollEvents();

        // =================================================================================
        // ОБРАБОТКА И РЕНДЕРИНГ ОКНА 1: Животные
        // =================================================================================
        {
            glfwMakeContextCurrent(animalWindow);
            ImGui::SetCurrentContext(animalContext);

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            int win_w, win_h;
            glfwGetWindowSize(animalWindow, &win_w, &win_h);

            // --- Основное окно контента ---
            ImGui::SetNextWindowPos(ImVec2(0, 0));
            ImGui::SetNextWindowSize(ImVec2((float)win_w, (float)win_h - 30));
            if (ImGui::Begin("Animal Management", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus)) {
                if (ImGui::Button(" Загрузить Животных")) {
                    std::ifstream file(animalsFile);
                    if (file.is_open()) {
                        int loadedCount = 0;
                        int skippedCount = 0;
                        std::string line;
                        while (std::getline(file, line)) {
                            std::istringstream iss(line);
                            std::string nickname, species, cage;
                            if (iss >> nickname >> species >> cage) {
                                if (isNicknameUnique(nickname, animals)) {
                                    animals.push_back(Animal(nickname, species, cage));
                                    loadedCount++;
                                } else {
                                    skippedCount++;
                                }
                            }
                        }
                        file.close();
                        if (loadedCount > 0) {
                            rebuildAllStructures();
                        }
                        statusMessage = "Добавлено новых животных: " + std::to_string(loadedCount) +
                                        ". Пропущено дубликатов: " + std::to_string(skippedCount) + ".";
                    } else {
                        statusMessage = "Ошибка загрузки файла " + std::string(animalsFile);
                    }
                    statusMessageTime = ImGui::GetTime();
                }
                ImGui::SameLine();
                if (ImGui::Button(" Сохранить Животных")) {
                    if (animalTable.exportToFile(animalsFile, animals)) { statusMessage = "Животные сохранены в " + std::string(animalsFile); }
                    else { statusMessage = "Ошибка сохранения файла " + std::string(animalsFile); }
                    statusMessageTime = ImGui::GetTime();
                }
                ImGui::SameLine();
                if (ImGui::Button(" Очистить ХТ")) {
                    animals.clear();
                    feedings.clear();
                    rebuildAllStructures();
                    statusMessage = "Справочник животных, кормлений и все структуры данных очищены.";
                    statusMessageTime = ImGui::GetTime();
                }
                ImGui::SameLine(ImGui::GetWindowWidth() - 120);
                if (ImGui::Button("О Программе")) {
                    statusMessage = "Курсовая работа: Зоопарк; Структуры данных: Хеш-таблица, АВЛ-дерево, Двусвязный кольцевой список";
                    statusMessageTime = ImGui::GetTime();
                }
                ImGui::Separator();

                if (ImGui::BeginTabBar("AnimalTabs")) {
                    if (ImGui::BeginTabItem("Справочник Животных")) {
                        ImGui::BeginChild("AnimalOps", ImVec2(0,0), false);
                        SectionHeader("Добавление/Удаление Животного");
                        ImGui::InputTextWithHint("Кличка", "Например, 'Симба'", newNickname, IM_ARRAYSIZE(newNickname));
                        ImGui::InputTextWithHint("Вид", "Например, 'Лев'", newSpecies, IM_ARRAYSIZE(newSpecies));
                        ImGui::InputTextWithHint("Вольер", "Например, 'A-12'", newCage, IM_ARRAYSIZE(newCage));

                        if (ImGui::Button("Добавить Животное", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f - 4, 0))) {
                            if (strlen(newNickname) == 0) {
                                statusMessage = "Ошибка: Кличка животного не может быть пустой.";
                            } else if (strlen(newSpecies) == 0) {
                                statusMessage = "Ошибка: Вид животного не может быть пустым.";
                            } else if (strlen(newCage) == 0) {
                                statusMessage = "Ошибка: Вольер не может быть пустым.";
                            } else if (!isNicknameUnique(newNickname, animals)) {
                                statusMessage = "Ошибка: Животное с кличкой '" + std::string(newNickname) + "' уже существует!";
                            } else {
                                Animal a(newNickname, newSpecies, newCage);
                                animals.push_back(a);
                                animalTable.insert(a.nickname, animals.size() - 1);
                                speciesTree.add(a.species, animals.size() - 1);
                                statusMessage = "Животное '" + std::string(newNickname) + "' добавлено.";
                                newNickname[0] = '\0'; newSpecies[0] = '\0'; newCage[0] = '\0';
                            }
                            statusMessageTime = ImGui::GetTime();
                        }
                        ImGui::SameLine();
                        if (ImGui::Button("Удалить Животное", ImVec2(-1, 0))) {
                            if (strlen(newNickname) == 0) {
                                statusMessage = "Ошибка: Укажите кличку животного для удаления.";
                            } else if (strlen(newSpecies) == 0) {
                                statusMessage = "Ошибка: Укажите вид животного для удаления.";
                            } else if (strlen(newCage) == 0) {
                                statusMessage = "Ошибка: Укажите вольер для удаления.";
                            } else {
                                int steps;
                                int idx = animalTable.search(newNickname, steps);
                                if (idx >= 0 && animals[idx].species == newSpecies && animals[idx].cage == newCage) {
                                    std::string removedNickname = animals[idx].nickname;
                                    animals.erase(idx);
                                    DynamicArray<FeedingEntry> new_feedings;
                                    for(size_t i = 0; i < feedings.size(); ++i) {
                                        if (feedings[i].nickname != removedNickname) {
                                            new_feedings.push_back(feedings[i]);
                                        }
                                    }
                                    feedings = std::move(new_feedings);
                                    rebuildAllStructures();
                                    statusMessage = "Животное '" + removedNickname + "' и все его кормления удалены.";
                                } else {
                                    statusMessage = "Ошибка: Животное с такими данными для удаления не найдено.";
                                }
                            }
                            statusMessageTime = ImGui::GetTime();
                        }

                        SectionHeader("Поиск Животного");
                        static char searchNickname[128] = "";
                        ImGui::InputText("Кличка для поиска", searchNickname, IM_ARRAYSIZE(searchNickname));
                        if (ImGui::Button("Найти", ImVec2(-1, 0))) {
                            if (strlen(searchNickname) == 0) {
                                statusMessage = "Ошибка: Укажите кличку для поиска.";
                            } else {
                                int steps;
                                int idx = animalTable.search(searchNickname, steps);
                                if (idx >= 0) {
                                    statusMessage = "Найдено за " + std::to_string(steps) + " шагов: " +
                                                    animals[idx].nickname + " (Вид: " + animals[idx].species +
                                                    ", Вольер: " + animals[idx].cage + ")";
                                } else {
                                    statusMessage = "Животное с кличкой '" + std::string(searchNickname) + "' не найдено.";
                                }
                            }
                            statusMessageTime = ImGui::GetTime();
                        }

                        SectionHeader("Список Животных");
                        if (ImGui::BeginTable("AnimalsTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY)) {
                            ImGui::TableSetupColumn("Кличка", ImGuiTableColumnFlags_WidthStretch);
                            ImGui::TableSetupColumn("Вид", ImGuiTableColumnFlags_WidthStretch);
                            ImGui::TableSetupColumn("Вольер", ImGuiTableColumnFlags_WidthFixed, 100);
                            ImGui::TableHeadersRow();
                            for (size_t i = 0; i < animals.size(); ++i) {
                                ImGui::TableNextRow();
                                ImGui::TableNextColumn(); ImGui::Text("%s", animals[i].nickname.c_str());
                                ImGui::TableNextColumn(); ImGui::Text("%s", animals[i].species.c_str());
                                ImGui::TableNextColumn(); ImGui::Text("%s", animals[i].cage.c_str());
                            }
                            ImGui::EndTable();
                        }
                        ImGui::EndChild();
                        ImGui::EndTabItem();
                    }

                    if (ImGui::BeginTabItem("Отладка ХТ")) {
                        ImGui::BeginChild("HTDebug", ImVec2(0,0), false);
                        SectionHeader("Статистика Хеш-Таблицы");
                        ImGui::Text("Емкость: %d", animalTable.getCapacity());
                        ImGui::Text("Элементов: %d", animalTable.getSize());
                        ImGui::Text("Коэффициент загрузки: %.2f", animalTable.getLoadFactor());
                        ImGui::Text("Начальный размер: %d", initialTableSize);
                        ImGui::InputInt("##NewInitialSize", &initialTableSize);
                        ImGui::SameLine();
                        if(ImGui::Button("Применить")){
                            animalTable.resize(initialTableSize);
                            rebuildAllStructures();
                        }

                        SectionHeader("Содержимое Хеш-Таблицы");
                        if (ImGui::BeginTable("HTViz", 6, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY)) {
                            ImGui::TableSetupColumn("Слот", ImGuiTableColumnFlags_WidthFixed, 50);
                            ImGui::TableSetupColumn("H1 (первичный)", ImGuiTableColumnFlags_WidthFixed, 100);
                            ImGui::TableSetupColumn("H2 (шаг)", ImGuiTableColumnFlags_WidthFixed, 80);
                            ImGui::TableSetupColumn("Ключ (Кличка)", ImGuiTableColumnFlags_WidthStretch);
                            ImGui::TableSetupColumn("Индекс", ImGuiTableColumnFlags_WidthFixed, 60);
                            ImGui::TableSetupColumn("Статус", ImGuiTableColumnFlags_WidthFixed, 60);
                            ImGui::TableHeadersRow();
                            for(int i = 0; i < animalTable.getCapacity(); ++i) {
                                HashEntry info = animalTable.getSlotInfo(i);
                                ImGui::TableNextRow();
                                ImGui::TableNextColumn();
                                ImGui::Text("%d", i);

                                ImGui::TableNextColumn();
                                if(info.status == 1) {
                                    ImGui::Text("%d", animalTable.getPrimaryHash(info.key));
                                } else {
                                    ImGui::Text("-");
                                }

                                ImGui::TableNextColumn();
                                ImGui::Text("%d", animalTable.getSecondaryHash());

                                ImGui::TableNextColumn();
                                if(info.status != 0) {
                                    ImGui::Text("%s", info.key.c_str());
                                } else {
                                    ImGui::Text("-");
                                }

                                ImGui::TableNextColumn();
                                if(info.index != -1) {
                                    ImGui::Text("%d", info.index);
                                } else {
                                    ImGui::Text("-");
                                }

                                ImGui::TableNextColumn();
                                if(info.status == 0) ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "0");
                                else if(info.status == 1) ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "1");
                                else if(info.status == 2) ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "2");
                            }
                            ImGui::EndTable();
                        }
                        ImGui::EndChild();
                        ImGui::EndTabItem();
                    }
                    ImGui::EndTabBar();
                }
            }
            ImGui::End();

            // --- Статус-бар для окна 1 ---
            ImGui::SetNextWindowPos(ImVec2(0, (float)win_h - 30));
            ImGui::SetNextWindowSize(ImVec2((float)win_w, 30));
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            if (ImGui::Begin("AnimalStatusBar", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
                if (ImGui::GetTime() - statusMessageTime < 10.0f) {
                    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "%s", statusMessage.c_str());
                } else {
                    ImGui::Text("Готов | Животных: %zu", animals.size());
                }
            }
            ImGui::End();
            ImGui::PopStyleVar();
            ImGui::Render();
            int display_w, display_h;
            glfwGetFramebufferSize(animalWindow, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
            glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            glfwSwapBuffers(animalWindow);
        }

        // =================================================================================
        // ОБРАБОТКА И РЕНДЕРИНГ ОКНА 2: Кормления
        // =================================================================================
        {
            glfwMakeContextCurrent(feedingWindow);
            ImGui::SetCurrentContext(feedingContext);

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            int win_w, win_h;
            glfwGetWindowSize(feedingWindow, &win_w, &win_h);

            // --- Основное окно контента ---
            ImGui::SetNextWindowPos(ImVec2(0, 0));
            ImGui::SetNextWindowSize(ImVec2((float)win_w, (float)win_h - 30));
            if (ImGui::Begin("Feeding Management", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus)) {
                if (ImGui::Button(" Загрузить Кормления")) {
                    int skipped_count = 0;
                    int loaded_count = 0;

                    std::ifstream file(feedingsFile);
                    if (file.is_open()) {
                        std::string line;
                        while (std::getline(file, line)) {
                            std::istringstream iss(line);
                            FeedingEntry e;
                            if (iss >> e.nickname >> e.feedType >> e.quantity >> e.date) {
                                int steps;
                                if (animalTable.search(e.nickname, steps) >= 0) {
                                    feedings.push_back(e);
                                    loaded_count++;
                                } else {
                                    skipped_count++;
                                }
                            }
                        }
                        file.close();
                        rebuildAllStructures();
                        statusMessage = "Добавлено кормлений: " + std::to_string(loaded_count) +
                                        " (пропущено " + std::to_string(skipped_count) + " из-за отсутствия животных).";
                    } else {
                        statusMessage = "Ошибка загрузки файла " + std::string(feedingsFile);
                    }
                    statusMessageTime = ImGui::GetTime();
                }
                ImGui::SameLine();
                if (ImGui::Button(" Сохранить Кормления")) {
                    if (feedingTree.exportToFile(feedingsFile, feedings)) { statusMessage = "Кормления сохранены в " + std::string(feedingsFile); }
                    else { statusMessage = "Ошибка сохранения файла " + std::string(feedingsFile); }
                    statusMessageTime = ImGui::GetTime();
                }
                ImGui::SameLine();
                if (ImGui::Button(" Очистить Дерево")) {
                    feedings.clear();
                    rebuildAllStructures();
                    statusMessage = "Справочник кормлений и дерево очищены.";
                    statusMessageTime = ImGui::GetTime();
                }
                ImGui::SameLine();
                if (ImGui::Button(" Сохранить Отчет")) {
                    if (reportGenerated && !reportResults.empty()) {
                        std::ofstream reportFile("report.txt");
                        if (reportFile.is_open()) {

                            // --- Вспомогательные функции форматирования (из вашего примера) ---

                            auto countChars = [](const std::string& str) -> int {
                                int count = 0;
                                for (size_t i = 0; i < str.length(); i++) {
                                    if ((str[i] & 0xC0) != 0x80) {
                                        count++;
                                    }
                                }
                                return count;
                            };

                            auto padRight = [&countChars](const std::string& str, int width) -> std::string {
                                int charCount = countChars(str);
                                if (charCount >= width) {
                                    return str;
                                }
                                return str + std::string(width - charCount, ' ');
                            };

                            auto padLeft = [&countChars](const std::string& str, int width) -> std::string {
                                int charCount = countChars(str);
                                if (charCount >= width) {
                                    return str;
                                }
                                return std::string(width - charCount, ' ') + str;
                            };

                            // --- Основная логика отчета ---

                            const int W_NICKNAME = 25;
                            const int W_SPECIES = 20;
                            const int W_COUNT = 22;

                            // Заголовок файла
                            reportFile << "=== ОТЧЕТ О КОРМЛЕНИИ ЖИВОТНЫХ ===\n";
                            reportFile << "Дата: " << reportDate << "\n";
                            if (strlen(reportSpeciesFilter) > 0)
                                reportFile << "Фильтр по виду: " << reportSpeciesFilter << "\n";
                            reportFile << "\n";

                            // Заголовки таблицы
                            reportFile << padRight("Кличка", W_NICKNAME) << " | ";
                            reportFile << padRight("Вид", W_SPECIES) << " | ";
                            reportFile << padLeft("Количество кормлений", W_COUNT) << "\n";

                            // Линия-разделитель
                            reportFile << std::string(W_NICKNAME, '-') << "-+-";
                            reportFile << std::string(W_SPECIES, '-') << "-+-";
                            reportFile << std::string(W_COUNT, '-') << "\n";

                            // Вывод данных и подсчет итогов
                            int totalFeedings = 0;
                            for (size_t i = 0; i < reportResults.size(); ++i) {
                                const auto& result = reportResults[i];
                                totalFeedings += result.feedingCount;

                                reportFile << padRight(result.nickname, W_NICKNAME) << " | ";
                                reportFile << padRight(result.species, W_SPECIES) << " | ";
                                reportFile << padLeft(std::to_string(result.feedingCount), W_COUNT) << "\n";
                            }

                            // Линия для итогов
                            reportFile << std::string(W_NICKNAME, '=') << "=+=";
                            reportFile << std::string(W_SPECIES, '=') << "=+=";
                            reportFile << std::string(W_COUNT, '=') << "\n";

                            // Строка "ИТОГО"
                            int prefixWidth = W_NICKNAME + W_SPECIES + 3;
                            reportFile << padRight("ИТОГО:", prefixWidth) << " | ";
                            reportFile << padLeft(std::to_string(totalFeedings), W_COUNT) << "\n";

                            // Дополнительная информация в конце отчета
                            reportFile << "\nВсего животных в отчете: " << reportResults.size() << "\n";
                            reportFile << "Общее количество кормлений: " << totalFeedings << "\n";

                            reportFile.close();
                            statusMessage = "Отчет сохранен в report.txt";
                        } else {
                            statusMessage = "Ошибка: Не удалось создать файл отчета.";
                        }
                    } else {
                        statusMessage = "Нет данных для сохранения отчета.";
                    }
                    statusMessageTime = ImGui::GetTime();
                }
                ImGui::Separator();

                if (ImGui::BeginTabBar("FeedingTabs")) {
                    if (ImGui::BeginTabItem("Справочник Кормлений")) {
                        ImGui::BeginChild("FeedingOps", ImVec2(0,0), false);

                        SectionHeader("Добавление/Удаление Кормления");
                        ImGui::InputTextWithHint("Кличка", "Кличка существующего животного", feedingNickname, IM_ARRAYSIZE(feedingNickname));
                        ImGui::InputTextWithHint("Тип корма", "Например, 'Мясо'", feedingFeedType, IM_ARRAYSIZE(feedingFeedType));
                        ImGui::InputInt("Количество кормлений", &feedingQuantity);
                        ImGui::InputTextWithHint("Дата", "DD.MM.YYYY", feedingDate, IM_ARRAYSIZE(feedingDate));

                        if (ImGui::Button("Добавить Кормление", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f - 4, 0))) {
                            if (strlen(feedingNickname) == 0) {
                                statusMessage = "Ошибка: Кличка животного не может быть пустой.";
                            } else if (strlen(feedingFeedType) == 0) {
                                statusMessage = "Ошибка: Тип корма не может быть пустым.";
                            } else if (feedingQuantity <= 0) {
                                statusMessage = "Ошибка: Количество кормлений не может быть отрицательным или нулевым.";
                            } else if (feedingQuantity > 1000) {
                                statusMessage = "Ошибка: Количество кормлений не может превышать 1000.";
                            } else if (strlen(feedingDate) == 0) {
                                statusMessage = "Ошибка: Дата кормления не может быть пустой.";
                            } else if (!isValidDate(feedingDate)) {
                                statusMessage = "Ошибка: Некорректный формат даты! Требуется DD.MM.YYYY";
                            } else {
                                int steps;
                                if (animalTable.search(feedingNickname, steps) < 0) {
                                    statusMessage = "Ошибка: Животное с кличкой '" + std::string(feedingNickname) + "' не найдено в справочнике.";
                                } else {
                                    FeedingEntry f{feedingNickname, feedingFeedType, feedingQuantity, feedingDate};
                                    feedings.push_back(f);
                                    feedingTree.add(f.nickname, feedings.size()-1);
                                    quantityTree.add(f.quantity, feedings.size()-1);
                                    dateTree.add(f.date, feedings.size()-1);
                                    statusMessage = "Кормление для '" + std::string(feedingNickname) + "' добавлено.";
                                }
                            }
                            statusMessageTime = ImGui::GetTime();
                        }
                        ImGui::SameLine();
                        if (ImGui::Button("Удалить Кормление", ImVec2(-1, 0))) {
                            if (strlen(feedingNickname) == 0) {
                                statusMessage = "Ошибка: Укажите кличку для удаления кормления.";
                            } else if (strlen(feedingFeedType) == 0) {
                                statusMessage = "Ошибка: Укажите тип корма для удаления кормления.";
                            } else if (feedingQuantity <= 0) {
                                statusMessage = "Ошибка: Количество не может быть отрицательным или нулевым.";
                            } else if (strlen(feedingDate) == 0) {
                                statusMessage = "Ошибка: Укажите дату для удаления кормления.";
                            } else if (!isValidDate(feedingDate)) {
                                statusMessage = "Ошибка: Некорректный формат даты! Требуется DD.MM.YYYY";
                            } else {
                                bool found = false;
                                int indexToRemove = -1;
                                for (int i = 0; i < (int)feedings.size(); ++i) {
                                    if (feedings[i].nickname == feedingNickname &&
                                        feedings[i].feedType == feedingFeedType &&
                                        feedings[i].quantity == feedingQuantity &&
                                        feedings[i].date == feedingDate) {
                                        indexToRemove = i;
                                        found = true;
                                        break;
                                        }
                                }
                                if (found) {
                                    feedings.erase(indexToRemove);
                                    rebuildAllStructures();
                                    statusMessage = "Кормление удалено.";
                                } else {
                                    statusMessage = "Ошибка: Кормление с такими данными не найдено.";
                                }
                            }
                            statusMessageTime = ImGui::GetTime();
                        }

                        SectionHeader("Поиск Кормлений по Кличке");
                        static char searchFeedingNickname[128] = "";
                        ImGui::InputText("Кличка для поиска", searchFeedingNickname, IM_ARRAYSIZE(searchFeedingNickname));
                        if (ImGui::Button("Найти Кормления", ImVec2(-1, 0))) {
                            if (strlen(searchFeedingNickname) == 0) {
                                statusMessage = "Ошибка: Укажите кличку для поиска кормлений.";
                            } else {
                                CircularList result = feedingTree.search(searchFeedingNickname);
                                if (result.size() > 0) {
                                    std::stringstream ss;
                                    ss << "Найдено кормлений: " << result.size() << " для клички '" << searchFeedingNickname << "'. ";
                                    statusMessage = ss.str();
                                } else {
                                    statusMessage = "Кормлений для клички '" + std::string(searchFeedingNickname) + "' не найдено.";
                                }
                            }
                            statusMessageTime = ImGui::GetTime();
                        }


                        SectionHeader("Список Кормлений");
                         if (ImGui::BeginTable("FeedingsTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY)) {
                            ImGui::TableSetupColumn("Кличка", ImGuiTableColumnFlags_WidthStretch);
                            ImGui::TableSetupColumn("Тип корма", ImGuiTableColumnFlags_WidthStretch);
                             ImGui::TableSetupColumn("Кормлений", ImGuiTableColumnFlags_WidthFixed, 80);
                            ImGui::TableSetupColumn("Дата", ImGuiTableColumnFlags_WidthFixed, 120);
                            ImGui::TableHeadersRow();
                            for (size_t i = 0; i < feedings.size(); ++i) {
                                ImGui::TableNextRow();
                                ImGui::TableNextColumn(); ImGui::Text("%s", feedings[i].nickname.c_str());
                                ImGui::TableNextColumn(); ImGui::Text("%s", feedings[i].feedType.c_str());
                                ImGui::TableNextColumn(); ImGui::Text("%d", feedings[i].quantity);
                                ImGui::TableNextColumn(); ImGui::Text("%s", feedings[i].date.c_str());
                            }
                            ImGui::EndTable();
                        }
                        ImGui::EndChild();
                        ImGui::EndTabItem();
                    }

                    if (ImGui::BeginTabItem("Отчеты")) {
                        ImGui::BeginChild("Reports", ImVec2(0,0), false);
                        SectionHeader("Фильтры отчета");
                        ImGui::InputTextWithHint("Дата", "DD.MM.YYYY", reportDate, IM_ARRAYSIZE(reportDate));
                        ImGui::InputTextWithHint("Фильтр по виду (необязательно)", "Например, 'Тигр'", reportSpeciesFilter, IM_ARRAYSIZE(reportSpeciesFilter));
                        ImGui::InputInt("Фильтр: Кол-во кормлений (0 = любое)", &reportQuantity);

                        if (ImGui::Button("Сформировать отчет", ImVec2(-1, 0))) {
                            reportResults.clear();
                            if (strlen(reportDate) == 0) {
                                statusMessage = "Ошибка: Дата не может быть пустой для формирования отчета.";
                            } else if (!isValidDate(reportDate)) {
                                statusMessage = "Ошибка: Некорректный формат даты! Требуется DD.MM.YYYY";
                            } else if (reportQuantity < 0) {
                                statusMessage = "Ошибка: Количество не может быть отрицательным.";
                            } else {

                                // Шаг 1: Получаем базовый список по обязательной дате
                                CircularList finalIndices = dateTree.search(reportDate);

                                // Шаг 2: Фильтруем по виду, если он указан
                                if (strlen(reportSpeciesFilter) > 0) {
                                    CircularList tempList;
                                    for (int i = 0; i < finalIndices.size(); ++i) {
                                        int feedingIndex = finalIndices.get(i);
                                        const auto& feeding = feedings[feedingIndex];
                                        int steps;
                                        int animalIdx = animalTable.search(feeding.nickname, steps);
                                        if (animalIdx != -1 && animals[animalIdx].species == reportSpeciesFilter) {
                                            tempList.add(feedingIndex);
                                        }
                                    }
                                    finalIndices = tempList;
                                }

                                // Шаг 3: Фильтруем по количеству, если оно указано
                                if (reportQuantity > 0) {
                                    CircularList tempList;
                                    for (int i = 0; i < finalIndices.size(); ++i) {
                                        int feedingIndex = finalIndices.get(i);
                                        if (feedings[feedingIndex].quantity == reportQuantity) {
                                            tempList.add(feedingIndex);
                                        }
                                    }
                                    finalIndices = tempList;
                                }

                                // --- Формирование отчета: каждое кормление - отдельная строка ---

                                int totalFeedingsSum = 0;

                                for(int i = 0; i < finalIndices.size(); ++i) {
                                    int index = finalIndices.get(i);
                                    if(index == -1) continue;
                                    const auto& feeding = feedings[index];
                                    int steps;
                                    int animalIdx = animalTable.search(feeding.nickname, steps);

                                    if (animalIdx != -1) {
                                        const auto& animal = animals[animalIdx];

                                        // Добавляем запись в отчет
                                        reportResults.push_back({
                                            feeding.nickname,
                                            animal.species,
                                            feeding.quantity
                                        });

                                        // Суммируем количество кормлений
                                        totalFeedingsSum += feeding.quantity;
                                    }
                                }

                                reportGenerated = true;
                                statusMessage = "Отчет сформирован: найдено животных: " +
                                                std::to_string(reportResults.size()) +
                                                ", всего кормлений: " +
                                                std::to_string(totalFeedingsSum);
                            }
                            statusMessageTime = ImGui::GetTime();
                        }

                        SectionHeader("Результаты отчета");
                        if (ImGui::BeginTable("ReportTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY)) {
                            ImGui::TableSetupColumn("Кличка", ImGuiTableColumnFlags_WidthStretch);
                            ImGui::TableSetupColumn("Вид", ImGuiTableColumnFlags_WidthStretch);
                            ImGui::TableSetupColumn("Количество кормлений", ImGuiTableColumnFlags_WidthFixed, 200);
                            ImGui::TableHeadersRow();
                            if(reportGenerated) {
                                for(size_t i = 0; i < reportResults.size(); ++i) {
                                    ImGui::TableNextRow();
                                    ImGui::TableNextColumn(); ImGui::Text("%s", reportResults[i].nickname.c_str());
                                    ImGui::TableNextColumn(); ImGui::Text("%s", reportResults[i].species.c_str());
                                    ImGui::TableNextColumn(); ImGui::Text("%d", reportResults[i].feedingCount);
                                }
                            }
                            ImGui::EndTable();
                        }

                        ImGui::EndChild();
                        ImGui::EndTabItem();
                    }

                    if (ImGui::BeginTabItem("Отладка Дерева")) {
                        ImGui::BeginChild("TreeDebug", ImVec2(0,0), false);
                        if (ImGui::Button("Показать Главное Дерево (по кличкам)", ImVec2(-1, 0))) {
                            debugLog << "\n--- Дерево кормлений (по кличке) ---\n";
                            feedingTree.print(debugLog);
                            debugLog << "-------------------------------------\n\n";
                        }
                        if (ImGui::Button("Показать Дерево Фильтра по Дате", ImVec2(-1, 0))) {
                            debugLog << "\n--- Дерево фильтра по дате ---\n";
                            dateTree.print(debugLog);
                            debugLog << "--------------------------------\n\n";
                        }
                        if (ImGui::Button("Показать Дерево Фильтра по Виду", ImVec2(-1, 0))) {
                            debugLog << "\n--- Дерево фильтра по виду ---\n";
                            speciesTree.print(debugLog);
                            debugLog << "--------------------------------\n\n";
                        }
                        if (ImGui::Button("Показать Дерево Фильтра по Количеству", ImVec2(-1, 0))) {
                            debugLog << "\n--- Дерево фильтра по количеству ---\n";
                            quantityTree.print(debugLog);
                            debugLog << "-------------------------------------\n\n";
                        }
                        SectionHeader("Журнал отладки");
                         if (ImGui::Button("Очистить лог")) debugLog.str("");
                         ImGui::SameLine();
                         if (ImGui::Button("Сохранить лог")) {
                             std::ofstream logFile("debug_log.txt");
                             logFile << debugLog.str();
                             logFile.close();
                         }
                        ImGui::BeginChild("DebugLog", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
                        ImGui::TextUnformatted(debugLog.str().c_str());
                        ImGui::EndChild();

                        ImGui::EndChild();
                        ImGui::EndTabItem();
                    }
                    ImGui::EndTabBar();
                }
            }
            ImGui::End();

            // --- Статус-бар для окна 2 ---
            ImGui::SetNextWindowPos(ImVec2(0, (float)win_h - 30));
            ImGui::SetNextWindowSize(ImVec2((float)win_w, 30));
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            if (ImGui::Begin("FeedingStatusBar", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
                if (ImGui::GetTime() - statusMessageTime < 10.0f) {
                    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "%s", statusMessage.c_str());
                } else {
                    ImGui::Text("Готов | Кормлений: %zu", feedings.size());
                }
            }
            ImGui::End();
            ImGui::PopStyleVar();

            ImGui::Render();
            int display_w, display_h;
            glfwGetFramebufferSize(feedingWindow, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
            glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            glfwSwapBuffers(feedingWindow);
        }
    }

    // ------------------------------
    // Очистка ресурсов
    // ------------------------------
    ImGui::SetCurrentContext(animalContext);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext(animalContext);

    ImGui::SetCurrentContext(feedingContext);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext(feedingContext);

    glfwDestroyWindow(animalWindow);
    glfwDestroyWindow(feedingWindow);
    glfwTerminate();

    return 0;
}