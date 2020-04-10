cmake_policy(SET CMP0007 NEW)

include(RunMCBERepair)

run_mcberepair(Help help rmkeys)
run_mcberepair(NoArgs rmkeys)

set(test_db "${RunMCBERepair_BINARY_DIR}/TestWorld")

extract_world("${test_db}"
    "${RunMCBERepair_SOURCE_DIR}/../minecraftWorlds/TestWorld01.mcworld")

run_mcberepair(OneArg rmkeys "${test_db}")
run_mcberepair(OneArgPostTest listkeys "${test_db}")

run_mcberepair(MultiArgs rmkeys "${test_db}"
    "@5:0:0:54" "@5:0:0:118" "AutonomousEntities" "@5:0:0:47-5")
run_mcberepair(MultiArgsPostTest listkeys "${test_db}")

run_mcberepair(BadCommand rmkeys noexist)

run_mcberepair(PercentDecode rmkeys "${test_db}"
    "Test%09%AF%af" "Test%!0" "Test%0~" "Test%" "Test%F"
)

run_mcberepair(BadKeys rmkeys "${test_db}"
    ""
    "@" "@0" "@0:" "@0:0" "@0:0:" "@0:0:0" "@0:0:0:" "@0:0:0:0-"
    "@a" "@0a" "@0:a" "@0:0a" "@0:0:a" "@0:0:0a" "@0:0:0:a" "@0:0:0:0a" "@0:0:0:0-a" "@0:0:0:0-0a"
)

file(REMOVE_RECURSE "${test_db}")
