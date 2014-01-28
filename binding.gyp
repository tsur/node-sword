{
  "targets": 
  [{
      "target_name": "sword",
      "sources": [ "src/sword.cc", "src/swordHandler.cc", "src/swordModule.cc"],
      "include_dirs": ["/usr/include/sword"],
      "cflags": ["<!(pkg-config --cflags sword)"],
      'libraries': ["<!(pkg-config --libs sword)"]
  }]
}