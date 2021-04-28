#include <iostream>
#include <sqlite3.h>
#include <vector>
#include <sstream>
#include <chrono>
#include <ctime>
using namespace std;
//using chrono::system_clock;

//classe para tabela transações
class Transactions{
private:
	int id_accountUser;
	string option, s_now;

	chrono::time_point<chrono::system_clock> now; 
	
	//struct e vetor que armazenam os dados
	struct TRANSACTION{
		int id, id_account;
		double value;
		string s_hour, type_transation;
	};
	vector<TRANSACTION> v;

	//variaveis do bd
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	TRANSACTION t;


public:
	Transactions(){
		rc = sqlite3_open("base.db", &db);
	}

	void init(int id_accountUser){
		this->id_accountUser = id_accountUser;

		dataRetrieve();		
	}
	
	static int callback_Transactions(void *data, int argc, char **argv, char **azColName){
		vector<TRANSACTION>* v = static_cast<vector<TRANSACTION>*>(data);
		
		v->push_back({atoi(argv[0]), atoi(argv[4]), atof(argv[3]), argv[1], argv[2]});
		return 0;
	}

	//função de armazenamento de dados do bd
	void dataRetrieve(){
		v.clear();
		//preparando o comando sql
		string sql = "SELECT * from transações\nWHERE id_conta = " + to_string(id_accountUser) + ";";
		//executando o comando e armazenando os dados na struct
		rc = sqlite3_exec(db, sql.c_str(), callback_Transactions, &v, &zErrMsg);

		if( rc != SQLITE_OK ) {
	      fprintf(stderr, "SQL error: %s\n", zErrMsg);
	      sqlite3_free(zErrMsg);
	   	}
	}

	void close(){
		sqlite3_close(db);
	}

	//printa o extrato
	void printTransactions(){
		cout << "\t# EXTRATO #\n";
		string value;
		for(size_t i = 0; i < v.size(); i++){
			value = to_string_with_precision(v.at(i).value); 
			cout << v.at(i).s_hour << " " << v.at(i).type_transation << " R$ " << value << endl; 
		}
		cout << "\n";
	}

	int withDraw(double balance, string account_type){
		option = "Saque";
		double value;
		cout << "Selecione o valor que deseja sacar: (R$) "; 
		cin >> value;
		cout << "\n";
		if(value > balance && account_type == "normal"){
			cout << "\nNão é possível sacar o valor desejado. (Valor maior que saldo da conta)\n\n";
			return 0;
		}else{
			getNowString();
			
			//preparando o comando sql
			string sql = "INSERT INTO transações (horario, tipo, valor, id_conta) VALUES (\'" + s_now + "\', \'" + option + "\', " + to_string(value * -1) + ", " + to_string(id_accountUser) + ");";
			//executando o comando e armazenando os dados na struct
			rc = sqlite3_exec(db, sql.c_str(), callback_Transactions, &v, &zErrMsg);

			if( rc != SQLITE_OK ) {
		      fprintf(stderr, "SQL error: %s\n", zErrMsg);
		      sqlite3_free(zErrMsg);
		   	}
		   	return balance - value;
	   	}
	   	sqlite3_close(db);
	}

	int deposit(double balance){
		option = "Depósito";
		double value;
		cout << "Selecione o valor que deseja depositar: (R$) ";
		cin >> value;
		cout << "\n";

		getNowString();
			
		//preparando o comando sql
		string sql = "INSERT INTO transações (horario, tipo, valor, id_conta) VALUES (\'" + s_now + "\', \'" + option + "\', " + to_string(value) + ", " + to_string(id_accountUser) + ");";
		//executando o comando e armazenando os dados na struct
		rc = sqlite3_exec(db, sql.c_str(), callback_Transactions, &v, &zErrMsg);

		if( rc != SQLITE_OK ) {
	      fprintf(stderr, "SQL error: %s\n", zErrMsg);
	      sqlite3_free(zErrMsg);
	   	}
		return balance + value;
	}	

	void getNowString(){
		char d[80];
		now = chrono::system_clock::now();
		time_t now_c = chrono::system_clock::to_time_t(now);
		tm now_tm = *std::localtime(&now_c);
		strftime(d, 80, "%d/%m/%y %X", &now_tm);
		s_now = d;
	}

	template <typename T>
	string to_string_with_precision(const T a_value, const int n = 2){ 
	    std::ostringstream out;
	    out.precision(n);
		if(a_value < 0) out << '(' << std::fixed << a_value << ')';
		else out << std::fixed << a_value; 
	    return out.str();
	}


};

//class para a tabela accounts
class Users{

private:
	string check_account, check_password;
	
	//struct que armazena os dados do bd
	struct USER{
		int id;
		double balance;
		string account_number, password, account_type;
	};
	
	
	//variaveis do bd
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	USER u;
	//const char* data = "Callback function called";

	Transactions transaction;

public:
	//construtor
	Users(){
		rc = sqlite3_open("base.db", &db);		
		login();
		option();
		sqlite3_close(db);
	}

	//enquanto o usuário nao oferecer uma entrada compatível, o programa solicita conta e senha
	void login(){ 
		while(true){
			cout << "\nInsira sua Conta Corrente: ";
			cin >> check_account;
			cout << "Insira sua Senha: ";
			cin >> check_password;
			dataRetrieve();
			
			if(u.account_number != "" && u.password != ""){
					cout << "\nLogin realizado\n";
					break;
			}else{
				cout << "\nLogin incorreto\n";		 
			}
		}
		cout << "\n"; 
	}

	static int callback_User(void *data, int argc, char **argv, char **azColName){
		USER *u = (USER *)data;
		u->id = atoi(argv[0]);
		u->account_number = argv[1];
		u->password = argv[2];
		u->account_type = argv[3];
		u->balance = atof(argv[4]);
		//cout << "Id = " << u->id << " account_number = " << u->account_number << " password = " << u->password << " account_type = " << u->account_type << " balance = " << u->balance << endl; 
		return 0;
	}

	//função de armazenamento de dados do bd
	void dataRetrieve(){
		//preparando o comando sql
		string sql = "SELECT * from accounts\nWHERE conta_corrente = " + check_account + " AND senha = " + check_password + ";";
		//executando o comando e armazenando os dados na struct
		rc = sqlite3_exec(db, sql.c_str(), callback_User, &u, &zErrMsg);

		if( rc != SQLITE_OK ) {
	      fprintf(stderr, "SQL error: %s\n", zErrMsg);
	      sqlite3_free(zErrMsg);
	   }
	}

	void updateData(){
		//preparando o comando sql
		string sql = "UPDATE accounts SET saldo = " + to_string(u.balance) + " WHERE ID = " + to_string(u.id) + ";\nSELECT * from accounts\nWHERE ID = " + to_string(u.id) + ";";
		//executando o comando e armazenando os dados na struct
		rc = sqlite3_exec(db, sql.c_str(), callback_User, &u, &zErrMsg);

		if( rc != SQLITE_OK ) {
	      fprintf(stderr, "SQL error: %s\n", zErrMsg);
	      sqlite3_free(zErrMsg);
	   	}
	}
	
	void option(){
		int choice; 
		int newBalance;
		while(true){
			do{
				cout << "Digite o número correspondente a opção desejada:\n\n1. Ver Saldo\n2. Extrato\n3. Saque\n4. Depósito\n5. Transferencia\n6. Solicitar visita do gerente \n7. Trocar de usuário\n8. Sair\n\n";
				cin >> choice;
				cout << "\n";
			}while(choice < 0 || choice > 9);
			
			if(choice == 8) break;
			else if(choice == 7){
				Users user;
				break;	
			}
			switch(choice){
				case 1:
					cout << "Seu saldo é de R$ " << u.balance << endl << endl;
					break;
				case 2:
					transaction.init(u.id);
					transaction.printTransactions();
					break;
				case 3:
					transaction.init(u.id);
					newBalance = transaction.withDraw(u.balance, u.account_type);
					if(newBalance != 0){
						u.balance = newBalance;
						updateData();
					}	
					break;
				case 4:
					transaction.init(u.id);
					u.balance =transaction.deposit(u.balance);
					break;
				case 5:
					transaction.init(u.id);
					break;	
				case 6:
					break;
			}
		}
		// transaction.close();
	}
};


int main(int argc, char const *argv[]){
	Users user;
	return 0;
}