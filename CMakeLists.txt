cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project (VulkanCourseApp)

set(ASSETS_DIR "${CMAKE_SOURCE_DIR}/assets")
 
set(THIRDPARTY_DIR "${CMAKE_SOURCE_DIR}/../../thirdparty")

set(PROJECT_SOURCE_FILES
  source/main.cpp
  source/Application.h
  source/Application.cpp
  source/Renderer.h
  source/Renderer.cpp
  vulkanAPI/VulkanDevice.h
  vulkanAPI/VulkanDevice.cpp
  vulkanAPI/VulkanValidation.h
  vulkanAPI/VulkanSwapchain.h
  vulkanAPI/VulkanSwapchain.cpp
  vulkanAPI/VulkanPipeline.h
  vulkanAPI/VulkanPipeline.cpp
  vulkanAPI/VulkanDescriptors.h
  vulkanAPI/VulkanDescriptors.cpp
  vulkanAPI/VulkanRenderpass.h
  vulkanAPI/VulkanRenderpass.cpp
  vulkanAPI/VulkanInstance.h
  vulkanAPI/VulkanInstance.cpp
  vulkanAPI/VulkanBuffer.h
  vulkanAPI/VulkanBuffer.cpp
  vulkanAPI/VulkanSync.h
  vulkanAPI/VulkanSync.cpp
  vulkanAPI/VulkanCommand.h
  vulkanAPI/VulkanCommand.cpp


  components/Mesh.h
  components/Mesh.cpp
  components/MeshModel.h
  components/MeshModel.cpp
  components/Texture.h
  components/Texture.cpp
  components/window.h
  components/window.cpp
  components/FileSystem.h
  components/FileSystem.cpp
  components/Config.h


)
# Add the application
include_directories(source)
add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_FILES})

# GLFW
add_subdirectory(
    ${THIRDPARTY_DIR}/glfw-3.4
    ${CMAKE_CURRENT_BINARY_DIR}/glfw_build
)
include_directories(${THIRDPARTY_DIR}/glfw-3.4/include)

# GLEW
add_compile_definitions(GLEW_NO_GLU)
set(BUILD_UTILS OFF CACHE BOOL "utilities" FORCE)

add_subdirectory(
    ${THIRDPARTY_DIR}/glew/build/cmake
    ${CMAKE_CURRENT_BINARY_DIR}/glew_build
)
include_directories(${THIRDPARTY_DIR}/glew/include)

set_property(TARGET glew_s PROPERTY FOLDER GLEW)
set_property(TARGET glew PROPERTY FOLDER GLEW)

# GLM
add_subdirectory(
    ${THIRDPARTY_DIR}/glm-1.0.1/glm
    ${CMAKE_CURRENT_BINARY_DIR}/glm_build
)
include_directories(${THIRDPARTY_DIR}/glm-1.0.1)

# Vulkan
include_directories(${THIRDPARTY_DIR}/vulkan/include)

# Add Assimp library
include_directories(${THIRDPARTY_DIR}/assimp/include)

# JSON
include_directories(${THIRDPARTY_DIR}/json)

# stb_image
include_directories(${THIRDPARTY_DIR}/stb_image)


# Link all thirdparty libraries
target_link_libraries(${PROJECT_NAME} 
    glfw 
    glew_s
	glm

    ${THIRDPARTY_DIR}/assimp/lib/assimp-vc140-mt.lib
    ${THIRDPARTY_DIR}/vulkan/lib/vulkan-1.lib

)


add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	${ASSETS_DIR}
	${CMAKE_BINARY_DIR}/assets
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/assimp-vc140-mt.dll"
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
)