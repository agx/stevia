project('phosh-osk-stub', 'c',
          version: '0.24.0',
          license: 'GPLv3+',
    meson_version: '>= 0.54.0',
  default_options: [ 'warning_level=1', 'buildtype=debugoptimized', 'c_std=gnu11' ],
)

prefix = get_option('prefix')
bindir = join_paths(prefix, get_option('bindir'))
datadir = join_paths(prefix, get_option('datadir'))
localedir = join_paths(prefix, get_option('localedir'))
libdir = join_paths(prefix, get_option('libdir'))
libexecdir = join_paths(prefix, get_option('libexecdir'))
desktopdir = join_paths(datadir, 'applications')
sessiondir = join_paths(datadir, 'gnome-session')
pkgdatadir = join_paths(datadir, meson.project_name())
pkglibdir = join_paths(libdir, meson.project_name())
systemddir = join_paths(prefix, 'lib/systemd')
systemduserdir = join_paths(systemddir, 'user')
presagedictdir = datadir / 'phosh' / 'osk' / 'presage'
schemasdir = datadir / 'glib-2.0' / 'schemas'

glib_ver = '2.62'
glib_ver_str = 'GLIB_VERSION_@0@'.format('2_62')
glib_ver_cmp = '>=@0@'.format(glib_ver)

add_project_arguments([
  '-DGLIB_VERSION_MIN_REQUIRED=@0@'.format(glib_ver_str),
  '-DGLIB_VERSION_MAX_REQUIRED=@0@'.format(glib_ver_str),
  '-DG_LOG_USE_STRUCTURED',
], language: 'c')

root_inc = include_directories('.')

cc = meson.get_compiler('c')

global_c_args = []
test_c_args = [
  '-Wcast-align',
  '-Wdate-time',
  '-Wdeclaration-after-statement',
  ['-Werror=format-security', '-Werror=format=2'],
  '-Wendif-labels',
  '-Werror=incompatible-pointer-types',
  '-Werror=missing-declarations',
  '-Werror=overflow',
  '-Werror=return-type',
  '-Werror=shift-count-overflow',
  '-Werror=shift-overflow=2',
  '-Werror=implicit-fallthrough=3',
  '-Werror=implicit-function-declaration',
  '-Wfloat-equal',
  '-Wformat-nonliteral',
  '-Wformat-security',
  '-Winit-self',
  '-Wmaybe-uninitialized',
  '-Wmissing-field-initializers',
  '-Wmissing-include-dirs',
  '-Wmissing-noreturn',
  '-Wnested-externs',
  '-Wno-missing-field-initializers',
  '-Wno-sign-compare',
  '-Wno-strict-aliasing',
  '-Wno-unused-parameter',
  '-Wold-style-definition',
  '-Wpointer-arith',
  '-Wredundant-decls',
  '-Wshadow',
  '-Wstrict-prototypes',
  '-Wswitch-default',
  '-Wswitch-enum',
  '-Wtype-limits',
  '-Wundef',
  '-Wunused-function',
]
if get_option('buildtype') != 'plain'
  test_c_args += '-fstack-protector-strong'
endif

foreach arg: test_c_args
  if cc.has_multi_arguments(arg)
    global_c_args += arg
  endif
endforeach
add_project_arguments(
  global_c_args,
  language: 'c'
)

gnome = import('gnome')
i18n  = import('i18n')

glib_dep = dependency('glib-2.0', version: glib_ver_cmp)
gio_dep = dependency('gio-2.0', version: glib_ver_cmp)
gobject_dep = dependency('gobject-2.0', version: glib_ver_cmp)
gnome_desktop_dep = dependency('gnome-desktop-3.0', version: '>=3.26')
gsettings_desktop_schemas_dep = dependency('gsettings-desktop-schemas')
gtk_dep = dependency('gtk+-3.0', version: '>=3.22')
gtk_wayland_dep = dependency('gtk+-wayland-3.0', version: '>=3.22')
json_glib_dep =  dependency('json-glib-1.0')
libfeedback_dep = dependency('libfeedback-0.0',
  fallback: ['libfeedback', 'libfeedback_dep'],
  default_options: ['introspection=disabled', 'daemon=false', 'gtk_doc=false']
  )
libhandy_dep = dependency('libhandy-1',
  version: '>=1.1.90',
  fallback: ['libhandy', 'libhandy_dep'],
  default_options: ['introspection=disabled']
			 )
libsystemd_dep = dependency('libsystemd', version: '>= 241')
wayland_client_dep = dependency('wayland-client', version: '>=1.14')
wayland_protos_dep = dependency('wayland-protocols', version: '>=1.12')
xkbcommon_dep = dependency('xkbcommon')

# For completion engines
presage_dep = cc.find_library('presage')

config_h = configuration_data()
config_h.set_quoted('GETTEXT_PACKAGE', 'phosh-osk-stub')
config_h.set_quoted('LOCALEDIR', localedir)
config_h.set_quoted('PHOSH_OSK_STUB_VERSION', meson.project_version())
config_h.set_quoted('PRESAGE_DICT_DIR', presagedictdir)

configure_file(
  input: 'pos-config.h.in',
  output: 'pos-config.h',
  configuration: config_h,
)

run_data = configuration_data()
run_data.set('ABS_BUILDDIR', meson.current_build_dir())
run_data.set('ABS_SRCDIR', meson.current_source_dir())
configure_file(
  input: 'run.in',
  output: 'run',
  configuration: run_data)

summary = [
  '',
  '--------------------',
  'phosh-osk-stub @0@'.format(meson.project_version()),
  '',
  '  Default OSK: @0@'.format(get_option('default_osk')),
  'Documentation: @0@'.format(get_option('gtk_doc')),
  '     Manpages: @0@'.format(get_option('man')),
  '------------------',
]

message('\n'.join(summary))


subdir('data')
subdir('po')
subdir('protocol')
subdir('src')
subdir('tests')
subdir('doc')

# gnome.post_install() is available since meson 0.59.0
# Distributions use their own tooling (e.g. postinst, triggers, etc)
# so it is okay if the post_install() is not run on distro builds
if meson.version().version_compare('>=0.59.0')
  gnome.post_install(
    glib_compile_schemas: true,
    gtk_update_icon_cache: true,
  )
endif
