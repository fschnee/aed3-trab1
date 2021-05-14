#!/usr/bin/env sh

if [ ! -d "builddir" ]
then
    meson builddir
fi

meson compile -C builddir && ./builddir/aed3_trab1 "$@"
