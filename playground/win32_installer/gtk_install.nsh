;-------------------------------------------------------------------
SetOutPath "$INSTDIR\bin"
File "gtk\bin\asprintf.dll"
File "gtk\bin\bzip2.dll"
File "gtk\bin\charset.dll"
File "gtk\bin\fc-cache.exe"
File "gtk\bin\fc-list.exe"
File "gtk\bin\freetype6.dll"
File "gtk\bin\gdk-pixbuf-query-loaders.exe"
File "gtk\bin\gspawn-win32-helper.exe"
File "gtk\bin\gtk-query-immodules-2.0.exe"
File "gtk\bin\gtk-update-icon-cache.exe"
File "gtk\bin\gtkthemeselector.exe"
File "gtk\bin\iconv.dll"
File "gtk\bin\intl.dll"
File "gtk\bin\jpeg62.dll"
File "gtk\bin\libart_lgpl_2-2.dll"
File "gtk\bin\libatk-1.0-0.dll"
File "gtk\bin\libcairo-2.dll"
File "gtk\bin\libfontconfig-1.dll"
File "gtk\bin\libfreetype-6.dll"
File "gtk\bin\libgdk-win32-2.0-0.dll"
File "gtk\bin\libgdk_pixbuf-2.0-0.dll"
File "gtk\bin\libgdkglext-win32-1.0-0.dll"
File "gtk\bin\libglib-2.0-0.dll"
File "gtk\bin\libgmodule-2.0-0.dll"
File "gtk\bin\libgobject-2.0-0.dll"
File "gtk\bin\libgthread-2.0-0.dll"
File "gtk\bin\libgtk-win32-2.0-0.dll"
File "gtk\bin\libgtkglext-win32-1.0-0.dll"
File "gtk\bin\libpango-1.0-0.dll"
File "gtk\bin\libpangocairo-1.0-0.dll"
File "gtk\bin\libpangoft2-1.0-0.dll"
File "gtk\bin\libpangowin32-1.0-0.dll"
File "gtk\bin\libpng12.dll"
File "gtk\bin\libpng13.dll"
File "gtk\bin\libpopt-0.dll"
File "gtk\bin\libtiff3.dll"
File "gtk\bin\libxml2.dll"
File "gtk\bin\pango-querymodules.exe"
File "gtk\bin\reconfig.bat"
File "gtk\bin\xmlparse.dll"
File "gtk\bin\xmltok.dll"
File "gtk\bin\zlib1.dll"
File "gtk\bin\librsvg-2-2.dll"

SetOutPath "$INSTDIR\etc\fonts"
File "gtk\etc\fonts\fonts.conf"
File "gtk\etc\fonts\fonts.dtd"
File "gtk\etc\fonts\local.conf"

SetOutPath "$INSTDIR\etc\gtk-2.0"
File "gtk\etc\gtk-2.0\gdk-pixbuf.loaders"
File "gtk\etc\gtk-2.0\gtk.immodules"
File "gtk\etc\gtk-2.0\gtkrc"

SetOutPath "$INSTDIR\etc\pango"
File "gtk\etc\pango\pango.aliases"
File "gtk\etc\pango\pango.modules"

SetOutPath "$INSTDIR\lib\gtk-2.0\2.4.0\engines"
File "gtk\lib\gtk-2.0\2.4.0\engines\libmetal.dll"
File "gtk\lib\gtk-2.0\2.4.0\engines\libpixmap.dll"
File "gtk\lib\gtk-2.0\2.4.0\engines\libredmond95.dll"
File "gtk\lib\gtk-2.0\2.4.0\engines\libwimp.dll"
File "gtk\lib\gtk-2.0\2.4.0\engines\libsvg.dll"

SetOutPath "$INSTDIR\lib\gtk-2.0\2.4.0\immodules"
File "gtk\lib\gtk-2.0\2.4.0\immodules\im-am-et.dll"
File "gtk\lib\gtk-2.0\2.4.0\immodules\im-cedilla.dll"
File "gtk\lib\gtk-2.0\2.4.0\immodules\im-cyrillic-translit.dll"
File "gtk\lib\gtk-2.0\2.4.0\immodules\im-ime.dll"
File "gtk\lib\gtk-2.0\2.4.0\immodules\im-inuktitut.dll"
File "gtk\lib\gtk-2.0\2.4.0\immodules\im-ipa.dll"
File "gtk\lib\gtk-2.0\2.4.0\immodules\im-thai-broken.dll"
File "gtk\lib\gtk-2.0\2.4.0\immodules\im-ti-er.dll"
File "gtk\lib\gtk-2.0\2.4.0\immodules\im-ti-et.dll"
File "gtk\lib\gtk-2.0\2.4.0\immodules\im-viqr.dll"

SetOutPath "$INSTDIR\lib\gtk-2.0\2.4.0\loaders"
File "gtk\lib\gtk-2.0\2.4.0\loaders\libpixbufloader-ani.dll"
File "gtk\lib\gtk-2.0\2.4.0\loaders\libpixbufloader-bmp.dll"
File "gtk\lib\gtk-2.0\2.4.0\loaders\libpixbufloader-gif.dll"
File "gtk\lib\gtk-2.0\2.4.0\loaders\libpixbufloader-ico.dll"
File "gtk\lib\gtk-2.0\2.4.0\loaders\libpixbufloader-jpeg.dll"
File "gtk\lib\gtk-2.0\2.4.0\loaders\libpixbufloader-pcx.dll"
File "gtk\lib\gtk-2.0\2.4.0\loaders\libpixbufloader-png.dll"
File "gtk\lib\gtk-2.0\2.4.0\loaders\libpixbufloader-pnm.dll"
File "gtk\lib\gtk-2.0\2.4.0\loaders\libpixbufloader-ras.dll"
;File "gtk\lib\gtk-2.0\2.4.0\loaders\libpixbufloader-svg.dll"
File "gtk\lib\gtk-2.0\2.4.0\loaders\libpixbufloader-tga.dll"
File "gtk\lib\gtk-2.0\2.4.0\loaders\libpixbufloader-tiff.dll"
File "gtk\lib\gtk-2.0\2.4.0\loaders\libpixbufloader-wbmp.dll"
File "gtk\lib\gtk-2.0\2.4.0\loaders\libpixbufloader-xbm.dll"
File "gtk\lib\gtk-2.0\2.4.0\loaders\libpixbufloader-xpm.dll"
File "gtk\lib\gtk-2.0\2.4.0\loaders\svg_loader.dll"

SetOutPath "$INSTDIR\lib\pango\1.5.0\modules"
File "gtk\lib\pango\1.5.0\modules\pango-arabic-fc.dll"
File "gtk\lib\pango\1.5.0\modules\pango-basic-fc.dll"
File "gtk\lib\pango\1.5.0\modules\pango-basic-win32.dll"
File "gtk\lib\pango\1.5.0\modules\pango-hangul-fc.dll"
File "gtk\lib\pango\1.5.0\modules\pango-hebrew-fc.dll"
File "gtk\lib\pango\1.5.0\modules\pango-indic-fc.dll"
File "gtk\lib\pango\1.5.0\modules\pango-khmer-fc.dll"
File "gtk\lib\pango\1.5.0\modules\pango-syriac-fc.dll"
File "gtk\lib\pango\1.5.0\modules\pango-thai-fc.dll"
File "gtk\lib\pango\1.5.0\modules\pango-tibetan-fc.dll"

SetOutPath "$INSTDIR\share\locale"

SetOutPath "$INSTDIR\share\themes\Default\gtk-2.0-key"
File "gtk\share\themes\Default\gtk-2.0-key\gtkrc"

SetOutPath "$INSTDIR\share\themes\MS-Windows\gtk-2.0"
File "gtk\share\themes\MS-Windows\gtk-2.0\gtkrc"

;-------------------------------------------------------------------