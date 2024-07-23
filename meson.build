project('hrry', 'cpp',
  version : '0.1.0',
  license : 'MIT',
  default_options : ['warning_level=3', 'cpp_std=c++17'])

os = build_machine.system()
deps = []

if os.contains('darwin')
  add_languages('objc')
elif os.contains('linux')
  deps += [dependency('x11')]
endif

message('running on -- ', os)
message('cpu_architecture -- ', build_machine.cpu_family(), build_machine.endian())

# Dependencies

#x11dep = dependency('x11')


message('the dependencies are: ')
foreach dep : deps
    message(' -', dep.name())
endforeach

hrry_inc = include_directories('include')

subdir('src')

install_man('share/man/man1/hrry.1')
#install_data( ['y','z'], install_dir : get_option('x') / 'hrry')
# -> share/hrry/y
# -> share/hrry/z
# install_data(sources : 'foo.dat', install_dir : '/etc') # -> /etc/foo.dat; abs path

#install_subdir('mydir', install_dir : 'include') # mydir subtree -> include/mydir


#subdir('tests')


# Run meson test
#test('simple test', exe)
