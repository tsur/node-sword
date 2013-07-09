var should = require('should')//require('assert')
    ,sword = require('../build/Release/sword')
    ;

describe('query keys from different modules', function(){
  
    before(function(){

    });
  
    it('Module not found', function(done){

        sword
        .query('UNKNOWN', function(err,module){
        
            should.exists(err);
            //err.should.be.ok();
            err.should.be.true;
            
            should.not.exist(module);
            should.strictEqual(undefined,module);
            
            done();
        
        });

    });
    
    it('Module found', function(done){

        sword
        .query('KJV', function(err,module){
        
            should.not.exist(err);
            should.strictEqual(null,err);
            
            should.exists(module);

            module.should.be.a('object');
            module.should.have.property('read');
            module.should.have.property('search');
            
            done();
        
        });

    });
    
    it('read a module', function(done){

        sword
        .query('KJV', function(err,module){
        
            should.not.exist(err);
            should.strictEqual(null,err);
            should.exists(module);
            module.should.be.a('object');
            module.should.have.property('read');
            module.should.have.property('search');
            
            module.read('gen1:1',function(result){

                    result.should.be.a('string');
                    result.should.not.be.empty;
                    done();
            });
        });
        
    });
    
    it('search for a valid word in a module', function(done){

        sword
        .query('KJV', function(err,module){
        
            should.not.exist(err);
            should.strictEqual(null,err);
            should.exists(module);
            module.should.be.a('object');
            module.should.have.property('read');
            module.should.have.property('search');
            
            module.search('Lamech',{scope:'genesis'},function(result){

                    result.should.be.an.instanceOf(Array);
                    result.should.not.be.empty;
                    done();
            });
        });
        
    });
    
    it('search for a invalid word in a module', function(done){

        sword
        .query('KJV', function(err,module){
        
            should.not.exist(err);
            should.strictEqual(null,err);
            should.exists(module);
            module.should.be.a('object');
            module.should.have.property('read');
            module.should.have.property('search');
            
            module.search('03hello',{scope:'genesis'},function(result){

                    result.should.be.an.instanceOf(Array);
                    result.should.be.empty;
                    done();
            });
        });
        
    });
    
    after(function(){

    });
  
});


