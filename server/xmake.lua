add_rules("plugin.compile_commands.autoupdate", { outputdir = ".vscode" })

-- Global
set_project("2fa-server")
set_version("0.0.1", {build = "%Y%m%d%H%M"})

-- Includes
-- includes("third_party/cinatra")

-- Targets
add_requires("openssl")

target("2fa_server")
    set_kind("binary")
    set_languages("c++20")

    -- cinatra
    add_cxxflags("-g -pthread -std=c++20", {tools={"gcc", "clang"}})
    add_cxxflags("-fcoroutines", {tools={"gcc"}})
    if is_mode("release") then 
        add_cxxflags("-fno-tree-slp-vectorize", {tools={"gcc"}})
    end
    add_cxxflags("/bigobj /EHa", {tools={"cl"}})
    add_defines("CINATRA_ENABLE_SSL")
    add_packages("openssl")
    -- end cinatra

    add_includedirs("src", "third_party", "third_party/cinatra/include")
    add_files("src/app.cc", "src/utils/**.cc")