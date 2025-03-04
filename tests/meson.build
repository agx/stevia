if get_option('tests')

  test_env = environment()
  test_env.set('G_DEBUG', 'gc-friendly,fatal-warnings')
  test_env.set('MALLOC_CHECK_', '2')
  test_env.set('NO_AT_BRIDGE', '1')
  test_env.set('GSETTINGS_BACKEND', 'memory')

  grab_word_test = executable(
    'test-grab-word',
    'test-grab-word.c',
    pie: true,
    dependencies: libpos_dep,
  )
  test('grab-word', grab_word_test, env: test_env)

  load_layouts_test = executable(
    'test-load-layouts',
    'test-load-layouts.c',
    pie: true,
    dependencies: libpos_dep,
  )
  test('load-layouts', load_layouts_test, env: test_env)

  capitalize_by_template_test = executable(
    'test-capitalize-by-template',
    'test-capitalize-by-template.c',
    pie: true,
    dependencies: libpos_dep,
  )
  test('capitalize-by-template', capitalize_by_template_test, env: test_env)

endif
