#-------------------------------------------------
#
# Project created by QtCreator 2026-01-27T10:02:58
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TEMPLATE = app

VERSION_MAJ = 1
VERSION_MID = 0
VERSION_MIN = 0
TARGET_NAME = TdipProc
VERSION=$${VERSION_MAJ}.$${VERSION_MID}.$${VERSION_MIN}
VER_MAJ=$$VERSION_MAJ
VER_MIN=$$VERSION_MID
VER_PAT=$$VERSION_MIN
DEFINES += "VERSION_MAJ="$${VERSION_MAJ}
DEFINES += "VERSION_MID="$${VERSION_MID}
DEFINES += "VERSION_MIN="$${VERSION_MIN}

TARGET = $${TARGET_NAME}-V$${VERSION_MAJ}.$${VERSION_MID}.$${VERSION_MIN}

!include($$PWD/Common.pri) {
    error("Error: Can't find the Common.pri file!")
}

DESTDIR += $$PWD/bin

INCLUDEPATH += $$PWD/format \

SOURCES += main.cpp\
        wdt_main.cpp \
    wdt_out_put.cpp \
    format/format_txt_shifting.cpp \
    model_range_limit.cpp \
    format/tdip_data_out.cpp \
    format/format_txt_coord.cpp \
    format/format_txt_elev.cpp \
    format/format_txt_site_coord.cpp \
    wdt_param_trs.cpp \
    wdt_tdip_res_table.cpp \
    wdt_tdip_res_space.cpp \
    wdt_tdip_res.cpp \
    dlg_limit_settings.cpp \
    model_tdip_res.cpp \
    format/shared_data.cpp \
    wdt_ts_plot.cpp \
    wdt_monitor_ch.cpp \
    plot_monitor.cpp

HEADERS  += wdt_main.h \
    wdt_out_put.h \
    format/format_txt_shifting.h \
    format/tdip_def.h \
    model_range_limit.h \
    format/tdip_data_out.h \
    format/format_txt_coord.h \
    format/format_txt_elev.h \
    format/format_txt_site_coord.h \
    wdt_param_trs.h \
    wdt_tdip_res_table.h \
    wdt_tdip_res_space.h \
    wdt_tdip_res.h \
    dlg_limit_settings.h \
    model_tdip_res.h \
    format/shared_data.h \
    wdt_ts_plot.h \
    wdt_monitor_ch.h \
    plot_monitor.h

FORMS    += wdt_main.ui \
    wdt_out_put.ui \
    wdt_param_trs.ui \
    wdt_tdip_res_table.ui \
    wdt_tdip_res_space.ui \
    wdt_tdip_res.ui \
    dlg_limit_settings.ui \
    wdt_ts_plot.ui

OTHER_FILES +=
