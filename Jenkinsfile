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
    def rootDir = pwd()
    def util = load "${rootDir}/inviwo/tools/jenkins/util.groovy"          
    properties(util.defaultProperties())
 
    try {
        def external = [${env.WORKSPACE}/modules/misc]
        def extraOn = []
        def off = ["ABUFFERGL" , "DISCRETEDATA", "GLFW", "HDF5"]

        util.buildStandard(params, external, extraOn, off)
        util.warn()
        util.unittest()
        util.integrationtest()        
        util.regression(currentBuild, env)
        util.copyright()
        util.doxygen()       
        util.publish()

        currentBuild.result = 'SUCCESS'
    } catch (e) {
        currentBuild.result = 'FAILURE'
        throw e
    } finally {
        util.slack(currentBuild, env)
    }
}
