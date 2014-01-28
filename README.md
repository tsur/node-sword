Node-Sword
======================

Note: Node-sword is a port for scripturesOS.

Installing node-sword:

```sh
$ npm install node-sword
```

## Dependences

- Sword API Engine (http://www.crosswire.org/sword/docs/apiinstall.jsp)

Note: node-sword only works for Unix Systems 

#How to configure sword

```js

var sword = require('node-sword');

sword.configure({

    //Directory containing mods.conf file or mods.d directory
    'modules'   : __dirname+'/resources',   /* default: system lookup (See note * below) */
    //Output format
    'format'    : sword.FORMAT_RTF,                 /* default: sword.FORMAT_PLAIN */
    //Directory containing locale files
    'locales'   : __dirname+'/resources/locales'    /* default: /usr/share/sword/locales.d */
    
});

```
The modules attribute defines the directory containing mods.conf file or the mods.d folder, not the actual modules location, which can be the same or some another. This last is defined in your mods.conf file or in the mods.d folder containing configuration files. An directory tree example could be the next:

- resources
    - mods.d  //The modules configuration files
    - modules //The modules themselves
    - locales //The locale files

Note *
 
It looks for modules in a configuration file in some of the following directories:

    1) /etc/sword.conf
    2) /usr/local/etc/sword.conf
    3) $HOME/.sword/
    4) $HOME/sword/
    5) ./
    6) $SWORD_PATH

The configuration file must contain valuable information about the location of your modules

In all cases, except for the first and second one, sword engine looks for a file with the name "mods.conf", or the files in the directory "mods.d". For instance, in the third case, it would look for: $HOME/.sword/mods.conf, $HOME/.sword/mods.d/*.conf

More information on: http://www.crosswire.org/wiki/Main_Page
    
End Note *

#How to retrieve information


```js

var sword = require('node-sword');

sword.info('modules', function(modules){

    console.log(modules);
    
});

sword.info('locales', function(locales){

    console.log(locales);
    
});

```

#How to query sword modules


```js

var sword = require('node-sword');

sword.module('KJV', function(err, bible){

    if(err) return console.log('Bible not found');
    
    //Do your work with module
    bible.read('Genesis 1:1', function(result){
    
        console.log(result);
    });
    
    bible.search('God', function(result){
    
        console.log(result);
    });
    
});

```

----------------------------------------------------
