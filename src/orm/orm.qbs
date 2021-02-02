import qbs
import qbs.File
import qbs.FileInfo
import qbs.TextFile

Project {
    DynamicLibrary {
        name: "QtOrm"
                  install: true
        Group {
            name: "public"
            files: [
                "qormabstractprovider.h",
                "qormclassproperty.h",
                "qormentityinstancecache.h",
                "qormentitylistmodel.h",
                "qormerror.h",
                "qormfilter.h",
                "qormfilterexpression.h",
                "qormglobal.h",
                "qormmetadata.h",
                "qormmetadatacache.h",
                "qormorder.h",
                "qormpropertymapping.h",
                "qormquery.h",
                "qormquerybuilder.h",
                "qormqueryresult.h",
                "qormrelation.h",
                "qormsession.h",
                "qormsessionconfiguration.h",
                "qormsqliteconfiguration.h",
                "qormsqliteprovider.h",
                "qormtransactiontoken.h",
            ]
            fileTags: ["public_headers"]
        }
        Group {
            name: "private"
            files: [
                "qormglobal_p.h",
                "qormmetadata_p.h",
                "qormsqlitestatementgenerator_p.h",
            ]
            fileTags: ["private_headers"]
        }
        Rule {
                inputs: ["public_headers"]
                outputArtifacts: [{
                    filePath: FileInfo.joinPaths(project.buildDirectory, "include", "QtOrm",
                                                 input.fileName),
                    fileTags: ["hpp"],
                }]
                outputFileTags: "hpp"
                prepare: {
                    var cmd = new JavaScriptCommand();
                    cmd.description = "copy " + input.fileName;
                    cmd.sourceCode = function() {
                        File.copy(input.filePath, output.filePath);
                        var file = new TextFile(input.filePath, TextFile.ReadOnly);
                        var regexp = /[\s]*class[\t ]+(Q_ORM_EXPORT[\t ]+)?(QOrm[A-Za-z0-9_]+)[^;]*$/
                        while (!file.atEof()) {
                            match = regexp.exec(file.readLine());
                            if (match) {
                                var path = FileInfo.joinPaths(project.buildDirectory, "include");
                                var headerFile = new TextFile(
                                            FileInfo.joinPaths(path, match[2]), TextFile.WriteOnly);
                                headerFile.write("#include \"" + input.filePath + "\"");
                            }
                        }
                        file.close();
                    };
                    return [cmd];
                }
        }
        Rule {
                inputs: ["private_headers"]
                outputArtifacts: [{
                    filePath: FileInfo.joinPaths(project.buildDirectory, "include", "QtOrm",
                                                 "private", input.fileName),
                    fileTags: ["hpp"],
                }]
                outputFileTags: "hpp"
                prepare: {
                    var cmd = new JavaScriptCommand();
                    cmd.description = "copy " + input.fileName;
                    cmd.sourceCode = function() {
                        File.copy(input.filePath, output.filePath);
                    };
                    return [cmd];
                }
        }

        Depends { name: "cpp" }
        cpp.includePaths: FileInfo.joinPaths(project.buildDirectory, "include")
        cpp.cxxLanguageVersion: "c++17"
        Depends { name: "Qt"; submodules: ["core", "sql"] }
        Export {
            Depends { name: "cpp" }
            cpp.includePaths: [
                FileInfo.joinPaths(project.buildDirectory, "include"),
                FileInfo.joinPaths(project.buildDirectory, "include", "QtOrm")
            ]
        }

        files: [
            "qormabstractprovider.cpp",
            "qormclassproperty.cpp",
            "qormentityinstancecache.cpp",
            "qormentitylistmodel.cpp",
            "qormerror.cpp",
            "qormfilter.cpp",
            "qormfilterexpression.cpp",
            "qormglobal.cpp",
            "qormglobal_p.cpp",
            "qormmetadata.cpp",
            "qormmetadatacache.cpp",
            "qormorder.cpp",
            "qormpropertymapping.cpp",
            "qormquery.cpp",
            "qormquerybuilder.cpp",
            "qormqueryresult.cpp",
            "qormrelation.cpp",
            "qormsession.cpp",
            "qormsessionconfiguration.cpp",
            "qormsqliteconfiguration.cpp",
            "qormsqliteprovider.cpp",
            "qormsqlitestatementgenerator_p.cpp",
            "qormtransactiontoken.cpp",
        ]
    }
}

