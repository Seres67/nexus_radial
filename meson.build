project('nexus_template', 'cpp',
        version : '1.0.0',
        default_options : ['warning_level=3', 'cpp_std=c++20'])

subdir('src')
include_dir = include_directories('include')

## Import Imgui
imgui_include_dirs = include_directories('include')
imgui_sources = files('src/imgui/imgui.cpp', 'src/imgui/imgui_draw.cpp', 'src/imgui/imgui_tables.cpp', 'src/imgui/imgui_widgets.cpp', 'src/imgui/imgui_demo.cpp')
imgui_lib = static_library('imgui', imgui_sources, include_directories: imgui_include_dirs)
imgui_dep = declare_dependency(link_with: imgui_lib, include_directories: imgui_include_dirs)

nexus_template = library('nexus_template', sources, include_directories : include_dir, dependencies: [imgui_dep])