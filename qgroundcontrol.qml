/*
    imports.qml
 
    Declaration of QML Imports
    required by project.
 
    This is necessary if we want
    to keep qml files in a folder
    separate from .pro file because
    of the way qmlimportscanner works.
    If these imports are not declared,
    qmake will not recognize them,
    and QtQuick will not be packaged
    with statically built apps (i.e. iOS)
    and imported at runtime.
 
    This must be kept in the same
    directory as your .pro file
*/
 
import QtQuick 2.1
import QtQuick.Window 2.1
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import QtQuick.LocalStorage 2.0
//import etc, etc, etc.
 
QtObject {}
