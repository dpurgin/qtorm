import qbs

QtApplication {
    name: "tst_entityinstancecache"
    type: ["application", "autotest"]
    cpp.cxxLanguageVersion: "c++17"
    Depends { name: "Qt"; submodules: ["core", "test"] }
    Depends { name: "QtOrm" }
    files: [
        "tst_entityinstancecache.cpp",
        "domain/province.cpp", "domain/province.h",
        "domain/town.cpp", "domain/town.h",
    ]
}
