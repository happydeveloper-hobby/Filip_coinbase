#include "Auth.h"
#include "API.h"
#include "App.h"
#include "Websock.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <iomanip>

bool stop = false;

int GetInput()
{
	int choice;
	std::cout << std::endl;
	std::cout << "Please select item number!   ";
	std::cin >> choice;
	std::cout << std::endl;
	std::cout << "Result : " << std::endl;
	std::cout << "================================================================= "  << std::endl << std::endl;
	return choice;
}

void websock(){
	std::vector<std::string> channels = {"heartbeat", "level2", "full", "user"};
	std::string product_id = "BTC-USD";
	std::string uri = "wss://ws-feed.pro.coinbase.com";
	Websock sock(channels, product_id, uri);
	sock.Connect();
	while(!stop){
		std::this_thread::sleep_for(std::chrono::seconds(3));
		std::cout << "------------------------------------" << std::endl;
		std::cout << "\t Buy: " << sock.Best_Buy_Price() << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(3));
		std::cout << "\t Sell: " << sock.Best_Sell_Price() << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(3));
		std::cout << "\t MidMarket: " << sock.MidMarket_Price() << std::endl;
		std::cout << "------------------------------------" << std::endl << std::endl;
	}
	sock.Disconnect();
}

void interupt()
{
	char c;
	std::cout << "if you exit, please Enter key!" << std::endl << std::endl;
	std::cin >> c;
	// std::cin.ignore();
	stop = true;
}

int main()
{
	App app;
	int choice = 0;
	do
	{
		// system("clear");
		std::cout << std::endl << std::endl << std::endl;
		std::cout << "\t" << std::left << std::setw(45) << "1) Get List Accounts" << std::setw(45) << "2)  Get an Account" << std::endl;
		std::cout << "\t" << std::left << std::setw(45) << "3) Get Account History" << std::setw(45) << "4) Get Holds" << std::endl;
		std::cout << "\t" << std::left << std::setw(45) << "5) Place a New Order" << std::setw(45) << "6) Cancel an Order" << std::endl;
		std::cout << "\t" << std::left << std::setw(45) << "7) Cancel all" << std::setw(45) << "8) List Orders" << std::endl;
		std::cout << "\t" << std::left << std::setw(45) << "9) Get an Order" << std::setw(45) << "10) Get Products" << std::endl;
		std::cout << "\t" << std::left << std::setw(45) << "11) Get Single Product" << std::setw(45) << "12) Get Product Order Book" << std::endl;
		std::cout << "\t" << std::left << std::setw(45) << "13) Get Product Ticker" << std::setw(45) << "14) Get Trades" << std::endl;
		std::cout << "\t" << std::left << std::setw(45) << "15) Get Historic Rates" << std::setw(45) << "16) Get 24hr Stats" << std::endl;
		std::cout << "\t" << std::left << std::setw(45) << "17) Get currencies" << std::setw(45) << "18) Get a currency" << std::endl;
		std::cout << "\t" << std::left << std::setw(45) << "19) Get Time" << std::setw(45) << "20) List Fills" << std::endl;
		std::cout << "\t" << std::left << std::setw(45) << "21) Get Current Exchange Limits" << std::setw(45) << "22) List Payment Methods" << std::endl;
		std::cout << "\t" << std::left << std::setw(45) << "23) List Coinbase Accounts" << std::setw(45) << "24) List Deposits" << std::endl;
		std::cout << "\t" << std::left << std::setw(45) << "25) Single Deposit" << std::setw(45) << "26) Payment method for deposit" << std::endl;
		std::cout << "\t" << std::left << std::setw(45) << "27) Coinbase deposit" << std::setw(45) << "28) Generate a Crypto Deposit Address" << std::endl;
		std::cout << "\t" << std::left << std::setw(45) << "29) List Withdrawals" << std::setw(45) << "30) Single Withdrawal" << std::endl;
		std::cout << "\t" << std::left << std::setw(45) << "31) Payment method for Withdraw" << std::setw(45) << "32) Coinbase for withdraw" << std::endl;
		std::cout << "\t" << std::left << std::setw(45) << "33) Withdraws funds to a crypto address." << std::setw(45) << "34) Fee Estimate" << std::endl;
		std::cout << "\t" << std::left << std::setw(45) << "35) Create Conversion" << std::setw(45) << "36) Get Current Fees" << std::endl;
		std::cout << "\t" << std::left << std::setw(45) << "37) Create a new report" << std::setw(45) << "38) Get report status" << std::endl;
		std::cout << "\t" << std::left << std::setw(45) << "39) List Profiles" << std::setw(45) << "40) Get a Profile" << std::endl;
		std::cout << "\t" << std::left << std::setw(45) << "41) Create profile transfer" << std::setw(45) << "42) Trailing Volume" << std::endl;
		std::cout << "\t" << std::left << std::setw(45) << "43) WebSocket Feed" << std::endl << std::endl;
		std::cout << "\t" << std::left << std::setw(45) << "44) Exit" << std::setw(45) << std::endl;
		
		choice = GetInput();

		switch(choice)
            {
                case 1:
                    app.Get_List_Accounts();
                    break;
                case 2:
                    app.Get_Account();
                    break;
				case 3:
                    app.Get_Account_History();
                    break;
				case 4:
                    app.Get_Holds();
                    break;
				case 5:
                    app.Place_New_Order();
                    break;
				case 6:
                    app.Cancel_Order();
                    break;
				case 7:
                    app.Cancel_All_Order();
                    break;
				case 8:
                    app.List_Orders();
                    break;
				case 9:
                    app.Get_Order();
                    break;
				case 10:
                    app.Get_Products();
                    break;
				case 11:
                    app.Get_Single_Product();
                    break;
				case 12:
                    app.Get_Product_Order_Book();
                    break;
				case 13:
                    app.Get_Product_Ticker();
                    break;
				case 14:
                    app.Get_Trades();
                    break;
				case 15:
                    app.Get_Historic_Rates();
                    break;
				case 16:
                    app.Get_24hr_Stats();
                    break;
				case 17:
                    app.Get_Currencies();
                    break;
				case 18:
                    app.Get_Currency();
                    break;
				case 19:
                    app.Get_Time();
                    break;
				case 20:
                    app.List_Fills();
                    break;
				case 21:
                    app.Get_Current_Exchange_Limits();
                    break;
				case 22:
                    app.List_Payment_Methods();
                    break;
				case 23:
                    app.List_Coinbase_Accounts();
                    break;
				case 24:
                    app.List_Deposits();
                    break;
				case 25:
                    app.Single_Deposit();
                    break;
				case 26:
                    app.Payment_Method_Deposit();
                    break;
				case 27:
                    app.Coinbase_Deposit();
                    break;
				case 28:
                    app.Generate_Crypto_Deposit_Address();
                    break;
				case 29:
                    app.List_Withdrawals();
                    break;
				case 30:
                    app.Single_Withdrawal();
                    break;
				case 31:
                    app.Payment_Method_Withdraw();
                    break;
				case 32:
                    app.Coinbase_Withdraw();
                    break;
				case 33:
                    app.Crypto_Withdraw();
                    break;
				case 34:
                    app.Fee_Estimate();
                    break;
				case 35:
                    app.Create_Conversion();
                    break;
				case 36:
                    app.Get_Current_Fees();
                    break;
				case 37:
                    app.Create_new_report();
                    break;
				case 38:
                    app.Get_report_status();
                    break;
				case 39:
                    app.List_Profiles();
                    break;
				case 40:
                    app.Get_Profile();
                    break;
				case 41:
                    app.Create_profile_transfer();
                    break;
				case 42:
                    app.Trailing_Volume();
                    break;
				case 44:
                    break;
				default:
                    break;
				case 43:
					std::thread t(websock); // Separate thread for loop.
					t.detach(); // This actually starts a thread.
					// websock();
					char c;
					std::cout << "if you exit, please type 'exit' and Enter key!" << std::endl << std::endl;
					std::cin >> c;
					std::cin.ignore();
					stop = true;
					// std::terminate();
					// t.~thread();
					break;
                // case 20:
                //     break;
                // default:
                //     break;
            }
		std::cout << std::endl;
		std::cout << "================================================================= "  << std::endl << std::endl;

	} while (choice < 44 );
	

	return 0;
}



