project(yaml-test)

add_executable(${PROJECT_NAME} test.c)
target_link_libraries(${PROJECT_NAME} yaml)

add_executable(yaml-test-doc test_doc.c)
target_link_libraries(yaml-test-doc yaml)