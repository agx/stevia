if get_option('tests')

  test_env = environment()
  test_env.set('G_DEBUG', 'gc-friendly,fatal-warnings')
  test_env.set('MALLOC_CHECK_', '2')
  test_env.set('NO_AT_BRIDGE', '1')
  test_env.set('GSETTINGS_BACKEND', 'memory')

  tests = ['grab-word', 'load-layouts', 'capitalize-by-template']

  foreach name : tests
    name = 'test-@0@'.format(name)
    test = executable(name, '@0@.c'.format(name), pie: true, dependencies: libpos_dep)
    test('@0@'.format(name), test, env: test_env)
  endforeach

endif
