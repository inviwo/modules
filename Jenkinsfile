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
    println "Env:" + env.getEnvironment()?.collect{"${it.key.padLeft(25)} = ${it.value}"}?.join("\n") ?: ''

    Map state = [
        env: env,
        build: currentBuild, 
        errors: [],
        display: 0,
        addLabel: {label -> },
        removeLabel: {label -> }
    ]

    def modulePaths = ["${env.WORKSPACE}/modules/misc"]
    try {
        util.buildStandard(
            state: state,
            modulePaths: modulePaths, 
            onModules: [],  
            offModules: ["ABUFFERGL"]
            opts: [:]
        )
        util.filterfiles()
        util.format(state)
        util.warn(state)
        util.unittest(state)
        util.integrationtest(state)        
        util.regression(state, modulePaths)
        util.copyright(state)    
        util.doxygen(state)

        state.build.result = state.errors.isEmpty() ? 'SUCCESS' : 'UNSTABLE'
    } catch (e) {
        currentBuild.result = 'FAILURE'
        throw e
    } finally {
        util.slack(state, "#jenkins-branch-pr")
        if (!state.errors.isEmpty()) {
            println "Errors in: ${state.errors.join(", ")}"
            state.build.description = "Errors in: ${state.errors.join(' ')}"
        }
    }
}
