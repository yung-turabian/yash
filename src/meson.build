sources = ['builtin.cpp',
           'hrry_x11.cpp',
           'jobs.cpp',
           'shell.cpp',
           'util.cpp',
           'hrry.cpp',
           'io.cpp',
           'linker.cpp',
           'parser.cpp',
           'tokenizer.cpp'
          ]

executable(meson.project_name(),
           sources,
           include_directories : [hrry_inc],
           dependencies : deps,
           install : true,
           install_dir : '/usr/bin')
