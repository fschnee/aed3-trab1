project('aed3-trab1', 'cpp',
    version : '0.0',
    default_options : [
        'buildtype=release',
        'warning_level=3',
        'cpp_std=c++2a',
    ],
    meson_version: '>= 0.52',
)

deps = [
    dependency('threads'),
    dependency('fmt', version: '>= 7.0.0', fallback: ['fmt', 'fmt_dep'], include_type: 'system'),
]

exe = executable(
    'aed3_trab1',
    'src/main.cpp',
    include_directories: include_directories('src'),
    dependencies: deps
)
