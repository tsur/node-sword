Node-Sword
======================


Install node-sword:

```sh
$ npm install node-sword
```

**See the [v0.11](https://github.com/rvagg/node-addon-examples/tree/v0.11/) branch for updated examples applicable to Node v0.11 and above.**

## Compatibility notes

### Node v0.10+
### Unix based system

#How to configure sword

```js

sword = require('node-sword');

sword.configure({

    'base':__dirname+'/sword',/* default: system lookup(see note) */
    'format':sword.FORMAT_RTF,/* default: sword.FORMAT_PLAIN */
    'locale':__dirname+'/sword/locales.d'/* default: /usr/share/sword/locales.d */
    
});

```
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
