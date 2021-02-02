import qbs

QtApplication {
    name: "navigationdb"
    cpp.cxxLanguageVersion: "c++17"
    Depends { name: "Qt"; submodules: ["core", "quick"] }
    Depends { name: "QtOrm" }
    files: [
        "domain/province.cpp", "domain/province.h",
        "domain/community.cpp", "domain/community.h",
        "main.cpp",
        "navigationdb.qrc",
    ]
}
