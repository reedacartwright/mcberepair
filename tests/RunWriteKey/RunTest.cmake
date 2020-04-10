include(RunMCBERepair)

set(test_db "${RunMCBERepair_BINARY_DIR}/TestWorld")

extract_world("${test_db}"
    "${RunMCBERepair_SOURCE_DIR}/../minecraftWorlds/TestWorld01.mcworld")

run_mcberepair(Help help writekey)
run_mcberepair(NoArgs writekey)
run_mcberepair(OneArg writekey "${test_db}")

run_mcberepair(TwoArgs writekey "${test_db}" "test_record")
run_mcberepair(TwoArgsPostTest dumpkey "${test_db}" "test_record")

run_mcberepair(BadCommand writekey noexist nokey)

file(REMOVE_RECURSE "${test_db}")
