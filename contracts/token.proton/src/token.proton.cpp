/*##################################*\
#
#
# Created by CryptoLions.io
#
#
\*##################################*/


#include <token.proton/token.proton.hpp>
#include <eosio/system.hpp>


namespace eosio {
	

	void tokenproton::reg(name tcontract, string tname, string url, string desc, string iconurl, symbol symbol){
		
		check( is_account( tcontract ), "tcontract account does not exist");

		check( tname.size() <= 16 , "max length for token name is 16.");
		check( url.size() <= 256 , "max length for url is 256.");
		check( iconurl.size() <= 256 , "max length for iconurl is 256.");
		check( desc.size() <= 512 , "max length for description is 512.");		
		
		check( symbol.is_valid(), "invalid symbol name" );
    
		//stats statstable( tcontract, symbol.code().raw() );
		//auto tokeninfo = statstable.find( symbol.code().raw() );
		//check( tokeninfo != statstable.end(), "token with contract and symbol does not exist, create token before registration." );
		//const auto& st = *tokeninfo;
		//require_auth( st.issuer );
		
		require_auth( tcontract );
		require_recipient( tcontract );
		
		tokens tokens_( _self, _self.value );
		
		auto tcontract_index = tokens_.template get_index< "tcontract"_n >();		
		for ( auto itro = tcontract_index.find( tcontract.value ); itro != tcontract_index.end(); itro++ ) {
			check( itro->symbol != symbol, "Token already registered");
		}
		
		auto newid = getid();
		tokens_.emplace( tcontract, [&]( auto& t ) {
			t.id = newid;
			t.tcontract = tcontract;
			t.tname = tname;
			t.url = url;
			t.desc = desc;
			t.iconurl = iconurl;
			t.symbol = symbol;
			t.blisted = false;
		});	
	}
		
	void tokenproton::update(uint64_t id, name tcontract, string tname, string url, string desc, string iconurl, symbol symbol){
		
		check( is_account( tcontract ), "tcontract account does not exist");

		check( tname.size() <= 16 , "max length for token name is 16.");
		check( url.size() <= 256 , "max length for url is 256.");
		check( iconurl.size() <= 256 , "max length for iconurl is 256.");
		check( desc.size() <= 512 , "max length for description is 512.");		
	
		check( symbol.is_valid(), "invalid symbol name" );
    
		tokens tokens_( _self, _self.value );		
		auto itr = tokens_.require_find( id, string("id: " + to_string( id ) + " cannot be found").c_str() );
		
		require_auth( tcontract );
		require_auth( itr->tcontract );
		
		require_recipient( tcontract );

		tokens_.modify( itr, tcontract, [&]( auto& t ) {
			t.tcontract = tcontract;
			t.tname = tname;
			t.url = url;
			t.desc = desc;
			t.iconurl = iconurl;
			t.symbol = symbol;
		});	
	}


	void tokenproton::remove(uint64_t id){

		tokens tokens_( _self, _self.value );		
		auto itr = tokens_.require_find( id, string("id: " + to_string( id ) + " cannot be found").c_str() );		
		require_auth( itr->tcontract );		
		
		check (!(itr->blisted), "Blacklisted tokens cannot be removed.");
		tokens_.erase( itr );
	}

	
	void tokenproton::updblacklist(uint64_t id, bool blisted){
		
		require_auth( _self );
		
		tokens tokens_( _self, _self.value );		
		auto itr = tokens_.require_find( id, string("id: " + to_string( id ) + " cannot be found").c_str() );
		
		tokens_.modify( itr, _self, [&]( auto& t ) {
			t.blisted = blisted;
		});	

	}

	
	uint64_t tokenproton::getid() {
		uint64_t resid;	

		conf config( _self, _self.value );
		_cstate = config.exists() ? config.get() : global{};

		_cstate.tid++;
		resid = _cstate.tid;	
		config.set( _cstate, _self );

		return resid;
	}

	
}


EOSIO_DISPATCH( eosio::tokenproton, (reg)(update)(remove)(updblacklist))  