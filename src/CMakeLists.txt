#Local libraries
add_executable(lep lep.cpp)
add_library(core config.cpp lepmodule.cpp lepserver.cpp ct_time.cpp util.cpp)
add_library(action action.cpp device.cpp arduino.cpp raspberry_pi.cpp speak.cpp music.cpp ct_time.cpp)
add_library(speech audio.cpp psintf.cpp speech.cpp lepclient.cpp ct_time.cpp)
add_library(gui gui.cpp lepclient.cpp ct_time.cpp)
add_library(text text.cpp lepclient.cpp ct_time.cpp)

target_link_libraries(lep core)
target_link_libraries(lep speech)
target_link_libraries(lep action)
target_link_libraries(lep gui)
target_link_libraries(lep text)

add_definitions(--std=c++11)

#ctlog
include_directories(/usr/include/)
link_directories(/usr/lib64)
target_link_libraries(lep ctlog)

#ctsockets
include_directories(/usr/include/ctsocket/)
link_directories(/usr/lib64)
target_link_libraries(lep tomcrypt)
target_link_libraries(lep ctsocket)

#pocketsphinx TODO: make platform independant
include_directories(/usr/include/pocketsphinx)
include_directories(/usr/include/sphinxbase)
link_directories(/usr/lib64)
target_link_libraries(lep pocketsphinx)
target_link_libraries(lep sphinxbase)

#futuregl TODO: make platform independant
include_directories(/usr/include/futuregl)
link_directories(/usr/lib64)
target_link_libraries(lep futuregl)

#OpenGL, GLFW, and FTGL TODO: make platform independant
include_directories(/usr/include/)
link_directories(/usr/lib64)
target_link_libraries(lep GL)
target_link_libraries(lep glfw)
target_link_libraries(lep ftgl)

#Pulseaudio
include_directories(/usr/include/)
link_directories(/usr/lib64)
target_link_libraries(lep pulse)
target_link_libraries(lep pulse-simple)
target_link_libraries(lep asound)

#Espeak
include_directories(/usr/include/)
link_directories(/usr/lib64)
target_link_libraries(lep espeak)

install(TARGETS lep DESTINATION bin)

