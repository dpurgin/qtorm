import qbs

QtApplication {
    name: "tst_sqlitestatementgenerator"
    type: ["application", "autotest"]
    cpp.cxxLanguageVersion: "c++17"
    Depends { name: "Qt"; submodules: ["core", "test"] }
    Depends { name: "QtOrm" }
    files: [
        "domain/person.cpp", "domain/person.h",
        "domain/province.cpp", "domain/province.h",
        "domain/town.cpp", "domain/town.h",
        "domain/community.cpp", "domain/community.h",
        "domain/withqvariant.cpp", "domain/withqvariant.h",
        "tst_sqlitestatementgenerator.cpp"]
}
