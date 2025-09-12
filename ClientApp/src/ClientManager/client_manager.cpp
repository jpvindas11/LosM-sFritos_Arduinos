/// Copyright Los Más Fritos - 2025

#include "client_manager.hpp"

ClientManager::ClientManager() 
    : current_scene_index(0), renderer("Frituinos", 800, 600) {
    // Example of adding a scene
    Scene mainMenu = Scene(SCENE_STARTUP, COLOR_BLUE);

    // Button
    mainMenu.add_button(Button(100.0f, -200.0f, 100.0f, -100.0f, 150, 50));
    scenes.push_back(mainMenu);

    // Set renderer pointers
    renderer.setSceneIndex_ptr(&current_scene_index);
    renderer.setScenes_ptr(&scenes);

}

ClientManager::~ClientManager() {
    scenes.clear();
}

void ClientManager::run() {
    while(1) {
        // Get the current scene
        Scene* current_scene = &scenes[current_scene_index];

        current_scene->tick_elements();
        current_scene->tick_buttons(false, 0.0f, 0.0f);

        renderer.draw();
    }
}