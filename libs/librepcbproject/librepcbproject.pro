#-------------------------------------------------
#
# Project created by QtCreator 2015-05-31T12:53:17
#
#-------------------------------------------------

TEMPLATE = lib
TARGET = librepcbproject

# Set the path for the generated binary
GENERATED_DIR = ../../generated

# Use common project definitions
include(../../common.pri)

QT += core widgets xml sql printsupport

CONFIG += staticlib

INCLUDEPATH += \
    ../

SOURCES += \
    project.cpp \
    circuit/circuit.cpp \
    circuit/netclass.cpp \
    circuit/netsignal.cpp \
    circuit/cmd/cmdnetclassadd.cpp \
    circuit/cmd/cmdnetclassremove.cpp \
    circuit/cmd/cmdnetsignaladd.cpp \
    circuit/cmd/cmdnetsignalremove.cpp \
    circuit/cmd/cmdnetclassedit.cpp \
    circuit/cmd/cmdnetsignaledit.cpp \
    schematics/schematic.cpp \
    schematics/cmd/cmdschematicadd.cpp \
    schematics/cmd/cmdschematicremove.cpp \
    schematics/cmd/cmdsymbolinstanceadd.cpp \
    schematics/cmd/cmdsymbolinstanceremove.cpp \
    schematics/cmd/cmdschematicnetlabeladd.cpp \
    schematics/cmd/cmdschematicnetlabelremove.cpp \
    schematics/cmd/cmdschematicnetlabeledit.cpp \
    schematics/cmd/cmdschematicnetpointedit.cpp \
    schematics/cmd/cmdsymbolinstanceedit.cpp \
    schematics/items/si_base.cpp \
    schematics/items/si_netlabel.cpp \
    schematics/items/si_netline.cpp \
    schematics/items/si_netpoint.cpp \
    schematics/items/si_symbol.cpp \
    schematics/items/si_symbolpin.cpp \
    schematics/graphicsitems/sgi_base.cpp \
    schematics/graphicsitems/sgi_netlabel.cpp \
    schematics/graphicsitems/sgi_netline.cpp \
    schematics/graphicsitems/sgi_netpoint.cpp \
    schematics/graphicsitems/sgi_symbol.cpp \
    schematics/graphicsitems/sgi_symbolpin.cpp \
    boards/board.cpp \
    boards/cmd/cmdboardadd.cpp \
    boards/items/bi_base.cpp \
    boards/items/bi_footprint.cpp \
    boards/graphicsitems/bgi_base.cpp \
    boards/graphicsitems/bgi_footprint.cpp \
    boards/items/bi_footprintpad.cpp \
    boards/graphicsitems/bgi_footprintpad.cpp \
    library/projectlibrary.cpp \
    erc/ercmsg.cpp \
    erc/ercmsglist.cpp \
    cmd/cmdprojectsetmetadata.cpp \
    settings/projectsettings.cpp \
    settings/cmd/cmdprojectsettingschange.cpp \
    schematics/schematiclayerprovider.cpp \
    library/cmd/cmdprojectlibraryaddelement.cpp \
    boards/items/bi_device.cpp \
    boards/cmd/cmddeviceinstanceadd.cpp \
    boards/cmd/cmddeviceinstanceedit.cpp \
    boards/cmd/cmddeviceinstanceremove.cpp \
    circuit/componentinstance.cpp \
    circuit/componentattributeinstance.cpp \
    circuit/componentsignalinstance.cpp \
    circuit/cmd/cmdcompattrinstadd.cpp \
    circuit/cmd/cmdcompattrinstedit.cpp \
    circuit/cmd/cmdcompattrinstremove.cpp \
    circuit/cmd/cmdcomponentinstanceadd.cpp \
    circuit/cmd/cmdcomponentinstanceedit.cpp \
    circuit/cmd/cmdcomponentinstanceremove.cpp \
    circuit/cmd/cmdcompsiginstsetnetsignal.cpp \
    boards/items/bi_polygon.cpp \
    boards/graphicsitems/bgi_polygon.cpp \
    boards/boardlayerstack.cpp \
    boards/items/bi_netpoint.cpp \
    boards/items/bi_netline.cpp \
    boards/graphicsitems/bgi_netpoint.cpp \
    boards/graphicsitems/bgi_netline.cpp \
    boards/cmd/cmdboardnetlineadd.cpp \
    boards/cmd/cmdboardnetlineremove.cpp \
    boards/cmd/cmdboardnetpointadd.cpp \
    boards/cmd/cmdboardnetpointedit.cpp \
    boards/cmd/cmdboardnetpointremove.cpp \
    boards/items/bi_via.cpp \
    boards/graphicsitems/bgi_via.cpp \
    boards/cmd/cmdboardviaadd.cpp \
    boards/cmd/cmdboardviaremove.cpp \
    boards/cmd/cmdboardviaedit.cpp \
    boards/cmd/cmdboarddesignrulesmodify.cpp \
    boards/boardgerberexport.cpp \
    schematics/items/si_netsegment.cpp \
    schematics/cmd/cmdschematicnetsegmentadd.cpp \
    schematics/cmd/cmdschematicnetsegmentremove.cpp \
    schematics/cmd/cmdschematicnetsegmentedit.cpp \
    schematics/cmd/cmdschematicnetsegmentaddelements.cpp \
    schematics/cmd/cmdschematicnetsegmentremoveelements.cpp \
    schematics/schematicselectionquery.cpp

HEADERS += \
    project.h \
    circuit/circuit.h \
    circuit/netclass.h \
    circuit/netsignal.h \
    circuit/cmd/cmdnetclassadd.h \
    circuit/cmd/cmdnetclassremove.h \
    circuit/cmd/cmdnetsignaladd.h \
    circuit/cmd/cmdnetsignalremove.h \
    circuit/cmd/cmdnetclassedit.h \
    circuit/cmd/cmdnetsignaledit.h \
    schematics/schematic.h \
    schematics/cmd/cmdschematicadd.h \
    schematics/cmd/cmdschematicremove.h \
    schematics/cmd/cmdsymbolinstanceadd.h \
    schematics/cmd/cmdsymbolinstanceremove.h \
    schematics/cmd/cmdschematicnetlabeladd.h \
    schematics/cmd/cmdschematicnetlabelremove.h \
    schematics/cmd/cmdschematicnetlabeledit.h \
    schematics/cmd/cmdschematicnetpointedit.h \
    schematics/cmd/cmdsymbolinstanceedit.h \
    schematics/items/si_base.h \
    schematics/items/si_netlabel.h \
    schematics/items/si_netline.h \
    schematics/items/si_netpoint.h \
    schematics/items/si_symbol.h \
    schematics/items/si_symbolpin.h \
    schematics/graphicsitems/sgi_base.h \
    schematics/graphicsitems/sgi_netlabel.h \
    schematics/graphicsitems/sgi_netline.h \
    schematics/graphicsitems/sgi_netpoint.h \
    schematics/graphicsitems/sgi_symbol.h \
    schematics/graphicsitems/sgi_symbolpin.h \
    boards/board.h \
    boards/cmd/cmdboardadd.h \
    boards/items/bi_base.h \
    boards/items/bi_footprint.h \
    boards/graphicsitems/bgi_base.h \
    boards/graphicsitems/bgi_footprint.h \
    boards/items/bi_footprintpad.h \
    boards/graphicsitems/bgi_footprintpad.h \
    library/projectlibrary.h \
    erc/ercmsg.h \
    erc/ercmsglist.h \
    erc/if_ercmsgprovider.h \
    cmd/cmdprojectsetmetadata.h \
    settings/projectsettings.h \
    settings/cmd/cmdprojectsettingschange.h \
    schematics/schematiclayerprovider.h \
    library/cmd/cmdprojectlibraryaddelement.h \
    boards/items/bi_device.h \
    boards/cmd/cmddeviceinstanceadd.h \
    boards/cmd/cmddeviceinstanceremove.h \
    boards/cmd/cmddeviceinstanceedit.h \
    circuit/componentinstance.h \
    circuit/componentattributeinstance.h \
    circuit/componentsignalinstance.h \
    circuit/cmd/cmdcompattrinstadd.h \
    circuit/cmd/cmdcompattrinstedit.h \
    circuit/cmd/cmdcompattrinstremove.h \
    circuit/cmd/cmdcomponentinstanceadd.h \
    circuit/cmd/cmdcomponentinstanceedit.h \
    circuit/cmd/cmdcomponentinstanceremove.h \
    circuit/cmd/cmdcompsiginstsetnetsignal.h \
    boards/items/bi_polygon.h \
    boards/graphicsitems/bgi_polygon.h \
    boards/boardlayerstack.h \
    boards/items/bi_netpoint.h \
    boards/items/bi_netline.h \
    boards/graphicsitems/bgi_netpoint.h \
    boards/graphicsitems/bgi_netline.h \
    boards/cmd/cmdboardnetlineadd.h \
    boards/cmd/cmdboardnetlineremove.h \
    boards/cmd/cmdboardnetpointadd.h \
    boards/cmd/cmdboardnetpointedit.h \
    boards/cmd/cmdboardnetpointremove.h \
    boards/items/bi_via.h \
    boards/graphicsitems/bgi_via.h \
    boards/cmd/cmdboardviaadd.h \
    boards/cmd/cmdboardviaremove.h \
    boards/cmd/cmdboardviaedit.h \
    boards/cmd/cmdboarddesignrulesmodify.h \
    boards/boardgerberexport.h \
    schematics/items/si_netsegment.h \
    schematics/cmd/cmdschematicnetsegmentadd.h \
    schematics/cmd/cmdschematicnetsegmentremove.h \
    schematics/cmd/cmdschematicnetsegmentedit.h \
    schematics/cmd/cmdschematicnetsegmentaddelements.h \
    schematics/cmd/cmdschematicnetsegmentremoveelements.h \
    schematics/schematicselectionquery.h

FORMS +=
