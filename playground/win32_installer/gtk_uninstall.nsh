;-------------------------------------------------------------------
Delete "$INSTDIR\bin\asprintf.dll"
Delete "$INSTDIR\bin\bzip2.dll"
Delete "$INSTDIR\bin\charset.dll"
Delete "$INSTDIR\bin\fc-cache.exe"
Delete "$INSTDIR\bin\fc-list.exe"
Delete "$INSTDIR\bin\freetype6.dll"
Delete "$INSTDIR\bin\gdk-pixbuf-query-loaders.exe"
Delete "$INSTDIR\bin\gspawn-win32-helper.exe"
Delete "$INSTDIR\bin\gtk-query-immodules-2.0.exe"
Delete "$INSTDIR\bin\gtk-update-icon-cache.exe"
Delete "$INSTDIR\bin\gtkthemeselector.exe"
Delete "$INSTDIR\bin\iconv.dll"
Delete "$INSTDIR\bin\intl.dll"
Delete "$INSTDIR\bin\jpeg62.dll"
Delete "$INSTDIR\bin\libart_lgpl_2-2.dll"
Delete "$INSTDIR\bin\libatk-1.0-0.dll"
Delete "$INSTDIR\bin\libcairo-2.dll"
Delete "$INSTDIR\bin\libfontconfig-1.dll"
Delete "$INSTDIR\bin\libfreetype-6.dll"
Delete "$INSTDIR\bin\libgdk-win32-2.0-0.dll"
Delete "$INSTDIR\bin\libgdk_pixbuf-2.0-0.dll"
Delete "$INSTDIR\bin\libgdkglext-win32-1.0-0.dll"
Delete "$INSTDIR\bin\libglib-2.0-0.dll"
Delete "$INSTDIR\bin\libgmodule-2.0-0.dll"
Delete "$INSTDIR\bin\libgobject-2.0-0.dll"
Delete "$INSTDIR\bin\libgthread-2.0-0.dll"
Delete "$INSTDIR\bin\libgtk-win32-2.0-0.dll"
Delete "$INSTDIR\bin\libgtkglext-win32-1.0-0.dll"
Delete "$INSTDIR\bin\libpango-1.0-0.dll"
Delete "$INSTDIR\bin\libpangocairo-1.0-0.dll"
Delete "$INSTDIR\bin\libpangoft2-1.0-0.dll"
Delete "$INSTDIR\bin\libpangowin32-1.0-0.dll"
Delete "$INSTDIR\bin\libpng12.dll"
Delete "$INSTDIR\bin\libpng13.dll"
Delete "$INSTDIR\bin\libpopt-0.dll"
Delete "$INSTDIR\bin\libtiff3.dll"
Delete "$INSTDIR\bin\libxml2.dll"
Delete "$INSTDIR\bin\pango-querymodules.exe"
Delete "$INSTDIR\bin\reconfig.bat"
Delete "$INSTDIR\bin\xmlparse.dll"
Delete "$INSTDIR\bin\xmltok.dll"
Delete "$INSTDIR\bin\zlib1.dll"
Delete "$INSTDIR\bin\librsvg-2-2.dll"
RMDir  "$INSTDIR\bin"

Delete "$INSTDIR\etc\fonts\fonts.conf"
Delete "$INSTDIR\etc\fonts\fonts.dtd"
Delete "$INSTDIR\etc\fonts\local.conf"
RMDir  "$INSTDIR\etc\fonts"

Delete "$INSTDIR\etc\gtk-2.0\gdk-pixbuf.loaders"
Delete "$INSTDIR\etc\gtk-2.0\gtk.immodules"
Delete "$INSTDIR\etc\gtk-2.0\gtkrc"
RMDir  "$INSTDIR\etc\gtk-2.0"

Delete "$INSTDIR\etc\pango\pango.aliases"
Delete "$INSTDIR\etc\pango\pango.modules"
RMDir  "$INSTDIR\etc\pango"
RMDir  "$INSTDIR\etc"

Delete "$INSTDIR\lib\gtk-2.0\2.4.0\engines\libmetal.dll"
Delete "$INSTDIR\lib\gtk-2.0\2.4.0\engines\libpixmap.dll"
Delete "$INSTDIR\lib\gtk-2.0\2.4.0\engines\libredmond95.dll"
Delete "$INSTDIR\lib\gtk-2.0\2.4.0\engines\libwimp.dll"
Delete "$INSTDIR\lib\gtk-2.0\2.4.0\engines\libsvg.dll"
RMDir  "$INSTDIR\lib\gtk-2.0\2.4.0\engines"

Delete "$INSTDIR\lib\gtk-2.0\2.4.0\immodules\im-am-et.dll"
Delete "$INSTDIR\lib\gtk-2.0\2.4.0\immodules\im-cedilla.dll"
Delete "$INSTDIR\lib\gtk-2.0\2.4.0\immodules\im-cyrillic-translit.dll"
Delete "$INSTDIR\lib\gtk-2.0\2.4.0\immodules\im-ime.dll"
Delete "$INSTDIR\lib\gtk-2.0\2.4.0\immodules\im-inuktitut.dll"
Delete "$INSTDIR\lib\gtk-2.0\2.4.0\immodules\im-ipa.dll"
Delete "$INSTDIR\lib\gtk-2.0\2.4.0\immodules\im-thai-broken.dll"
Delete "$INSTDIR\lib\gtk-2.0\2.4.0\immodules\im-ti-er.dll"
Delete "$INSTDIR\lib\gtk-2.0\2.4.0\immodules\im-ti-et.dll"
Delete "$INSTDIR\lib\gtk-2.0\2.4.0\immodules\im-viqr.dll"
RMDir  "$INSTDIR\lib\gtk-2.0\2.4.0\immodules"

Delete "$INSTDIR\lib\gtk-2.0\2.4.0\loaders\libpixbufloader-ani.dll"
Delete "$INSTDIR\lib\gtk-2.0\2.4.0\loaders\libpixbufloader-bmp.dll"
Delete "$INSTDIR\lib\gtk-2.0\2.4.0\loaders\libpixbufloader-gif.dll"
Delete "$INSTDIR\lib\gtk-2.0\2.4.0\loaders\libpixbufloader-ico.dll"
Delete "$INSTDIR\lib\gtk-2.0\2.4.0\loaders\libpixbufloader-jpeg.dll"
Delete "$INSTDIR\lib\gtk-2.0\2.4.0\loaders\libpixbufloader-pcx.dll"
Delete "$INSTDIR\lib\gtk-2.0\2.4.0\loaders\libpixbufloader-png.dll"
Delete "$INSTDIR\lib\gtk-2.0\2.4.0\loaders\libpixbufloader-pnm.dll"
Delete "$INSTDIR\lib\gtk-2.0\2.4.0\loaders\libpixbufloader-ras.dll"
;Delete "$INSTDIR\lib\gtk-2.0\2.4.0\loaders\libpixbufloader-svg.dll"
Delete "$INSTDIR\lib\gtk-2.0\2.4.0\loaders\libpixbufloader-tga.dll"
Delete "$INSTDIR\lib\gtk-2.0\2.4.0\loaders\libpixbufloader-tiff.dll"
Delete "$INSTDIR\lib\gtk-2.0\2.4.0\loaders\libpixbufloader-wbmp.dll"
Delete "$INSTDIR\lib\gtk-2.0\2.4.0\loaders\libpixbufloader-xbm.dll"
Delete "$INSTDIR\lib\gtk-2.0\2.4.0\loaders\libpixbufloader-xpm.dll"
Delete "$INSTDIR\lib\gtk-2.0\2.4.0\loaders\svg_loader.dll"
RMDir  "$INSTDIR\lib\gtk-2.0\2.4.0\loaders"
RMDir  "$INSTDIR\lib\gtk-2.0\2.4.0"
RMDir  "$INSTDIR\lib\gtk-2.0"

Delete "$INSTDIR\lib\pango\1.5.0\modules\pango-arabic-fc.dll"
Delete "$INSTDIR\lib\pango\1.5.0\modules\pango-basic-fc.dll"
Delete "$INSTDIR\lib\pango\1.5.0\modules\pango-basic-win32.dll"
Delete "$INSTDIR\lib\pango\1.5.0\modules\pango-hangul-fc.dll"
Delete "$INSTDIR\lib\pango\1.5.0\modules\pango-hebrew-fc.dll"
Delete "$INSTDIR\lib\pango\1.5.0\modules\pango-indic-fc.dll"
Delete "$INSTDIR\lib\pango\1.5.0\modules\pango-khmer-fc.dll"
Delete "$INSTDIR\lib\pango\1.5.0\modules\pango-syriac-fc.dll"
Delete "$INSTDIR\lib\pango\1.5.0\modules\pango-thai-fc.dll"
Delete "$INSTDIR\lib\pango\1.5.0\modules\pango-tibetan-fc.dll"
RMDir  "$INSTDIR\lib\pango\1.5.0\modules"
RMDir  "$INSTDIR\lib\pango\1.5.0"
RMDir  "$INSTDIR\lib\pango"
RMDir  "$INSTDIR\lib"

RMDir "$INSTDIR\share\locale"

Delete "$INSTDIR\share\themes\Default\gtk-2.0-key\gtkrc"
RMDir  "$INSTDIR\share\themes\Default\gtk-2.0-key"
RMDir  "$INSTDIR\share\themes\Default"
RMDir  "$INSTDIR\share\themes"

Delete "$INSTDIR\share\themes\MS-Windows\gtk-2.0\gtkrc"
RMDir  "$INSTDIR\share\themes\MS-Windows\gtk-2.0"
RMDir  "$INSTDIR\share\themes\MS-Windows"
RMDir  "$INSTDIR\share\themes"
RMDir  "$INSTDIR\share"

;-------------------------------------------------------------------