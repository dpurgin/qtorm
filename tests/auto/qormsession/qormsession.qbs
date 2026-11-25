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
        "domain/withnotnull.cpp", "domain/withnotnull.h",
        "domain/withforeignkey.cpp", "domain/withforeignkey.h",
        "domain/withunique.cpp", "domain/withunique.h",
        "domain/withkeyword.cpp", "domain/withkeyword.h",
        "tst_ormsession.cpp",
        "ormsession.qrc"]
}
