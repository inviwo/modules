#ifdef _MSC_VER
#pragma comment(linker, "/SUBSYSTEM:CONSOLE")
#endif

#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/common/inviwoapplication.h>
#include <inviwo/core/util/logcentral.h>
#include <inviwo/core/common/coremodulesharedlibrary.h>
#include <modules/tensorvisbase/tensorvisbasemodule.h>
#include <modules/tensorvisbase/tensorvisbasemodulesharedlibrary.h>

#include <warn/push>
#include <warn/ignore/all>
#include <gtest/gtest.h>
#include <warn/pop>

using namespace inviwo;

int main(int argc, char** argv) {

    inviwo::LogCentral::init();

    InviwoApplication app(argc, argv, "Inviwo-Unittests-TensorVisBase");
    {
        std::vector<std::unique_ptr<InviwoModuleFactoryObject>> modules;
        modules.emplace_back(createInviwoCore());
        modules.emplace_back(createTensorVisBaseModule());
        app.registerModules(std::move(modules));
    }

    int ret = -1;
    {

#ifdef IVW_ENABLE_MSVC_MEM_LEAK_TEST
        VLDDisable();
        ::testing::InitGoogleTest(&argc, argv);
        VLDEnable();
#else
        ::testing::InitGoogleTest(&argc, argv);
#endif
        ret = RUN_ALL_TESTS();
    }

    return ret;
}