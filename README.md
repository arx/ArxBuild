# ArxBuild - ArxFatals buildscripts

## Dependencies

bash
gcc

## Generate build reports

Initialize the git repository containing the project to be tested (location can be configured in 'config').
`$ git clone git://github.com/lubosz/ArxFatalis.git repo`

`$ ./autobuild`

console2html.cpp will be compiled automatically.

## Generate statistics and index pages

`$ ./buildstats [-f]`

Unless the optional parameter '-f' is given, only pages with changed build logs are generated.

This is automatically called by 'autobuild' after each build.

## Clean 

Remove any build logs for commits that are not referenced by branches.
`$ ./clean`

## Links

[Arx Libertatis (Arx Fatalis port)](https://github.com/lubosz/ArxFatalis)

Chat: \#arxfatalis on irc.freenode.net

[Arx Wiki](http://arx.parpg.net/)
