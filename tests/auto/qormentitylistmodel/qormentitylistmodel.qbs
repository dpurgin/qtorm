import qbs

QtApplication {
    name: "tst_ormentitylistmodel"
    type: ["application", "autotest"]
    cpp.cxxLanguageVersion: "c++17"
    Depends { name: "Qt"; submodules: ["core", "test"] }
    Depends { name: "QtOrm" }
    files: [
        "domain/province.cpp", "domain/province.h",
        "domain/town.cpp", "domain/town.h",
        "tst_ormentitylistmodel.cpp",
        "resource.qrc"]
}
