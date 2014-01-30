var sword = require('./build/Release/sword');

sword.configure({

    'modules'   : __dirname+'/resources',   /* default: system lookup (See note * below) */
    // 'format'    : sword.FORMAT_RTF,                 /* default: sword.FORMAT_PLAIN */
    'locales'   : __dirname+'/resources/locales'    /* default: /usr/share/sword/locales.d */
    
});

sword.info('locales', function(result){

    console.log('locales: ',result);
});

sword.info('modules', function(modules){

    console.log(modules);
    
});

var SWModule = sword.module;

try
{
    var bible_kjv       = new sword.module('KJV');
    var bible_chincsv   = new SWModule('ChiNCVs');

    bible_kjv.search('God', function(result){
    
        console.log(result);
    });

    bible_chincsv.read('Genesis 1:1', function(result){
    
        console.log(result);
    });

    // bible_kjv.read('Genesis 1:1', function(result){
    
    //     console.log(result);
    // });

    // bible_kjv.read('Judges 1:1', {keys:true, locale:"fi"}, function(result){
    
    //     console.log(result);
    // });
    
}
catch(e)
{
    console.log(e);
}

// try
// {
//     var bible = new sword.module('?');
// }
// catch(e)
// {
//     console.log(e);
// }

// try
// {
//     var bible = new sword.module();
// }
// catch(e)
// {
//     console.log(e);
// }

// try
// {
//     var bible = new sword.module(4);
// }
// catch(e)
// {
//     console.log(e);
// }


// sword.module('KJV', function(err, bible){

//     if(err) return console.log('bible not found');
    
//     //Do your work with module
//     // module.read('Genesis 1:1', function(result){
    
//     //     console.log(result);
//     // });

//     bible.read('Genesis 1:2', function(result){
    
//         console.log(result);
//     });
    
//     // bible.search('God', function(result){
    
//     //     console.log(result);
//     // });
    
// });

// sword.module("ChiNCVs", function(err, bible){

//     if(err) return console.log(err);
    
//     bible.read('Genesis 1:1', function(result){

//         console.log(result);
//     });
// });