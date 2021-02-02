import qbs
import qbs.FileInfo

Project {
    name: "qtorm"

    minimumQbsVersion: "1.6"
    property bool withDocumentation: true
    property bool withExamples: true
    property bool withTests: true

    references: [
        "src/src.qbs",
    ]

//    SubProject {
//        filePath: "doc/doc.qbs"
//        Properties {
//            condition: parent.withDocumentation
//        }
//    }

    SubProject {
        filePath: "examples/examples.qbs"
        Properties {
            condition: parent.withExamples
        }
    }
    SubProject {
        filePath: "tests/tests.qbs"
        Properties {
            condition: parent.withTests
        }
    }
    AutotestRunner {
        Depends { name: "QtOrm" }
        environment: {
            var path = FileInfo.joinPaths(qbs.installRoot, qbs.installPrefix, "lib");
            return base.concat(["LD_LIBRARY_PATH=" + path]);
        }
    }
}
