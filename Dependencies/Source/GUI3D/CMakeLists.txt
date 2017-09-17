cmake_minimum_required(VERSION 2.6)

project (GUI3D)

set (GUI3DFOLDER ${GAMEKIT_ADDITIONAL_DEP_FOLDER}/gui3d/build/source)

MESSAGE(STATUS "___GORILLAFOLDER ${GUI3DFOLDER}")

set (GORILLA  ${GUI3DFOLDER}/Gorilla.h ${GUI3DFOLDER}/Gorilla.cpp)
MESSAGE(STATUS "GSOURCE ${GUI3DFOLDER} ${GORILLA}")


    set (GUI3D_FILES
        ${GUI3DFOLDER}/Gui3D.cpp
        ${GUI3DFOLDER}/Gui3D.h
        ${GUI3DFOLDER}/Gui3DButton.cpp
        ${GUI3DFOLDER}/Gui3DButton.h
        ${GUI3DFOLDER}/Gui3DCaption.cpp
        ${GUI3DFOLDER}/Gui3DCaption.h
        ${GUI3DFOLDER}/Gui3DCheckbox.cpp
        ${GUI3DFOLDER}/Gui3DCheckbox.h
        ${GUI3DFOLDER}/Gui3DCheckboxSprite.cpp
        ${GUI3DFOLDER}/Gui3DCheckboxSprite.h
        ${GUI3DFOLDER}/Gui3DCheckboxText.cpp
        ${GUI3DFOLDER}/Gui3DCheckboxText.h
        ${GUI3DFOLDER}/Gui3DClock.cpp
        ${GUI3DFOLDER}/Gui3DClock.h
        ${GUI3DFOLDER}/Gui3DCombobox.cpp
        ${GUI3DFOLDER}/Gui3DCombobox.h
        ${GUI3DFOLDER}/Gui3DConfig.h
        ${GUI3DFOLDER}/Gui3DContainer.cpp
        ${GUI3DFOLDER}/Gui3DContainer.h
        ${GUI3DFOLDER}/Gui3DDebugPanelColors.cpp
        ${GUI3DFOLDER}/Gui3DDebugPanelColors.h
        ${GUI3DFOLDER}/Gui3DFunctorBase.h
        ${GUI3DFOLDER}/Gui3DInlineSelector.cpp
        ${GUI3DFOLDER}/Gui3DInlineSelector.h
        ${GUI3DFOLDER}/Gui3DListbox.cpp
        ${GUI3DFOLDER}/Gui3DListbox.h
        ${GUI3DFOLDER}/Gui3DMemberFunction.h
        ${GUI3DFOLDER}/Gui3DMultipleElementsSelector.cpp
        ${GUI3DFOLDER}/Gui3DMultipleElementsSelector.h
        ${GUI3DFOLDER}/Gui3DPanel.cpp
        ${GUI3DFOLDER}/Gui3DPanel.h
        ${GUI3DFOLDER}/Gui3DPanelColors.h
        ${GUI3DFOLDER}/Gui3DPanelContainer.cpp
        ${GUI3DFOLDER}/Gui3DPanelContainer.h
        ${GUI3DFOLDER}/Gui3DPanelElement.cpp
        ${GUI3DFOLDER}/Gui3DPanelElement.h
        ${GUI3DFOLDER}/Gui3DPanelElementCallback.h
        ${GUI3DFOLDER}/Gui3DProgressBar.cpp
        ${GUI3DFOLDER}/Gui3DProgressBar.h
        ${GUI3DFOLDER}/Gui3DScreenPanel.cpp
        ${GUI3DFOLDER}/Gui3DScreenPanel.h
        ${GUI3DFOLDER}/Gui3DScrollBar.cpp
        ${GUI3DFOLDER}/Gui3DScrollBar.h
        ${GUI3DFOLDER}/Gui3DStringCursorEdit.cpp
        ${GUI3DFOLDER}/Gui3DStringCursorEdit.h
        ${GUI3DFOLDER}/Gui3DTextZone.cpp
        ${GUI3DFOLDER}/Gui3DTextZone.h
        ${GUI3DFOLDER}/Gui3DUtils.h
        ${GUI3DFOLDER}/Gui3DVerticalSelector.cpp
        ${GUI3DFOLDER}/Gui3DVerticalSelector.h
        ${GUI3DFOLDER}/Gui3DRect.h
        ${GUI3DFOLDER}/Gui3DRect.cpp
    )

set (GORILLA_CONSOLE         ${GUI3DFOLDER}/OgreConsoleForGorilla.h  ${GUI3DFOLDER}/OgreConsoleForGorilla.cpp)

message(STATUS "OGRE-BIN: ${OGREKIT_BULLET_INCLUDE}")

include_directories(${GAMEKIT_ADDITIONAL_DEP_FOLDER}/ogre-git/OgreMain/include
					${OGRE_BINARY_DIR}/../include
					../OIS/include
					../GameKit/Utils
					../Lua/lua
					${OGREKIT_BULLET_INCLUDE}
					${GAMEKIT_ADDITIONAL_DEP_FOLDER}/GUI3D/build
					./Gorilla
)

add_library(${PROJECT_NAME} ${GORILLA} ${GUI3D_FILES} ${GORILLA_CONSOLE})
