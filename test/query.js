var should = require('should')//require('assert')
    ,sword = require('../build/Release/sword')
    ;

sword.configure({

    'modules'   : __dirname+'/../resources',   /* default: system lookup (See note * below) */
    // 'format'    : sword.FORMAT_RTF,                 /* default: sword.FORMAT_PLAIN */
    'locales'   : __dirname+'/../resources/locales'    /* default: /usr/share/sword/locales.d */
    
});

describe('TestSuite for loading modules:', function(){
  
    before(function(){

    });
  
    it('should throw exception when module not found', function(){

        (function(){
          
          var module = new sword.module('UNKNOWN');

        }).should.throw();

        (function(){
          
          var module = new sword.module('');

        }).should.throw();

        (function(){
          
          var module = new sword.module();

        }).should.throw();

        (function(){
          
          var module = new sword.module(20);

        }).should.throw();

        (function(){
          
          var module = new sword.module({});

        }).should.throw();

        (function(){
          
          var module = new sword.module(true);

        }).should.throw();

    });

    it('should not throw exception when module is found', function(){

        (function(){
          
          var bible_kjv = new sword.module('KJV');

        }).should.not.throw();

    });

    after(function(){

    });
  
});

describe('TestSuite for querying modules:', function(){
  
    before(function(){

    });

    it('should read right text for an existing key', function (done){

        (function(){

            var bible_kjv = new sword.module('KJV');

            bible_kjv.read('Genesis 1:1', function(result){

                should.exists(result);
                result.should.not.be.empty;
                // result.should.equal("Genesis 1:1 In the beginning God created the heaven and the earth.");
                done();

            });

        }).should.not.throw();

    });

    it('should not read any text for an unexisting key', function(done){

        (function(){

          var bible_kjv = new sword.module('KJV');

          bible_kjv.read('unknown 1:1', function(result){

            should.exists(result);
            result.should.be.empty;

            done();

          });

        }).should.not.throw();

    });

    it('should get matches for an existing text on a module', function(done){

        (function(){

          var bible_kjv = new sword.module('KJV');

          bible_kjv.search('Lamech', function(result){

            result.should.be.an.instanceOf(Array);
            result.should.not.be.empty;

            done();

          });

        }).should.not.throw();

    });

    it('should not get matches for an unexisting text on a module', function(done){

        (function(){

          var bible_kjv = new sword.module('KJV');

          bible_kjv.search('trinity', function(result){

            result.should.be.an.instanceOf(Array);
            result.should.be.empty;

            done();

          });

        }).should.not.throw();
        
    });

    it('should not get matches for an existing text on some foreign module but not present in the asked module', function(done){

        (function(){

          var bible_kjv = new sword.module('KJV');

          //Dios means God in Spanish
          bible_kjv.search('Dios', function(result){

            result.should.be.an.instanceOf(Array);
            result.should.be.empty;

            done();

          });

        }).should.not.throw();
        
    });

    it('should work in the event of concurrent requests', function(done){

        (function(){

          var bible_kjv     = new sword.module('KJV');
          var num_requests  = 0;

          num_requests++;
          bible_kjv.read('Genesis 1:1', function(result){

            should.exists(result);
            result.should.not.be.empty;

            num_requests--;

            if(!num_requests)
            {
                done();
            }

          });

          num_requests++;
          bible_kjv.read('Genesis 1:2', function(result){

            should.exists(result);
            result.should.not.be.empty;

            num_requests--;

            if(!num_requests)
            {
                done();
            }

          });

          num_requests++;
          bible_kjv.read('Genesis 1:3', function(result){

            should.exists(result);
            result.should.not.be.empty;

            num_requests--;

            if(!num_requests)
            {
                done();
            }

          });

        }).should.not.throw();
        
    });

    // TODO
    // it('should not display keys when options keys is set to false', function(done){
        
        
    // });
    
    
    after(function(){

    });
  
});