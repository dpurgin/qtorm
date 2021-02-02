import qbs

QtApplication {
    name: "tst_ormfilterexpression"
    type: ["application", "autotest"]
    cpp.cxxLanguageVersion: "c++17"
    Depends { name: "Qt"; submodules: ["core", "test"] }
    Depends { name: "QtOrm" }
    files: ["tst_ormfilterexpression.cpp"]
}
