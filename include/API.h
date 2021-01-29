// Api.h
#ifndef API_H
#define API_H

#include "Auth.h"
#include <string>
#include <vector>

class API
{
private:
    std::string Call(std::string method, bool authed, std::string path, std::string body);

public:
    API();
    ~API();
    Auth auth;
    std::string uri;
    std::string product_id;
    std::string Get_Buy_Price();
    double Get_Balance(std::string currency);
    std::string Place_Limit_Order(std::string side, std::string price, std::string size);

    // Accounts
    /*
  Name:
      List Accounts
  Summary:
      Get a list of trading accounts from the profile of the API key.
  */
    std::string Get_List_Accounts();

    /*
  Name:
      Get an Account
  Summary:
      Information for a single account. Use this endpoint when you know the account_id. API key must belong to the same profile as the account.
  */
    std::string Get_Account(std::string account_id);

    /*
  Name:
      Get Account History
  Summary:
      List account activity of the API key's profile. Account activity either increases or decreases your account balance. Items are paginated and sorted latest first. See the Pagination section for retrieving additional entries after the first page.    
  */
    std::string Get_Account_History(std::string account_id);

    /*
  Name:
      Get Holds
  Summary:
      List holds of an account that belong to the same profile as the API key. Holds are placed on an account for any active orders or pending withdraw requests. As an order is filled, the hold amount is updated. If an order is canceled, any remaining hold is removed. For a withdraw, once it is completed, the hold is removed.
  */
    std::string Get_Holds(std::string account_id);

    //Orders
    /*
  Name:
      Place a New Order
  Summary:
      You can place two types of orders: limit and market. Orders can only be placed if your account has sufficient funds.     
  Parameters:
      client_oid	[optional] Order ID selected by you to identify your order
      type	      [optional] limit or market (default is limit)
      side	      buy or sell
      product_id	A valid product id
      stp	        [optional] Self-trade prevention flag
      stop	      [optional] Either loss or entry. Requires stop_price to be defined.
      stop_price	[optional] Only if stop is defined. Sets trigger price for stop order.
      
      LIMIT ORDER PARAMETERS
      price	      Price per bitcoin
      size	      Amount of base currency to buy or sell
      time_in_force	[optional] GTC, GTT, IOC, or FOK (default is GTC)
      cancel_after	[optional]* min, hour, day
      post_only	    [optional]** Post only flag

      MARKET ORDER PARAMETERS
      size	      [optional]* Desired amount in base currency
      funds	      [optional]* Desired amount of quote currency to use
  */
    std::string Place_New_Order(std::string side, std::string price, std::string size, std::string client_oid = "", std::string type = "limit", std::string stp = "dc", std::string stop = "", std::string stop_price = "", std::string time_in_force = "GTC", std::string cancel_after = "", bool post_only = true, std::string funds = "");

    /*
  Name:
      Cancel an Order
  Summary:
      Cancel a previously placed order. Order must belong to the profile that the API key belongs to.
  Parameters:
      isOid = false; =>    request URL = DELETE /orders/<id>
      isOid = true;  =>    request URL = DELETE /orders/client:<client_oid>
  */
    std::string Cancel_Order(std::string oid, bool isOid = false);

    /*
  Name:
      Cancel all
  Summary:
      With best effort, cancel all open orders from the profile that the API key belongs to. The response is a list of ids of the canceled orders.
  */
    std::string Cancel_All_Order();

    /*
  Name:
      List Orders
  Summary:
      List your current open orders from the profile that the API key belongs to. Only open or un-settled orders are returned.
  */
    std::string List_Orders(std::string product_id = "", bool isOpen = false, bool isPending = false, bool isActive = false);

    /*
  Name:
      Get an Order
  Summary:
      Get a single order by order id from the profile that the API key belongs to.
  Parameters:
      isOid = false; =>    request URL = GET /orders/<id>
      isOid = true;  =>    request URL = GET /orders/client:<client_oid>
  */
    std::string Get_Order(std::string oid, bool isOid = false);

    // Products
    /*
  Name:
      Get Products
  Summary:
      Get a list of available currency pairs for trading.
  */
    std::string Get_Products();

    /*
  Name:
      Get Single Product
  Summary:
      Get market data for a specific currency pair.
  */
    std::string Get_Single_Product();

    /*
  Name:
      Get Product Order Book
  Summary:
      Get a list of open orders for a product. The amount of detail shown can be customized with the level parameter.
  */
    std::string Get_Product_Order_Book();

    /*
  Name:
      Get Product Ticker
  Summary:
      Snapshot information about the last trade (tick), best bid/ask and 24h volume.
  */
    std::string Get_Product_Ticker();

    /*
  Name:
      Get Trades
  Summary:
      List the latest trades for a product.
  */
    std::string Get_Trades();

    /*
  Name:
      Get Historic Rates
  Summary:
      Historic rates for a product. Rates are returned in grouped buckets based on requested granularity.
  */
    std::string Get_Historic_Rates();

    /*
  Name:
      Get 24hr Stats
  Summary:
      Get 24 hr stats for the product. volume is in base currency units. open, high, low are in quote currency units.
  */
    std::string Get_24hr_Stats();

    //Currencies
    /*
  Name:
      Get currencies
  Summary:
      List known currencies.
  */
    std::string Get_Currencies();

    /*
    Name:
        Get a currency
    Summary:
        List the currency for specified id.
    */
    std::string Get_Currency(std::string cid);

    //Time
    /*
    Name:
        Get Time
    Summary:
        Get the API server time.
    */
    std::string Get_Time();

    //Fills
    /*
    Name:
        List Fills
    Summary:
        Get a list of recent fills of the API key's profile.
    */
    std::string List_Fills(std::string order_id = "", std::string product_id = "");

    //Limits
    /*
    Name:
        Get Current Exchange Limits
    Summary:
        This request will return information on your payment method transfer limits, as well as buy/sell limits per currency.
    */
   std::string Get_Current_Exchange_Limits(); 

    //Payment Methods
    /*
    Name:
        List Payment Methods
    Summary:
        Get a list of your payment methods.
    */
   std::string List_Payment_Methods(); 

   //Coinbase Accounts
    /*
    Name:
        List Accounts
    Summary:
        Get a list of your coinbase accounts.
    */
   std::string List_Coinbase_Accounts(); 

    //Deposits
    /*
    Name:
        List Deposits
    Summary:
        Get a list of deposits from the profile of the API key, in descending order by created time
    */
   std::string List_Deposits(std::string profile_id = "", std::string before = "", std::string after = "", std::string limit = ""); 

    /*
    Name:
        Single Deposit
    Summary:
        Get information on a single deposit.
    */
    std::string Single_Deposit(std::string transfer_id);

    /*
    Name:
        Payment method
    Summary:
        Deposit funds from a payment method. See the Payment Methods section for retrieving your payment methods.
    */
    std::string Payment_Method_Deposit(std::string amount, std::string currency, std::string payment_method_id);

    /*
    Name:
        Coinbase
    Summary:
         You can move funds between your Coinbase accounts and your Coinbase Pro trading accounts within your daily limits.
    */
    std::string Coinbase_Deposit(std::string amount, std::string currency, std::string coinbase_account_id);

    /*
    Name:
        Generate a Crypto Deposit Address
    Summary:
        You can generate an address for crypto deposits.
    */
    std::string Generate_Crypto_Deposit_Address(std::string coinbase_account_id);


    //Withdraw
    /*
    Name:
        List Withdrawals
    Summary:
        Get a list of withdrawals from the profile of the API key, in descending order by created time. 
    */
    std::string List_Withdrawals(std::string profile_id = "", std::string before = "", std::string after = "", std::string limit = "");

    /*
    Name:
        Single Withdrawal
    Summary:
        Get information on a single withdrawal.
        It may contain the cancel code, so you must process the cancel code!
    */
    std::string Single_Withdrawal(std::string transfer_id);

    /*
    Name:
        Payment method
    Summary:
        Withdraw funds from a payment method. See the Payment Methods section for retrieving your payment methods.
    */
    std::string Payment_Method_Withdraw(std::string amount, std::string currency, std::string payment_method_id);

    /*
    Name:
        Coinbase
    Summary:
         You can move funds between your Coinbase accounts and your Coinbase Pro trading accounts within your daily limits.
    */
    std::string Coinbase_Withdraw(std::string amount, std::string currency, std::string coinbase_account_id);

    /*
    Name:
        Crypto
    Summary:
        Withdraws funds to a crypto address.
    */
    std::string Crypto_Withdraw(std::string amount, std::string currency, std::string crypto_address, std::string destination_tag = "", std::string no_destination_tag = "", std::string add_network_fee_to_total = "");

    /*
    Name:
        Fee Estimate
    Summary:
        Gets the network fee estimate when sending to the given address.
    */
    std::string Fee_Estimate(std::string currency, std::string crypto_address);

    //Stablecoin Conversions
    /*
    Name:
        Create Conversion
    Summary:
        A successful conversion will be assigned a conversion id. The corresponding ledger entries for a conversion will reference this conversion id. 
    */
    std::string Create_Conversion(std::string from, std::string to, std::string amount);

    //Fees
    /*
    Name:
        Get Current Fees
    Summary:
        This request will return your current maker & taker fee rates, as well as your 30-day trailing volume.
    */
    std::string Get_Current_Fees();

    //Reports
    /*
    Name:
        Create a new report
    Summary:
        Reports provide batches of historic information about your profile in various human and machine readable forms.
    */
    std::string Create_new_report(std::string type, std::string start_date, std::string end_date, std::string product_id = "", std::string account_id = "", std::string format = "", std::string email = "");

    /*
    Name:
        Get report status
    Summary:
        Once a report request has been accepted for processing, the status is available by polling the report resource endpoint.
    */
    std::string Get_report_status(std::string report_id);

    //Profiles
    /*
    Name:
        List Profiles
    Summary:
        List your profiles.
    */
    std::string List_Profiles(std::string active = "true");

    /*
    Name:
        Get a Profile
    Summary:
        Get a single profile by profile id.
    */
    std::string Get_Profile(std::string profile_id);

    /*
    Name:
        Create profile transfer
    Summary:
        Transfer funds from API key's profile to another user owned profile.
    */
    std::string Create_profile_transfer(std::string from, std::string to, std::string currency, std::string amount  );
    
    //User Account
    /*
    Name:
        Trailing Volume
    Summary:
        This request will return your 30-day trailing volume for all products of the API key's profile. This is a cached value that's calculated every day at midnight UTC.
    */
    std::string Trailing_Volume();













};



#endif // API_H
