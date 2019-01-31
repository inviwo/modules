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
    properties(util.defaultProperties())

    def modulePaths = ["${env.WORKSPACE}/modules/misc"]
    def on = []
    def off = ["ABUFFERGL" , "DISCRETEDATA", "GLFW", "HDF5"]

    Map state = [
        env: env,
        params: params, 
        build: currentBuild, 
        errors: [],
        display: 0,
        addLabel: {label -> },
        removeLabel: {label -> }
    ]
 
    try {
        util.buildStandard(
            state: state,
            modulePaths: modulePaths, 
            onModules: on,  
            offModules: off
        )
        util.filterfiles()
        util.format(state)
        util.warn(state)
        util.unittest(state)
        util.integrationtest(state)        
        util.regression(state, modulePaths)
        util.copyright(state)    
        util.doxygen(state)

        currentBuild.result = 'SUCCESS'
    } catch (e) {
        currentBuild.result = 'FAILURE'
        throw e
    } finally {
        util.slack(state, "#jenkins-branch-pr")
    }
}
