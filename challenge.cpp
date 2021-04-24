#include <iostream>
#include <fstream>
#include <ctime>
#include <vector>
#include <cstring>
using namespace std;

class Login{

private:
	string conta, senha;

public:
	//realiza obtenção da conta e senha
	void init(){
		cout << "\nInsira sua Conta Corrente: ";
		cin >> conta;
		cout << "Insira sua Senha: ";
		cin >> senha;
	}

	string getConta(){ return conta;}

	string getSenha(){ return senha;}

	bool login(){
		init();
		string check_login, check_senha, tmp;
		ifstream logins("login.txt");
		//verifica se a conta corrente senha fornecidos pelo usuário existe
		while(getline(logins, check_login)){
			check_login.pop_back(); //deleta o '\n'
			getline(logins, check_senha);
			check_senha.pop_back(); //deleta o '\n' se nao for a ultima linha		
			getline(logins, tmp); //pula a linha em branco
			
			if(check_login == getConta()){
				if(check_senha == getSenha()){
					cout << "\nLogin realizado\n";
					logins.close();
					return true;
				}else cout << "\nSenha incorreta\n";
				logins.close();
				return false;	
			} 
		}
		cout << "\nConta Corrente incorreta\n";
		logins.close();
		return false;
	}
};

class Conta{

private:
	string conta, tipo_conta, operacoes, s_date;
	int saldo, tempo_transacoes;
	time_t date;
	struct EXTRATO{
		string horario, tipo, valor;
	};
	vector<EXTRATO> extrato_;

public:
	int escolha;
	Conta(string conta){
		//armezanamento de dados do txt
		this->conta = conta;
		string linha;
		vector<string> txt;
 		ifstream lista_contas("contas.txt");
 		ofstream saida;
 		saida.open("test.txt", std::ofstream::out | std::ofstream::trunc);
 		while(getline(lista_contas, linha)){
 			txt.push_back(linha);
 			linha.pop_back();
 			if(conta == linha){
 				getline(lista_contas, linha);
 				txt.push_back(linha);
 				linha.pop_back();
 				tipo_conta = linha;
 				
 				getline(lista_contas, linha);
 				txt.push_back(linha);
 				linha.pop_back();
 				saldo = get_valor(linha);
 				
 				getline(lista_contas, linha);
 				txt.push_back(linha);
 				linha.pop_back();
 				operacoes = linha;
 				
 				lista_contas.close();
 				break;
 			}else{
 				for(int i = 0; i < 4; i++){
 					getline(lista_contas, linha); //pula 4 linhas
 					txt.push_back(linha);
 				} 
 			}
 		}
 		preenche_extrato();
 		current_date_time();
 		while(escolha != 8){
	 		do{
	 			updateSaldo();
	 			escolha = init();
	 			current_date_time();
	 		}while(!(escolha > 0) || !(escolha < 9)); 
	 		opcoes(escolha);
	 	}

	 	for(int i = 0; i < txt.size(); i++) saida << txt.at(i);
	}
	
	int init(){
		int i;
		cout << "\nDigite o número correspondente a opção desejada:\n1. Ver Saldo\n2. Extrato\n3. Saque\n4. Depósito\n5. Transferencia\n6. Solicitar visita do gerente\n7. Trocar de usuário\n8. Sair\n\n";
		cin >> i;
		cout << "\n";
		return i;
	}

	void opcoes(int opcao){
		switch(opcao){
			case 1:
				getSaldo();
				break;
			case 2:
				cout << "#Extrato da Conta#\n\n";
				for(int i = 0; i < extrato_.size(); i++) cout << extrato_.at(i).horario << " " << extrato_.at(i).tipo << " " << extrato_.at(i).valor << endl;
				break;
			case 3:
				saque();
				break;
			case 4:
				cout << minutosPassados(extrato_.back().horario) << endl;
				break;
			case 5:
				
				break;
			case 6:
				
				break;
			case 7:
				break;
		}
	}
	
	void preenche_extrato(){
		struct EXTRATO tmp;
		if(operacoes != "\n"){
			vector<string> n_operacoes = split(operacoes, ';', 0);
			for(int i = 0; i < n_operacoes.size(); i++){				
				vector<string> infos = split(n_operacoes.at(i), ' ', 1);
				tmp.horario = infos.at(0) + " " + infos.at(1);
				tmp.tipo = infos.at(2);
				tmp.valor = infos.at(3);
				extrato_.push_back(tmp);
				//cout << "Horario: " << tmp.horario << " / Tipo: " << tmp.tipo << " / Valor: " << tmp.valor << endl;
			}
		}
	}
	
	int get_valor(string s){
		if(s[0] == '('){
			s.pop_back();
			s.erase(0, 1);
			return stoi(s) * -1;
		}else return stoi(s);
	}

	string get_s_valor(int i){
		if(i < 0) return to_string(i * - 1);
		else return to_string(i);
	}
	
	vector<string> split(string s, char separador, bool finished){
		int index = 0;
		size_t pos = 0;
		string token;
		vector<string> splitado;
		while ((pos = s.find(separador)) != string::npos) {
		    token = s.substr(0, pos);
		  	splitado.push_back(token);
		    //cout << token <<endl;
		    s.erase(0, pos + 1);
		    index++;
		}
		if(finished) splitado.push_back(s);
		//cout << s << endl;
		return splitado;
	}
	
	void getSaldo(){ cout << "Seu saldo é de R$ " << saldo << endl;}
	
	void saque(){
		int v_saque;
		cout << "Digite o valor que deseja sacar (em R$): ";
		cin >> v_saque;
		if(tipo_conta == "normal" && v_saque > saldo) cout << "\nO valor do saque é maior que o permitido para seu tipo de conta.\n";
		else{
			saldo -= v_saque;
		}
	}

	void updateSaldo(){
		if(saldo < 0){
			int minutos = minutosPassados(extrato_.back().horario);
			int debitado = debitadoSaldo(minutos);
			if(minutos > 0){
				struct EXTRATO up;
				up.horario = s_date;
				cout << up.horario << " horario" << endl;
				up.tipo = "Débito";
				up.valor = get_s_valor(debitado);
				cout << debitado << " valor" << endl;
				extrato_.push_back(up);
			}
		}
	}

	void current_date_time(){		
		date = time(0);
		struct tm *date_tm = localtime(&this->date);
		char *d = ctime(&this->date);
		strftime(d, 80, "%d/%m/%y %X", date_tm);
		s_date = d;
	}
		
	int minutosPassados(string horario){
		cout << horario << endl;
		//cout << s_date << endl;
		char horario_array[17];
		strcpy(horario_array, horario.c_str());
		struct tm tm;
		strptime(horario_array, "%d/%m/%y %X", &tm);
		time_t t_horario = mktime(&tm);
		cout << -1 * difftime( t_horario, date) / 60 - 60<< " minutosPassados\n";
		return -1 * difftime( t_horario, date) / 60 - 60;
	}

	int debitadoSaldo(int minutosPassados){
		int total = 0;
		for(int i = 0; i < minutosPassados; i++){
			saldo += saldo * 0.01;
			total += saldo * 0.01;
		}
		return total;
	}

};

int main(int argc, char const *argv[]){
	Login user;
	while(!user.login()); //enquanto o usuario não fornecer conta e senha corretas, o programa pedirá o login
	Conta conta(user.getConta());
	return 0;
}	