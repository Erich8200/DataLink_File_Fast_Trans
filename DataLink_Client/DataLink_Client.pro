QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Packet/file_data_packet.cpp \
    Packet/file_info_packet.cpp \
    Packet/file_packet.cpp \
    Packet/hello_packet.cpp \
    SessionStrategy/on_hello_strategy.cpp \
    StrategyFactory/on_hello_strategy_factory.cpp \
    main.cpp \
    mainwidget.cpp \
    my_global.cpp

HEADERS += \
    Packet/file_data_packet.h \
    Packet/file_info_packet.h \
    Packet/file_packet.h \
    Packet/hello_packet.h \
    SessionStrategy/on_hello_strategy.h \
    StrategyFactory/on_hello_strategy_factory.h \
    mainwidget.h \
    my_global.h

INCLUDEPATH += \
    ../../AsyncClientLib \
    $$(CPP_LIB_DIR)/boost_1_70_0 \
    $$(CPP_LIB_DIR)/cryptopp/include

FORMS += \
    mainwidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32-msvc*: {
    QMAKE_CFLAGS *= /utf-8
    QMAKE_CXXFLAGS *= /utf-8
}

PLATFORM = win64

DEFINES += _WIN32_WINNT=0x0A00 # Boost configuration

contains(PLATFORM, win64) {
#    win32: DEPENDPATH += ../AsyncServer_VS/x64
    win32:CONFIG(debug, debug|release): {
        LIBS += -L../../AsyncClientLib/x64/Debug -lAsyncClientLib
        LIBS += -L$$(CPP_LIB_DIR)/boost_1_70_0/lib64-msvc-14.1/ -lboost_date_time-vc141-mt-gd-x64-1_70
        LIBS += -L$$(CPP_LIB_DIR)/cryptopp/x64/Output/Debug -lcryptlib
    }
    else:win32:CONFIG(release, debug|release): {
        LIBS += -L../../AsyncClientLib/x64/Release -lAsyncClientLib
        LIBS += -L$$(CPP_LIB_DIR)/boost_1_70_0/lib64-msvc-14.1/ -lboost_date_time-vc141-mt-x64-1_70
        LIBS += -L$$(CPP_LIB_DIR)/cryptopp/x64/Output/Release -lcryptlib
    }
} else {
    win32:CONFIG(debug, debug|release): {
        LIBS += -L../../AsyncClientLib/Debug -lAsyncClientLib
        LIBS += -L$$(CPP_LIB_DIR)/boost_1_70_0/lib32-msvc-14.1/ -lboost_date_time-vc141-mt-gd-x32-1_70
        LIBS += -L$$(CPP_LIB_DIR)/cryptopp/Win32/Output/Debug -lcryptlib
    }
    else:win32:CONFIG(release, debug|release): {
        LIBS += -L../../AsyncClientLib/Release -lAsyncClientLib
        LIBS += -L$$(CPP_LIB_DIR)/boost_1_70_0/lib32-msvc-14.1/ -lboost_date_time-vc141-mt-x32-1_70
        LIBS += -L$$(CPP_LIB_DIR)/cryptopp/Win32/Output/Release -lcryptlib
    }
}

RC_ICONS = icons/upload-64px.ico
