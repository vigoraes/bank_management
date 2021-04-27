#include <iostream>
#include <sqlite3.h>
#include <vector>
using namespace std;

class Users{

private:
	//struct que armazena os dados do bd
	struct USER{
		int id;
		double balance;
		string account_number, password, account_type;
	};
	
	string check_account, check_password;
	
	//variaveis dos bd
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	USER u;
	//const char* data = "Callback function called";
	
	static int callback_User(void *data, int argc, char **argv, char **azColName){
		USER *u = (USER *)data;
		cout << "cheguei" << endl;
		u->id = atoi(argv[0]);
		u->account_number = argv[1];
		u->password = argv[2];
		u->account_type = argv[3];
		u->balance = atof(argv[4]);
		cout << "Id = " << u->id << " account_number = " << u->account_number << " password = " << u->password << " account_type = " << u->account_type << " balance = " << u->balance << endl; 
		return 0;
	}
public:
	//construtor
	Users(){
		rc = sqlite3_open("base.db", &db);		
		//enquanto o usuário não fornecer uma entrada compativel, o programa solicitará nova conta e senha
		while(init());
		sqlite3_close(db);
	}

	bool init(){ 
		cout << "\nInsira sua Conta Corrente: ";
		cin >> check_account;
		cout << "Insira sua Senha: ";
		cin >> check_password;
		dataRetrieve();
		
		if(u.account_number != "" && u.password != ""){
				cout << "\nLogin realizado\n";
				return false;
		}else{
			cout << "\nLogin incorreto\n";
			return true;		 
		}
		// return true; 
	}

	void dataRetrieve(){
		//preparando o comando sql
		string sql = "SELECT * from accounts\nWHERE conta_corrente = " + check_account + " AND senha = " + check_password + ";";
		//executando o comando e armazenando os dados na struct
		rc = sqlite3_exec(db, sql.c_str(), callback_User, &u, &zErrMsg);
	}

	//void()
	
};

class Transactions{

private:
	struct ACTION{
		int id, id_account;
		float value;
		time_t t_hour;
		string s_hour;
	};
public:

};

int main(int argc, char const *argv[]){

	Users user;

	
	return 0;
}