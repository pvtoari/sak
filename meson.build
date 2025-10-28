project('sak', 'c',
    version: run_command('cat', join_paths(meson.project_source_root(), 'VERSION'), check: true).stdout().strip(),
    default_options : [
    	'c_std=c99',
    	'b_staticpic=false',
    	'b_pie=false',
    	'b_lto=false',
    	'buildtype=debugoptimized',
    	'warning_level=2',
    ]
)

c_std = 'c99'
unsupported_options = ['b_staticpic', 'b_pie', 'b_lto']
cc = meson.get_compiler('c')

if meson.has_external_property('ld')
    ld = meson.get_external_property('ld')
else
    ld = ''
    if ld == ''
        ld = 'ld'
    endif

    if not meson.is_cross_build()
        ld_env = run_command('sh', '-c', 'printf "%s\\n" "$LD"', check: true).stdout().strip()
        if ld_env != ''
            ld = ld_env
        endif
    endif
endif

if ld == 'ld'
    ld = 'ld.bfd'
endif

inc = include_directories('include')
src = files(
	'src/kernel.c'
)

dependencies = [
    subproject('cc-runtime').get_variable('cc_runtime_dep'),
    subproject('freestnd-c-hdrs').get_variable('freestnd_c_hdrs_dep'),
    subproject('limine-protocol').get_variable('limine_protocol_dep'),
]

exec_name = meson.project_name()
if get_option('c_std') != c_std
    error(f'@exec_name@ needs to build as @c_std@')
endif

cflags = []
ldflags = run_command('sh', '-c', 'printf "%s\\n" "$LDFLAGS"', check: true).stdout().split()

cflags += [
    '-nostdinc',
    '-ffreestanding',
    '-fno-stack-protector',
    '-fno-stack-check',
    '-fno-lto',
    '-fno-PIC',
    '-ffunction-sections',
    '-fdata-sections',
    '-DLIMINE_API_REVISION=4',
]

cflags += [
        '-m64',
        '-march=x86-64',
        '-mabi=sysv',
        '-mgeneral-regs-only',
        '-mno-red-zone',
        '-mcmodel=kernel',
    ]

ldflags += [
    '-nostdlib',
    '-static',
    '-z', 'max-page-size=0x1000',
    '--gc-sections',
    '-T', '../x86_64.lds'
]

output_static_lib = static_library(
    exec_name,
    sources: src,
    include_directories: inc,
    dependencies: dependencies,
    c_args: cflags,
    install: false,
)

custom_target(
    output: exec_name + '-x86_64',
    input: output_static_lib,
    command: [
        ld,
        ldflags,
        '--whole-archive',
        '@INPUT@',
        '--no-whole-archive',
        '-o', '@OUTPUT@',
    ],
    install: true,
    install_dir: get_option('datadir') / exec_name,
    install_mode: 'rw-r--r--',
)
