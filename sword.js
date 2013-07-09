var sword = require('./build/Release/sword');

/*
    Busqueda de modulos
    
    1. Se busca un archivo de configuracion
    
        a) /etc/sword.conf
        b) /usr/local/etc/sword.conf
        c) $HOME/.sword/
        d) $HOME/sword/
        e) ./
        f) $SWORD_PATH
        g) custom(el que tu quieras)
        
        Nota: Excepto para a y b, en el resto se buscan alguno de los ficheros que coincidan con "sword.conf", "mods.conf", o los ficheros del directorio "mods.d". Por ejemplo, para el caso c, se buscaría: $HOME/.sword/sword.conf, $HOME/.sword/mods.conf, $HOME/.sword/mods.d/*.conf
        
    2. El/los archivo/s de configuración contienen información acerca de cada uno de los modulos existentes como por ejemplo su ubicación real

    Como crear el archivo de configuracion: http://www.crosswire.org/wiki/DevTools:conf_Files
    
    
    CONFIGURE:
    
    sword.configure('path to configure','markup');
    sword.configure('path to configure'); //markup default to PLAIN
    
    MODULE:
    
    
*/
console.log('inicio');
//console.log(addon.configure('hola'));
/*
sword.query("KJV", function(err, bible){
    
    console.log(err, bible);

    if(err) return console.log(err);
    
    bible.read('gene1:1', function(err,text){
    
        console.log(err,text);
    });
    //bible.search('peter', function(matches){});
});
*/
console.log(__dirname+'/sword/locales.d/');

//console.log(sword.FORMAT_PLAIN);

sword.configure({

    //'base':__dirname+'/sword',//default: lo explicado arriba del todo
    //'format':sword.FORMAT_RTF,//default: sword.FORMAT_PLAIN
    //'locale':__dirname+'/sword/locales.d'//default: /usr/share/sword/locales.d
});

/*
sword.info('locales', function(result){

    console.log('locales: ',result);
});

sword.info('locales', function(result){

    console.log('locales: ',result);
});
*/
sword.query("KJV", function(err, bible){

    if(err) return console.log(err);
    
    bible.read('rt1:34', {locale:'es', filters:[sword.FILTER_HEADINGS], format: sword.FORMAT_HTML}, function(result){
        console.log(result);
    });
    /**/
    /*bible.search("Wash thyself therefore", {scope:'rt'}, function(matches){
    bible.search("Wash thyself therefore", {scope:'rt', locale:'es'}, function(matches){
        
        console.log(matches);
    });*/
    
});

console.log('fin');
