if get_option('gtk_doc')

  toml_data = configuration_data()
  toml_data.set('VERSION', meson.project_version())

  pos_toml = configure_file(
    input: 'phosh-osk-stevia.toml.in',
    output: 'phosh-osk-stevia.toml',
    configuration: toml_data,
  )

  markdown_files = ['testing-layouts.md']

  dependency(
    'gi-docgen',
    version: '>= 2021.1',
    fallback: ['gi-docgen', 'dummy_dep'],
    native: true,
    required: get_option('gtk_doc'),
  )

  gidocgen = find_program('gi-docgen')

  docs_dir = datadir / 'doc'

  custom_target(
    'phosh-osk-stevia',
    input: [pos_toml, pos_gir[0]],
    output: 'pos-0',
    command: [
      gidocgen,
      'generate',
      '--quiet',
      '--add-include-path=@0@'.format(meson.current_build_dir() / '../../src'),
      '--config=@INPUT0@',
      '--output-dir=@OUTPUT@',
      '--no-namespace-dir',
      '--content-dir=@0@'.format(meson.current_source_dir()),
      '--fatal-warnings',
      '@INPUT1@',
    ],
    depend_files: markdown_files,
    build_by_default: true,
    install: true,
    install_dir: docs_dir,
  )

endif

if get_option('man')
  manpages = [['phosh-osk-stevia', 1]]

  rst2man = find_program('rst2man', 'rst2man.py', required: false)
  rst2man_flags = ['--syntax-highlight=none']

  foreach manpage : manpages
    man_name = manpage[0]
    man_section = manpage[1]

    custom_target(
      'man-@0@'.format(man_name),
      input: '@0@.rst'.format(man_name),
      output: '@0@.@1@'.format(man_name, man_section),
      command: [rst2man, rst2man_flags, '@INPUT@'],
      capture: true,
      install: true,
      install_dir: get_option('mandir') / 'man@0@'.format(man_section),
    )
  endforeach
endif
