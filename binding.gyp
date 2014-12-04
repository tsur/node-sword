{
  "targets": 
  [{
      "target_name": "<(module_name)",
      "sources": [ "src/sword.cc", "src/swordHandler.cc", "src/swordModule.cc"],
      "include_dirs": ["/usr/include/sword"],
      "cflags": ["<!(pkg-config --cflags sword)"],
      'libraries': ["<!(pkg-config --libs sword)"]
  }, {
      "target_name": "action_after_build",
      "type": "none",
      "dependencies": [ "<(module_name)" ],
      "copies": [
        {
          "files": [ "<(PRODUCT_DIR)/<(module_name).node" ],
          "destination": "<(module_path)"
        }
      ]
    }]
}