# download latest lcov
lcov_commit="40580cd65909bc8324ae09b36bca2e178652ff3f"

cd $1

if [[ ! -f ./lcov/bin/lcov ]]; then
    mkdir -p lcov
    curl -sL  https://github.com/linux-test-project/lcov/archive/${lcov_commit}.tar.gz |
    tar --strip-components=1 -xz -C lcov
fi

lcov_dir=$(realpath ./lcov/bin)

PATH="${PATH}:${lcov_dir}"

# Run lcov
lcov --directory . --capture --output-file coverage.info
lcov --remove coverage.info '/usr/*' '*/leveldb-mcpe/*' '*/tests/*' '*/examples/*' '*/contrib/*' --output-file coverage.info

# Output summary to console
lcov --list coverage.info 

# Upload report to CodeCov
#bash <(curl -s https://codecov.io/bash) || echo "CodeCov failed to collect reports."
