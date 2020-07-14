# Description
This is the a driver component for Sevenhugs technical test

# Requirements
Following tools are required to build the application and run the test:
* gcc
* lcov (for test coverage)

# How to build
To build, run `make`.

# How to test
To enable profiling and/or coverage, set `PROFILE=1` or `COVERAGE=1`.
Run tests using `make test` (optional), which invokes the `TEST_SUITE`
specified in Makefile or run your tests manually.
Running tests will write out profile and coverage information to `gmon.out`
(profile) and `*.gcda` for each object file (coverage).
Run `make profile` to generate `gprof.out`.
Run `make coverage` to generate `coverage/index.html` using `lcov`.

* make PROFILE=1 COVERAGE=1
* make test
* make PROFILE=1 COVERAGE=1 coverage profile
