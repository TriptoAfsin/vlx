#!/bin/sh
# Print the VLZ feature cuts from extras/vlz/vlz.ini as Meson -D flags.
#
# build.sh appends these after its own flags (see the "VLZ:" hook there), so they win over its
# hard-coded ones such as -Dskins2=enabled. A cross file alone would lose: Meson gives command-line
# options priority over [project options] in machine files.
ini="$(dirname "$0")/vlz.ini"
sed -n '/^\[project options\]/,/^\[/p' "$ini" |
    grep -E '^[A-Za-z0-9_-]+[[:space:]]*=' |
    sed -e 's/[[:space:]]*#.*$//' \
        -e "s/^\([A-Za-z0-9_-]*\)[[:space:]]*=[[:space:]]*/-D\1=/" \
        -e "s/[][' ]//g" |
    tr '\n' ' '
