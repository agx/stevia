Title: Testing Keyboard Layouts
Slug: testinglayouts

Testing layout changes or experimenting with new layouts is simple due
to GLib's [resource overlays][]. This mechanism allows you to replace
or add files to stevia's build in resource bundle without rebuilding
stevia.

Let's assume you you have the layout's JSON in the current directory
in a file named `de.json`. You can then use that layout by running:

```sh
G_RESOURCE_OVERLAYS=/mobi/phosh/stevia/layouts=$PWD phosh-osk-stevia --replace
```

This puts all files in the current directory at the resource path
`/mobi/phosh/stevia/layouts` so the layout would appear at
`/mobi/phosh/stevia/layouts/de.json` and thus replace the existing
layout there. Adding new layouts for testing works the same way.

If everything works you should see this on the console:

```console
GLib-GIO-Message: Adding GResources overlay '/mobi/phosh/stevia/layouts=/path/to/current/dir'
GLib-GIO-Message: Mapped file '/path/to/current/dir/de.json' as a resource overlay
```

This informs you that stevia picked up your modified layout (note that
`/path/to/current/dir/` depends on your current working directory so
the output might be slightly different).

If you need to make further changes just stop stevia and run the above
command again. This allows for quick test cycles without risking to
break your system.

[resource overlays]: https://docs.gtk.org/gio/struct.Resource.html#overlays
