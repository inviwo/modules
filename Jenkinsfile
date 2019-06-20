node {
    stage('Fetch') { 
        dir('modules') {
            checkout scm
            sh 'git submodule sync --recursive'
            sh 'git submodule update --init --recursive'
        }
        dir('inviwo') {
            git([url: 'https://github.com/inviwo/inviwo.git', branches: '*/master'])
            sh 'git submodule sync --recursive'
            sh 'git submodule update --init --recursive'
        }
    }

    def util = load "${env.WORKSPACE}/inviwo/tools/jenkins/util.groovy"          
    if(!env.disabledProperties) properties(util.defaultProperties())
    util.printMap("Environment", env.getEnvironment())

    Map state = [
        env: env,
        build: currentBuild, 
        errors: [],
        display: 0,
        addLabel: {label -> },
        removeLabel: {label -> }
    ]

    def modulePaths = ["${env.WORKSPACE}/modules/misc", "${env.WORKSPACE}/modules/tensorvis", "${env.WORKSPACE}/modules/topovis"]
    util.wrap(state, "#jenkins-branch-pr") {
        util.buildStandard(
            state: state,
            modulePaths: modulePaths, 
            onModules: ["TOPOLOGYTOOLKIT"],  
            offModules: ["ABUFFERGL"],
            opts: [:]
        )
        util.filterfiles()
        util.format(state)
        util.warn(state 'daily/modules/appleclang')
        util.unittest(state)
        util.integrationtest(state)        
        util.regression(state, modulePaths)
        util.copyright(state)    
        util.doxygen(state)
    }
}
