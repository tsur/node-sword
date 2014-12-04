Node-Sword
======================

Node-Sword is a binding for the Sword API Engine which makes it possible to use it from your Node.js based project.

## Setting up

```sh
$ npm install node-sword
```

Note: currently node-sword only works for Unix based systems 

## How to use

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
The "modules" attribute defines the directory containing the mods.conf file or the mods.d folder, not the actual modules location, which can be the same or some another. This last is defined in your mods.conf file or in the mods.d folder containing all configuration files. You may have either all configurations in a single file (mods.conf) or splited into multiple files inside a mods.d folder. Set up an example tree directory as the next one at your root project (same directory where you did run npm install):

- resources
    - mods.d  // Contains the modules configuration files
    - modules // Contains the modules themselves
    - locales // Contains the locale files

As an example, let's download the [KJV bible module](http://www.crosswire.org/sword/servlet/SwordMod.Verify?modName=KJV&pkgType=raw) (find more from [here](http://www.crosswire.org/sword/modules/ModDisp.jsp?modType=Bibles)) and after unzip it, you'll have a folder KJV containing two folders: "mods.d" and "modules". Copy/Move the kjv.conf file inside mods.d folder into resources/mods.d/ and then copy the "texts" folder inside modules folder into resources/modules/

(*) If you do not set a "modules" key in the sword.configure, then it looks by default for modules in a configuration file in some of the following directories:

    1) /etc/sword.conf
    2) /usr/local/etc/sword.conf
    3) $HOME/.sword/
    4) $HOME/sword/
    5) ./
    6) $SWORD_PATH

The configuration file must contain valuable information about the location of your modules

In all cases, except for the first and second one, sword engine looks for a file with the name "mods.conf", or the files in the directory "mods.d". For instance, in the third case, it would look for: $HOME/.sword/mods.conf, $HOME/.sword/mods.d/*.conf

More information [here](http://www.crosswire.org/wiki/Main_Page)
    

## How to retrieve information


```js

var sword = require('node-sword');

sword.info('modules', function(modules){

    console.log(modules);
    
});

sword.info('locales', function(locales){

    console.log(locales);
    
});

```

## How to query sword modules


```js

var sword       = require('node-sword');

try
{
    var bibleKJV       = new sword.module('KJV');

    bibleKJV.read('Genesis 1:1', {keys:false}, function(result){
    
        console.log(result);
    });

    /*
    bibleKJV.search('God', function(result){
    
        console.log(result);
    });

    bibleKJV.read('Genesis 1,2,3', function(result){
    
        console.log(result);
    });

    bibleKJV.read('Judges 1:1', {keys:true, locale:"es"}, function(result){
    
        console.log(result);
    });

    bibleKJV.read('Ex', function(result){
    
        console.log(result);
    });
    */
    
}
catch(e)
{
    console.log(e);
}


```

## Tests

In order to run the tests, first install dependences and then just run "make test" or "npm test" 

```sh
$ npm install
$ make test
```

----------------------------------------------------
