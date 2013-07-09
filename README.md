Node-Sword
======================

Installing node-sword:

```sh
$ npm install node-sword
```

## Compatibility notes

- Node v0.10+
- Unix based systems

#How to configure sword

```js

sword = require('node-sword');

sword.configure({

    'base':__dirname+'/sword',/* default: system lookup (See note *) */
    'format':sword.FORMAT_RTF,/* default: sword.FORMAT_PLAIN */
    'locale':__dirname+'/sword/locales.d'/* default: /usr/share/sword/locales.d */
    
});

```

Note *
 
It looks for a configuration file in some of the following directories:

    1) /etc/sword.conf
    2) /usr/local/etc/sword.conf
    3) $HOME/.sword/
    4) $HOME/sword/
    5) ./
    6) $SWORD_PATH

The configuration file must contain valuable information about the location of all modules

In all cases, except first and second one, sword engine looks for some file with the name  "sword.conf", "mods.conf", or the files in the directory "mods.d". For instance, in the third case, it would look for: $HOME/.sword/sword.conf, $HOME/.sword/mods.conf, $HOME/.sword/mods.d/*.conf

More information on: http://www.crosswire.org/wiki/Main_Page
    
End Note *

#How to retrieve information


```js

sword = require('node-sword');

sword.info('modules', function(modules){

    console.log(modules);
    
});

sword.info('locales', function(locales){

    console.log(locales);
    
});

```

#How to query sword modules


```js

sword = require('node-sword');

sword.query('KJV', function(err, module){

    if(err) return console.log('module not found');
    
    //Do your work with module
    module.read('Genesis 1:1', function(result){
    
        console.log(result);
    });
    
    module.search('God', function(result){
    
        console.log(result);
    });
    
});

```

----------------------------------------------------
