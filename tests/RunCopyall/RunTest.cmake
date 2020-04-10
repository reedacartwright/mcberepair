include(RunMCBERepair)

run_mcberepair(Help help copyall)

run_mcberepair(NoArgs copyall)
run_mcberepair(OneArg copyall noexist)

run_mcberepair(BadCommand1 copyall noexist noexist)

set(test_db "${RunMCBERepair_BINARY_DIR}/TestWorld")
set(copy_db "${RunMCBERepair_BINARY_DIR}/CopyWorld")

extract_world("${test_db}"
    "${RunMCBERepair_SOURCE_DIR}/../minecraftWorlds/TestWorld01.mcworld")

run_mcberepair(BadCommand2 copyall "${test_db}" noexist)

file(MAKE_DIRECTORY "${copy_db}/db") 

run_mcberepair(TwoArgs copyall "${test_db}" "${copy_db}")
run_mcberepair(TwoArgsPostTest listkeys "${copy_db}")


file(REMOVE_RECURSE "${test_db}" "${copy_db}")
