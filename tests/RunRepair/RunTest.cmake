include(RunMCBERepair)

run_mcberepair(Help help repair)

run_mcberepair(NoArgs repair)
run_mcberepair(BadCommand repair noexist)

set(test_db "${RunMCBERepair_BINARY_DIR}/TestWorld")

extract_world("${test_db}"
    "${RunMCBERepair_SOURCE_DIR}/../minecraftWorlds/TestWorld01.mcworld")

run_mcberepair(OneArg repair "${test_db}")

file(REMOVE_RECURSE "${test_db}")
