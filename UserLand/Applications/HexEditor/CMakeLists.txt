set(SOURCES
    HexEditor.cpp
    HexEditorWidget.cpp
    FindDialog.cpp
    main.cpp
)

pranaos_app(HexEditor ICON app-hex-editor)
target_link_libraries(HexEditor LibGUI)