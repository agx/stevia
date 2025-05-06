desktop_file = 'sm.puri.OSK0.desktop'
target = 'sm.puri.OSK0.desktop'

if get_option('default_osk') == false
  target = '@0@.desktop'.format(app_id)
endif

desktopconf = configuration_data()
desktopconf.set('bindir', bindir)

i18n.merge_file(
  input: configure_file(
    input: desktop_file + '.in.in',
    output: desktop_file + '.in',
    configuration: desktopconf,
  ),
  output: target,
  po_dir: '../po',
  install: true,
  install_dir: desktopdir,
  type: 'desktop',
)
#generate XML enum definitions for GSettings schema
schema_enum_headers = files('../src/phosh-osk-enums.h')
generate_enums_schema = gnome.mkenums(
  'sm.puri.phosh.osk.enums.xml',
  sources: schema_enum_headers,
  comments: '<!-- @comment@ -->',
  fhead: '<schemalist>',
  vhead: '  <@type@ id="mobi.phosh.osk.@EnumName@">',
  vprod: '    <value nick="@valuenick@" value="@valuenum@"/>',
  vtail: '  </@type@>',
  ftail: '</schemalist>',
  install_header: true,
  install_dir: schemasdir,
)

#workaround due to https://github.com/mesonbuild/meson/issues/1687
copy_schema = custom_target(
  'copy-gschema-to-builddir',
  input: 'sm.puri.phosh.osk.gschema.xml',
  output: 'sm.puri.phosh.osk.gschema.xml',
  command: ['cp', '@INPUT@', '@OUTPUT@'],
)
schemas = ['sm.puri.phosh.osk.gschema.xml']
compile_schemas = custom_target(
  'glib-compile-schemas',
  build_by_default: true,
  output: 'gschemas.compiled',
  install: false,
  command: [find_program('glib-compile-schemas'), meson.current_build_dir()],
  depends: [generate_enums_schema, copy_schema],
)
install_data(schemas, install_dir: schemasdir)

glib_compile_schemas = find_program('glib-compile-schemas', required: false)
if glib_compile_schemas.found()
  test(
    'Validate schema file',
    glib_compile_schemas,
    args: ['--strict', '--dry-run', meson.current_build_dir()],
  )
endif

appstream_file = i18n.merge_file(
  input: '@0@.metainfo.xml.in'.format(app_id),
  output: '@0@.metainfo.xml'.format(app_id),
  po_dir: '../po',
  install: true,
  install_dir: join_paths(get_option('datadir'), 'metainfo'),
)

appstream_util = find_program('appstreamcli', required: false)
if appstream_util.found()
  test('Validate appstream file', appstream_util, args: ['validate', '--no-net', appstream_file])
endif

subdir('completers')
