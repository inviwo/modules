node {
    stage('Fetch') {
        dir('modules') {
            checkout scm
            sh 'git submodule sync --recursive'
            sh 'git submodule update --init --recursive'
        }
        dir('inviwo') {
            git([url: 'https://github.com/inviwo/inviwo.git', branches: '*/master'])
        }
    }

    def util = load "${env.WORKSPACE}/inviwo/tools/jenkins/util.groovy"
    util.config(this)

    writeFile file: 'inviwo/CMakeUserPresets.json', text: """
    {
        "version": 6,
        "cmakeMinimumRequired": { "major": 3, "minor": 23, "patch": 0 },
        "configurePresets": [
            {
                "name": "ninja-developer-modules",
                "displayName": "Ninja Developer configuration",
                "inherits" : ["ninja-developer", "modules", "modules-vcpkg"],
                "cacheVariables": {
                    "VCPKG_TARGET_TRIPLET" : "x64-osx-dynamic",
                    "VCPKG_MANIFEST_FEATURES" : ""
                }
            }
        ]
    }
    """

    def modulePaths = ["${env.WORKSPACE}/modules/misc",
                       "${env.WORKSPACE}/modules/molvis",
                       "${env.WORKSPACE}/modules/tensorvis",
                       "${env.WORKSPACE}/modules/topovis",
                       "${env.WORKSPACE}/modules/vectorvis"]
    util.wrap(this, "#jenkins-branch-pr") {
        util.format(this, "${env.WORKSPACE}/modules")
        util.buildStandard(
            state: this,
            modulePaths: modulePaths,
            onModules: ["OPENMESH", "SPRINGSYSTEM", "VASP",
                        "NANOVGUTILS", "TENSORVISBASE", "INTEGRALLINEFILTERING",
                        "MOLVISBASE", "MOLVISGL", "MOLVISPYTHON",
                        "DATAFRAMECLUSTERING"],
            offModules: ["VTK", "TENSORVISIO", "TTK", "GRAPHVIZ"],
            opts: [:],
            preset: "ninja-developer-modules"
        )
        util.warn(this, 'daily/modules/appleclang')
        util.unittest(this)
        util.integrationtest(this)
        util.regression(this, modulePaths)
        util.copyright(this)
        util.doxygen(this)
    }
}
