include_directories(${PODCASTDL_SOURCE_DIR}/src /usr/include /usr/local/include)
link_directories(${PODCASTDL_SOURCE_DIR}/src /usr/lib /usr/local/lib)
add_definitions( -DPODCASTDL_VERSION_STRING=\"0.3.0\" )


add_executable(podcastdl podcastdl.c podcast.c util.c)
target_link_libraries(podcastdl mrss)


install(TARGETS podcastdl DESTINATION bin)



