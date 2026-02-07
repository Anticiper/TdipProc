#-------------------------------------------------
#   Libs
#-------------------------------------------------

CONFIG += c++11

qtHaveModule(printsupport): QT += printsupport  #this is for qcustomplot

#this is for sockect api in cutils
win32:{
    LIBS += libwsock32 libWs2_32 libiphlpapi -L$$PWD/Common/cutils/libevent/lib_win32 libevent_core
}
linux-g++:{
    LIBS += -L$$PWD/Common/cutils/libevent/lib_linux_x86
    LIBS += -levent_core -levent_pthreads
    LIBS += -ldl -lpthread
}
linux-arm-gnueabi-g++:{
    LIBS += -L$$PWD/Common/cutils/libevent/lib_arm_4.4.3
    LIBS += -levent_core -levent_pthreads
    LIBS += -ldl -lpthread
}

INCLUDEPATH += $$PWD/Common
INCLUDEPATH += $$PWD/Common/algo
INCLUDEPATH += $$PWD/Common/cutils/c
INCLUDEPATH += $$PWD/Common/cutils/cpp
INCLUDEPATH += $$PWD/Common/cutils/libevent/include
INCLUDEPATH += $$PWD/Common/ctrl/sip
INCLUDEPATH += $$PWD/Common/deal/geo
INCLUDEPATH += $$PWD/Common/deal/sip
INCLUDEPATH += $$PWD/Common/deal/csamt
INCLUDEPATH += $$PWD/Common/format
INCLUDEPATH += $$PWD/Common/pub/sip
INCLUDEPATH += $$PWD/Common/pub/tdip
INCLUDEPATH += $$PWD/Common/pub/geo
INCLUDEPATH += $$PWD/Common/pub/csamt
INCLUDEPATH += $$PWD/Common/qt_model/sip
INCLUDEPATH += $$PWD/Common/qt_model/tdip
INCLUDEPATH += $$PWD/Common/qt_model/pub
INCLUDEPATH += $$PWD/Common/qt_pub
INCLUDEPATH += $$PWD/Common/qt_view

HEADERS += \
    $$PWD/Common/common_ver.h \
    $$PWD/Common/algo/_kiss_fft_guts_double.h \
    $$PWD/Common/algo/_kiss_fft_guts.h \
    $$PWD/Common/algo/kiss_fft_double.h \
    $$PWD/Common/algo/kiss_fft.h \
    $$PWD/Common/algo/algo_geo.h \
    $$PWD/Common/algo/algo_pub.h \
    $$PWD/Common/algo/complex.h \
    $$PWD/Common/algo/Functions.h \
    $$PWD/Common/algo/MathBasicDefs.h \
    $$PWD/Common/algo/win_function.h \
#    $$PWD/Common/cutils/c/alarm_perf_agent.h \
#    $$PWD/Common/cutils/c/alarm_perf_center.h \
#    $$PWD/Common/cutils/c/alarm_perf_code.h \
#    $$PWD/Common/cutils/c/alarm_perf_pub.h \
    $$PWD/Common/cutils/c/ccli.h \
    $$PWD/Common/cutils/c/cdefine.h \
    $$PWD/Common/cutils/c/chash.h \
    $$PWD/Common/cutils/c/chash_int.h \
    $$PWD/Common/cutils/c/chash_str.h \
    $$PWD/Common/cutils/c/chash_str_str.h \
    $$PWD/Common/cutils/c/check_sum.h \
    $$PWD/Common/cutils/c/cini.h \
    $$PWD/Common/cutils/c/cled.h \
    $$PWD/Common/cutils/c/clist.h \
    $$PWD/Common/cutils/c/clog.h \
    $$PWD/Common/cutils/c/cmd_head.h \
    $$PWD/Common/cutils/c/cmd5.h \
    $$PWD/Common/cutils/c/cobj.h \
    $$PWD/Common/cutils/c/cobj_addr.h \
    $$PWD/Common/cutils/c/cobj_alarm.h \
    $$PWD/Common/cutils/c/cobj_alarmagent.h \
    $$PWD/Common/cutils/c/cobj_checkpoint.h \
    $$PWD/Common/cutils/c/cobj_cli.h \
    $$PWD/Common/cutils/c/cobj_cmd_recv.h \
    $$PWD/Common/cutils/c/cobj_cmd_req.h \
    $$PWD/Common/cutils/c/cobj_cmd_routine.h \
    $$PWD/Common/cutils/c/cobj_cstr.h \
    $$PWD/Common/cutils/c/cobj_dev_addr.h \
    $$PWD/Common/cutils/c/cobj_hash_item.h \
    $$PWD/Common/cutils/c/cobj_int.h \
    $$PWD/Common/cutils/c/cobj_ledevent.h \
    $$PWD/Common/cutils/c/cobj_ledstyle.h \
    $$PWD/Common/cutils/c/cobj_perf.h \
    $$PWD/Common/cutils/c/cobj_perfagent.h \
    $$PWD/Common/cutils/c/cobj_str.h \
    $$PWD/Common/cutils/c/cobj_thread.h \
    $$PWD/Common/cutils/c/cobj_tlv.h \
    $$PWD/Common/cutils/c/command.h \
    $$PWD/Common/cutils/c/csem.h \
    $$PWD/Common/cutils/c/cserial.h \
    $$PWD/Common/cutils/c/cstring.h \
    $$PWD/Common/cutils/c/cthread.h \
    $$PWD/Common/cutils/c/cutils.h \
    $$PWD/Common/cutils/c/cvector.h \
    $$PWD/Common/cutils/c/cversion.h \
    $$PWD/Common/cutils/c/dev_addr.h \
    $$PWD/Common/cutils/c/dev_addr_sock.h \
    $$PWD/Common/cutils/c/ex_assert.h \
    $$PWD/Common/cutils/c/ex_errno.h \
    $$PWD/Common/cutils/c/ex_file.h \
    $$PWD/Common/cutils/c/ex_memory.h \
    $$PWD/Common/cutils/c/ex_socket.h \
    $$PWD/Common/cutils/c/ex_string.h \
    $$PWD/Common/cutils/c/ex_time.h \
    $$PWD/Common/cutils/c/git_version.h \
#    $$PWD/Common/cutils/c/socket_broadcast.h \
#    $$PWD/Common/cutils/c/socket_libevent.h \
#    $$PWD/Common/cutils/c/sys_info.h \
    $$PWD/Common/cutils/c/tlv.h \
    $$PWD/Common/cutils/c/utarray.h \
    $$PWD/Common/cutils/cpp/ex_code.h \
    $$PWD/Common/deal/geo/ad_data.h \
    $$PWD/Common/deal/geo/pub_define.h \
    $$PWD/Common/deal/geo/result.h \
    $$PWD/Common/deal/geo/spect_array.h \
    $$PWD/Common/deal/geo/tdip_result.h \
    $$PWD/Common/deal/geo/tdip_result_cal.h \
    $$PWD/Common/deal/sip/result_sip.h \
    $$PWD/Common/deal/sip/sip_spect2res.h \
    $$PWD/Common/deal/sip/sip_ts2spect.h \
    $$PWD/Common/deal/geo/data_limit.h \
#    $$PWD/Common/deal/csamt/csamt_res.h \
    $$PWD/Common/format/format.h \
#    $$PWD/Common/format/format_cmt.h \
#    $$PWD/Common/format/format_crs.h \
    $$PWD/Common/format/format_smt.h \
    $$PWD/Common/format/format_srs.h \
    $$PWD/Common/format/format_trs.h \
    $$PWD/Common/format/format_txt.h \
    $$PWD/Common/pub/sip/sip_pub.h \
    $$PWD/Common/pub/tdip/tdip_pub.h \
#    $$PWD/Common/pub/csamt/csamt_pub.h \
    $$PWD/Common/pub/geo/geo_define.h \
    $$PWD/Common/qt_model/pub/item_cmb.h \
    $$PWD/Common/qt_model/pub/item_color.h \
    $$PWD/Common/qt_model/sip/model_sip_dev_lay.h \
    $$PWD/Common/qt_model/tdip/model_tdip_dev_lay.h \
#    $$PWD/Common/qt_pub/qt_config.h \
#    $$PWD/Common/qt_pub/qt_error.h \
    $$PWD/Common/qt_pub/qt_pub.h \
    $$PWD/Common/qt_view/custom_plot.h \
    $$PWD/Common/qt_view/dlg_ok_cancel.h \
    $$PWD/Common/qt_view/dlg_plot_range.h \
    $$PWD/Common/qt_view/qcustomplot.h \
    $$PWD/Common/qt_view/ui_cursor.h \
    $$PWD/Common/qt_view/ui_pub.h \
    $$PWD/Common/deal/geo/spect_algo.h \
    $$PWD/Common/qt_view/wdt_gps_pos.h \
    $$PWD/Common/qt_view/item.h \
    $$PWD/Common/qt_view/item_gps_pos.h \
    $$PWD/Common/qt_model/pub/model_st_pos.h \
    $$PWD/Common/deal/geo/data_threshold.h \
    $$PWD/Common/format/format_tmt.h

SOURCES += \
    $$PWD/Common/algo/kiss_fft_double.c \
    $$PWD/Common/algo/kiss_fft.c \
    $$PWD/Common/algo/algo_pub.c \
    $$PWD/Common/algo/algo_geo.c \
    $$PWD/Common/algo/Functions.cc \
#    $$PWD/Common/cutils/c/alarm_perf_agent.c \
#    $$PWD/Common/cutils/c/alarm_perf_pub.c \
    $$PWD/Common/cutils/c/ccli.c \
    $$PWD/Common/cutils/c/chash.c \
    $$PWD/Common/cutils/c/chash_int.c \
    $$PWD/Common/cutils/c/chash_str.c \
    $$PWD/Common/cutils/c/chash_str_str.c \
    $$PWD/Common/cutils/c/check_sum.c \
    $$PWD/Common/cutils/c/cini.c \
    $$PWD/Common/cutils/c/cled.c \
    $$PWD/Common/cutils/c/clist.c \
    $$PWD/Common/cutils/c/clog.c \
    $$PWD/Common/cutils/c/cmd_head.c \
    $$PWD/Common/cutils/c/cmd5.c \
    $$PWD/Common/cutils/c/cobj.c \
    $$PWD/Common/cutils/c/cobj_addr.c \
    $$PWD/Common/cutils/c/cobj_alarm.c \
    $$PWD/Common/cutils/c/cobj_alarmagent.c \
    $$PWD/Common/cutils/c/cobj_checkpoint.c \
    $$PWD/Common/cutils/c/cobj_cli.c \
    $$PWD/Common/cutils/c/cobj_cmd_recv.c \
    $$PWD/Common/cutils/c/cobj_cmd_req.c \
    $$PWD/Common/cutils/c/cobj_cmd_routine.c \
    $$PWD/Common/cutils/c/cobj_cstr.c \
    $$PWD/Common/cutils/c/cobj_dev_addr.c \
    $$PWD/Common/cutils/c/cobj_hash_item.c \
    $$PWD/Common/cutils/c/cobj_int.c \
    $$PWD/Common/cutils/c/cobj_ledevent.c \
    $$PWD/Common/cutils/c/cobj_ledstyle.c \
    $$PWD/Common/cutils/c/cobj_perf.c \
    $$PWD/Common/cutils/c/cobj_perfagent.c \
    $$PWD/Common/cutils/c/cobj_str.c \
    $$PWD/Common/cutils/c/cobj_thread.c \
    $$PWD/Common/cutils/c/cobj_tlv.c \
    $$PWD/Common/cutils/c/command.c \
    $$PWD/Common/cutils/c/csem_unix.c\
    $$PWD/Common/cutils/c/csem_win.c\
    $$PWD/Common/cutils/c/cserial.c \
    $$PWD/Common/cutils/c/cstring.c \
    $$PWD/Common/cutils/c/cthread.c \
    $$PWD/Common/cutils/c/cutils.c \
    $$PWD/Common/cutils/c/cvector.c \
    $$PWD/Common/cutils/c/cversion.c \
    $$PWD/Common/cutils/c/dev_addr.c \
    $$PWD/Common/cutils/c/dev_addr_sock.c \
    $$PWD/Common/cutils/c/ex_assert.c \
    $$PWD/Common/cutils/c/ex_errno.c \
    $$PWD/Common/cutils/c/ex_file.c \
    $$PWD/Common/cutils/c/ex_memory.c \
    $$PWD/Common/cutils/c/ex_socket.c \
    $$PWD/Common/cutils/c/ex_string.c \
    $$PWD/Common/cutils/c/ex_time.c \
    $$PWD/Common/cutils/c/socket_broadcast.c \
    $$PWD/Common/cutils/c/socket_libevent.c \
    $$PWD/Common/cutils/c/sys_info.c \
    $$PWD/Common/cutils/c/tlv.c \
    $$PWD/Common/cutils/cpp/ex_code.cpp \
    $$PWD/Common/deal/geo/ad_data.c \
    $$PWD/Common/deal/geo/pub_define.c \
    $$PWD/Common/deal/geo/result.cpp \
    $$PWD/Common/deal/geo/spect_array.cpp \
    $$PWD/Common/deal/geo/tdip_resul.cpp \
    $$PWD/Common/deal/geo/tdip_result_cal.cpp \
    $$PWD/Common/deal/sip/result_sip.cpp \
    $$PWD/Common/deal/sip/sip_spect2res.cpp \
    $$PWD/Common/deal/sip/sip_ts2spect.cpp \
    $$PWD/Common/deal/geo/data_limit.c \
#    $$PWD/Common/deal/csamt/csamt_res.cpp \
    $$PWD/Common/format/format.cpp \
#    $$PWD/Common/format/format_cmt.cpp \
#    $$PWD/Common/format/format_crs.cpp \
    $$PWD/Common/format/format_smt.cpp \
    $$PWD/Common/format/format_srs.cpp \
    $$PWD/Common/format/format_trs.cpp \
    $$PWD/Common/format/format_txt.cpp \
    $$PWD/Common/pub/sip/sip_pub.c \
    $$PWD/Common/pub/tdip/tdip_pub.c \
#    $$PWD/Common/pub/csamt/csamt_pub.c \
    $$PWD/Common/qt_model/pub/item_cmb.cpp \
    $$PWD/Common/qt_model/pub/item_color.cpp \
    $$PWD/Common/qt_model/sip/model_sip_dev_lay.cpp \
    $$PWD/Common/qt_model/tdip/model_tdip_dev_lay.cpp \
#    $$PWD/Common/qt_pub/qt_config.cpp \
#    $$PWD/Common/qt_pub/qt_error.cpp \
    $$PWD/Common/qt_pub/qt_pub.cpp \
    $$PWD/Common/qt_view/custom_plot.cpp \
    $$PWD/Common/qt_view/dlg_ok_cancel.cpp \
    $$PWD/Common/qt_view/dlg_plot_range.cpp \
    $$PWD/Common/qt_view/qcustomplot.cpp \
    $$PWD/Common/qt_view/ui_cursor.cpp \
    $$PWD/Common/qt_view/ui_pub.cpp \
    $$PWD/Common/deal/geo/spect_algo.cpp \
    $$PWD/Common/qt_view/wdt_gps_pos.cpp \
    $$PWD/Common/qt_view/item.cpp \
    $$PWD/Common/qt_view/item_gps_pos.cpp \
    $$PWD/Common/qt_model/pub/model_st_pos.cpp \
    $$PWD/Common/deal/geo/data_threshold.cpp \
    $$PWD/Common/format/format_tmt.cpp

