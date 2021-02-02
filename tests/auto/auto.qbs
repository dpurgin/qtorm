import qbs

Project {
    references: [
        "domainclasses/domainclasses.qbs",
        "qormentityinstancecache/qormentityinstancecache.qbs",
        "qormmetadatacache/qormmetadatacache.qbs",
        "qormsession/qormsession.qbs",
        "qormfilterexpression/qormfilterexpression.qbs",
        "qormsqlitestatementgenerator/qormsqlitestatementgenerator.qbs",
    ]
}

