cmake_minimum_required(VERSION 3.16)
project(Insectron CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 2.5 COMPONENTS graphics window system audio REQUIRED)

add_executable(Insectron
    Main.cpp
    Game.cpp
    Match.cpp
    SaveGame.cpp
    FormationManager.cpp
    Managers/AssetManager.cpp
    Managers/AudioManager.cpp
    Managers/ScoreManager.cpp
    Scenes/Scene.cpp
    Scenes/SceneMenu.cpp
    Scenes/SceneOptions.cpp
    Scenes/SceneInitials.cpp
    Scenes/SceneHighScores.cpp
    Scenes/SceneStageIntro.cpp
    Scenes/SceneMatch.cpp
    Entities/Entity.cpp
    Entities/Player.cpp
    Entities/Enemy.cpp
    Entities/Bullet.cpp
    Entities/PowerUp.cpp
)

target_include_directories(Insectron PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/Scenes
    ${CMAKE_SOURCE_DIR}/Entities
    ${CMAKE_SOURCE_DIR}/Managers
)

target_link_libraries(Insectron PRIVATE sfml-graphics sfml-window sfml-system sfml-audio)

configure_file(${CMAKE_SOURCE_DIR}/Nave.png ${CMAKE_BINARY_DIR}/Nave.png COPYONLY)

if(EXISTS ${CMAKE_SOURCE_DIR}/Resources/Tr2n.ttf)
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/Resources)
    configure_file(${CMAKE_SOURCE_DIR}/Resources/Tr2n.ttf ${CMAKE_BINARY_DIR}/Resources/Tr2n.ttf COPYONLY)
endif()
