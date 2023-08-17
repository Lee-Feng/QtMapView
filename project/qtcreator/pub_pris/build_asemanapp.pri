# build a Qt app
QT += quick
CONFIG += c++11

# project path defInition
include(path.pri)

win32 {
TEMPLATE = app
# dest dir for apps
DESTDIR = $$PTVAR_BIN
}

include($$PTVAR_SRC/ios/ios.pri)
include($$PTVAR_SRC/android/android.pri)
exists ($$PTVAR_SRC/qml/imports): QML_IMPORT_PATH += $$SRC_DIR/qml/imports

#android项目不能指定目录
!android {
DESTDIR = $$PTVAR_BIN
}
