if not get_option('tests')
  subdir_done()
endif

test_env = environment()
test_env.set('G_DEBUG', 'gc-friendly,fatal-warnings')
test_env.set('MALLOC_CHECK_', '2')
test_env.set('NO_AT_BRIDGE', '1')
test_env.set('GSETTINGS_BACKEND', 'memory')
test_env.set('GSETTINGS_SCHEMA_DIR', meson.project_build_root() / 'data')

tests = ['grab-word', 'load-layouts', 'capitalize-by-template', 'osk-widget']

foreach name : tests
  name = 'test-@0@'.format(name)
  test = executable(name, '@0@.c'.format(name), pie: true, dependencies: libpos_dep)
  test('@0@'.format(name), test, depends: compile_schemas, env: test_env)
endforeach
