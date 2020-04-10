include(RunMCBERepair)

set(test_db "${RunMCBERepair_BINARY_DIR}/TestWorld")

extract_world("${test_db}"
    "${RunMCBERepair_SOURCE_DIR}/../minecraftWorlds/TestWorld01.mcworld")

run_mcberepair(Help help dumpkey)
run_mcberepair(NoArgs dumpkey)
run_mcberepair(OneArg dumpkey "${test_db}")

run_mcberepair(TwoArgs dumpkey "${test_db}" "HelloWorld")

run_mcberepair(BadCommand dumpkey noexist nokey)

file(REMOVE_RECURSE "${test_db}")
