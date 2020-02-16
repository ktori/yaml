target_sources(yaml
        PUBLIC
        yaml.h
        callbacks.h
        PRIVATE
        yaml.c
        yaml_impl.h
        )

add_subdirectory(document)