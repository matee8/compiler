set_defaultmode("debug")
set_languages("c++20")
add_rules("mode.debug", "mode.release")
add_cxxflags("-pedantic-errors", "-Wall", "-Weffc++", "-Wextra", "-Wconversion", "-Wsign-conversion", "-Werror")

if is_mode("debug") then
    add_cxxflags("-ggdb")
end

if is_mode("release") then
    add_cxxflags("-DNDEBUG")
end

local libs = { }

local dev_libs = { }

add_includedirs("include")
add_requires(table.unpack(libs))

target("compiler-lib")
    set_kind("static")
    add_files("src/*.cpp")
    add_packages(table.unpack(libs))

target("compiler")
    set_kind("binary")
    add_files("bin/main.cpp")
    add_packages(table.unpack(libs))
    add_deps("compiler-lib")
