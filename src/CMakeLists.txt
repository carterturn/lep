# Local libraries
add_library(core config.cpp lepmodule.cpp util.cpp ct_time.cpp)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
configure_file("compile_config.h.in" "compile_config.h")

add_definitions(--std=c++11)

# ctsockets
include_directories(/usr/include/ctsocket/)
link_directories(/usr/lib64)

# Server
if(SERVER)
  add_executable(server lepserver.cpp)
  target_link_libraries(server core)
  if(SERVER_DEVICES)
    add_library(device action.cpp device.cpp arduino.cpp raspberry_pi.cpp ct_time.cpp)
    target_link_libraries(server device)
  endif(SERVER_DEVICES)
  if(SERVER_AUDIO)
    add_library(audio_out speak.cpp music.cpp)
    target_link_libraries(server audio_out)
    # Espeak
    include_directories(/usr/include/)
    link_directories(/usr/lib64)
    target_link_libraries(server espeak)
  endif(SERVER_AUDIO)
  target_link_libraries(server ctsocket)
  install(TARGETS server DESTINATION ${PROJECT_SOURCE_DIR})
endif(SERVER)

# Clients
if(SPEECH OR GUI OR TEXT)
  add_library(client lepmodule.cpp lepclient.cpp ct_time.cpp util.cpp)
endif(SPEECH OR GUI OR TEXT)

# Speech
if(SPEECH)
  add_library(speech_lib audio.cpp psintf.cpp)
  add_executable(speech speech.cpp)
  target_link_libraries(speech core)
  target_link_libraries(speech client)
  target_link_libraries(speech speech_lib)
  # pocketsphinx TODO: make platform independant
  include_directories(/usr/include/sphinxbase)
  include_directories(/usr/include/pocketsphinx)
  link_directories(/usr/lib64)
  target_link_libraries(speech pulse)
  target_link_libraries(speech pulse-simple)
  target_link_libraries(speech pocketsphinx)
  target_link_libraries(speech sphinxbase)
  target_link_libraries(speech ctsocket)
  install(TARGETS speech DESTINATION ${PROJECT_SOURCE_DIR})
endif(SPEECH)

# GUI
if(GUI)
  add_executable(gui gui.cpp)
  target_link_libraries(gui core)
  target_link_libraries(gui client)
  # futuregl TODO: make platform independant
  include_directories(/usr/include/futuregl)
  link_directories(/usr/lib64)
  target_link_libraries(gui futuregl)
  # OpenGL, GLFW, and FTGL TODO: make platform independant
  include_directories(/usr/include/)
  link_directories(/usr/lib64)
  target_link_libraries(gui GL)
  target_link_libraries(gui glfw)
  target_link_libraries(gui ftgl)
  target_link_libraries(gui ctsocket)
  install(TARGETS gui DESTINATION ${PROJECT_SOURCE_DIR})
endif(GUI)

# Text
if(TEXT)
  add_executable(text text.cpp)
  target_link_libraries(text core)
  target_link_libraries(text client)
  target_link_libraries(text ctsocket)
  install(TARGETS text DESTINATION ${PROJECT_SOURCE_DIR})
endif(TEXT)
