var sword = require('../sword');

sword.configure({
  'modules': __dirname + '/modules'
});
sword.info('modules', function(result){
  console.log(result);
});

var SWModule = sword.module;
var bible_kjv = new sword.module('ASV');

bible_kjv.read('Jn.3.16', function(result) {
  console.log(result);
});
