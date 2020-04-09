include(RunMCBERepair)

set(test_db "${RunMCBERepair_BINARY_DIR}/TestWorld")

extract_world("${test_db}"
    "${RunMCBERepair_SOURCE_DIR}/../minecraftWorlds/TestWorld01.mcworld")

run_mcberepair(NoArgs listkeys)
run_mcberepair(OneArg listkeys "${test_db}")

file(REMOVE_RECURSE "${test_db}")
