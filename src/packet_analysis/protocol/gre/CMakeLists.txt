
include(ZeekPlugin)

include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

zeek_plugin_begin(PacketAnalyzer GRE)
zeek_plugin_cc(GRE.cc Plugin.cc)
zeek_plugin_end()
