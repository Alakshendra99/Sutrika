{
  "targets": [
    {
      "target_name": "sutrika",

      "sources": [
        "Sutrika_API.cpp",
        "Static/Sutrika.cpp"
      ],

      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")",
        "Static",
        "External"
      ],

      "libraries": [
        "<(module_root_dir)/External/PCANBasic.lib"
      ],

      "defines": [
        "NAPI_CPP_EXCEPTIONS"
      ],

      "cflags_cc": [
        "/std:c++20"
      ],

      "msvs_settings": {
        "VCCLCompilerTool": {
          "ExceptionHandling": 1
        }
      }
    }
  ]
}