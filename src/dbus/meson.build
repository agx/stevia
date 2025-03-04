dbus_inc = include_directories('.')

generated_dbus_sources = []

generated_dbus_sources += gnome.gdbus_codegen(
  'pos-osk0-dbus',
  files('sm.puri.OSK0.xml'),
  interface_prefix: 'sm.puri.',
  namespace: 'PosDbus',
)
