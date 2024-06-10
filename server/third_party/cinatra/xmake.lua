
add_requires("openssl")
-- Target
target("cinatra")
    set_kind("static")
    set_languages("c++20")
    add_cxxflags("-g -pthread -std=c++20", {tools={"gcc", "clang"}})
    add_cxxflags("-fcoroutines", {tools={"gcc"}})
    if is_mode("release") then 
        add_cxxflags("-fno-tree-slp-vectorize", {tools={"gcc"}})
    end
    add_cxxflags("/bigobj /EHa", {tools={"cl"}})
    add_includedirs("include", {public=true})
    add_defines("CINATRA_ENABLE_SSL")
    add_packages("openssl")
    -- add_files("include/**.h", "include/**.hpp")