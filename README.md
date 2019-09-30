# ArxBuild - Arx Libertatis automatic testing

This repository contains code to test [Arx Libertatis](https://github.com/arx/ArxLibertatis) under various compilers and configurations.

## Dependencies

bash

gcc

## Generate build reports

Initialize the git repository containing the project to be tested (location can be configured in `config`).

`$ git clone git://github.com/lubosz/ArxFatalis.git repo`

`$ ./autobuild [options] [project]`

Available options:

Available options:

* `--nofetch` (`-n`): Don't update the GIT repository before building

console2html.cpp will be compiled automatically.

## Generate statistics and index pages

`$ ./buildstats [options]`

Available options:

* `--force` (`-f`): Regenerate all index pages even if we think nothing changed

Unless the optional parameter `-f` is given, only pages with changed build logs are generated.

This is automatically called by `autobuild` after each build.

## Clean 

Remove any build logs for commits that are not referenced by branches.

`$ ./clean`

To do the same for binaries, run:

`$ ./cleanbin`

## Contact

IRC: \#arxfatalis on irc.freenode.net

Website: [arx-libertatis.org](https://arx-libertatis.org/)

Wiki: [wiki.arx-libertatis.org](https://wiki.arx-libertatis.org/)
