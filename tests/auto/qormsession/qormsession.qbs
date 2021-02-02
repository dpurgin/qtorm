import qbs

QtApplication {
    name: "tst_ormsession"
    type: ["application", "autotest"]
    cpp.cxxLanguageVersion: "c++17"
    Depends { name: "Qt"; submodules: ["core", "sql", "test"] }
    Depends { name: "QtOrm" }
    files: [
        "domain/person.cpp", "domain/person.h",
        "domain/province.cpp", "domain/province.h",
        "domain/town.cpp", "domain/town.h",
        "tst_ormsession.cpp",
        "ormsession.qrc"]
}
