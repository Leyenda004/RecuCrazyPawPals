file(
    COPY ${V_PROJECT_DIRECTORY}/crazypawpals/resources
    DESTINATION ${V_EXECUTABLE_DIRECTORY}
)

file(
    COPY ${V_PROJECT_DIRECTORY}/crazypawpals/bin/
    DESTINATION ${V_EXECUTABLE_DIRECTORY}
    PATTERN "*.dll"
)