include(RunMCBERepair)

set(test_db "${RunMCBERepair_BINARY_DIR}/TestWorld")

extract_world("${test_db}"
    "${RunMCBERepair_SOURCE_DIR}/../minecraftWorlds/TestWorld01.mcworld")

run_mcberepair(NoArgs rmkeys)
run_mcberepair(OneArg rmkeys "${test_db}")
run_mcberepair(OneArgPostTest listkeys "${test_db}")

run_mcberepair(MultiArgs rmkeys "${test_db}"
    "@5:0:0:54" "@5:0:0:118" "AutonomousEntities" "@5:0:0:47-5")
run_mcberepair(MultiArgsPostTest listkeys "${test_db}")

run_mcberepair(BadCommand rmkeys noexist)

run_mcberepair(Help help rmkeys)

file(REMOVE_RECURSE "${test_db}")
