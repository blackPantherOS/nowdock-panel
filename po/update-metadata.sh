#!/bin/sh

BASEDIR=".." # root of translatable sources
PROJECT="plasma_applet_org.kde.store.nowdock.panel" # project name
PROJECTPATH="../../nowdockpanel" # project path
PROJECTPATHPLASMOID="../../nowdockplasmoid" # project path
BUGADDR="https://github.com/psifidotos/nowdock-panel/" # MSGID-Bugs
DEFAULTLAYOUT="../../layout-templates/org.kde.store.nowdock.defaultPanel"
EMPTYLAYOUT="../../layout-templates/org.kde.store.nowdock.emptyPanel"
WDIR="`pwd`/nowdockpanel" # working dir

cd nowdockpanel
intltool-merge --quiet --desktop-style . ../../nowdockpanel.metadata.desktop.template "${PROJECTPATH}"/metadata.desktop.cmake
intltool-merge --quiet --desktop-style . ../../defaultLayout.metadata.desktop.template "${DEFAULTLAYOUT}"/metadata.desktop.cmake
intltool-merge --quiet --desktop-style . ../../emptyLayout.metadata.desktop.template "${EMPTYLAYOUT}"/metadata.desktop.cmake

echo "metadata.desktop files for panel were updated..."

cd ../nowdockplasmoid
intltool-merge --quiet --desktop-style . ../../nowdockplasmoid.metadata.desktop.template "${PROJECTPATHPLASMOID}"/metadata.desktop.cmake

echo "metadata.desktop files for plasmoid were updated..."

