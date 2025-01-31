cmake_minimum_required(VERSION 3.22)
project(JuceHello VERSION 1.0.0)

find_package(JUCE CONFIG REQUIRED)

juce_add_gui_app(JuceHello
    PRODUCT_NAME "JuceHello")

juce_generate_juce_header(JuceHello)

target_sources(JuceHello
    PRIVATE
    main.cpp
    MainComponent.cpp
    AudioRecorder.cpp
    MyProcessor.cpp
    AnalyserComponent.cpp)



target_compile_definitions(JuceHello
    PRIVATE
        JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_gui_app` call
        JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_gui_app` call
        JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:JuceHello,JUCE_PRODUCT_NAME>"
        JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:JuceHello,JUCE_VERSION>")


target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
                           "${PROJECT_BINARY_DIR}"
                           "${JUCE_CMAKE_UTILS_DIR}")

target_link_libraries(${CMAKE_PROJECT_NAME}
    PRIVATE
        juce::juce_gui_basics
        juce::juce_gui_extra
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags) 

