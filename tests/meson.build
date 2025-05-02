if not get_option('tests')
  subdir_done()
endif

test_env = environment()
test_env.set('G_DEBUG', 'gc-friendly,fatal-warnings')
test_env.set('MALLOC_CHECK_', '2')
test_env.set('NO_AT_BRIDGE', '1')
test_env.set('GSETTINGS_BACKEND', 'memory')
test_env.set('GSETTINGS_SCHEMA_DIR', meson.project_build_root() / 'data')

tests = ['grab-word', 'load-layouts', 'osk-widget', 'completers']
foreach name : tests
  name = 'test-@0@'.format(name)
  test = executable(name, '@0@.c'.format(name), pie: true, dependencies: libpos_dep)
  test('@0@'.format(name), test, depends: compile_schemas, env: test_env)
endforeach

if presage_dep.found()
  # Create a dummy db for testing
  presage_test_db = custom_target(
    'presage-test-db',
    input: meson.current_source_dir() / 'data' / 'input.txt',
    output: 'database_en.db',
    command: ['mk-presage-db', '@INPUT@', '@OUTPUT@'],
  )

  name = 'test-completer-presage'
  test(
    name,
    executable(
      name,
      f'@name@.c',
      pie: true,
      c_args: '-DPOS_PRESAGE_TEST_DB_DIR="@0@"'.format(meson.current_build_dir()),
      dependencies: libpos_dep,
    ),
    depends: [compile_schemas, presage_test_db],
    env: test_env,
  )
endif
