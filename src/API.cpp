#include "API.h"
#include <iostream>
#include "curl/curl.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

using namespace rapidjson;

/* Used by API::Call to put websource into a string type */
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  ((std::string *)userp)->append((char *)contents, size * nmemb);
  return size * nmemb;
}

/* Uses libcurl to get Data From API */
std::string API::Call(std::string method, bool authed, std::string path, std::string body)
{
  CURL *curl;
  CURLcode res;
  std::string readBuffer;
  curl = curl_easy_init();
  if (curl)
  {
    struct curl_slist *chunk = NULL;
    curl_easy_setopt(curl, CURLOPT_URL, (uri + path).c_str());
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl/1.0");
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    chunk = curl_slist_append(chunk, "Content-Type: application/json");
    if (authed)
    {
      std::string time_stamp = auth.GetTimestamp();
      std::string sign = auth.Sign(time_stamp, method, path, body);
      chunk = curl_slist_append(chunk, ("CB-ACCESS-KEY: " + auth.Key).c_str());
      chunk = curl_slist_append(chunk, ("CB-ACCESS-SIGN: " + sign).c_str());
      chunk = curl_slist_append(chunk, ("CB-ACCESS-TIMESTAMP: " + time_stamp).c_str());
      chunk = curl_slist_append(chunk, ("CB-ACCESS-PASSPHRASE: " + auth.Passphrase).c_str());
    }
    res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
    if (method == "POST")
    {
      curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, -1L);
      curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
    }
    if (method == "DELETE")
    {
      curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
    }
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    res = curl_easy_perform(curl);
    /* Check for errors */
    if (res != CURLE_OK)
      std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
    /* always cleanup */
    curl_easy_cleanup(curl);
    /* free the custom headers */
    curl_slist_free_all(chunk);
  }
  return readBuffer;
}

std::string API::Get_Buy_Price()
{
  std::string ret = "";
  std::string st = Call("GET", false, "/products/" + product_id + "/book", "");
  Document d;
  d.Parse(st.c_str());
  if (d.HasMember("message"))
  {
    assert(d["message"].IsString());
    std::cout << "(Get_Buy_Price) Message: " << d["message"].GetString() << std::endl;
  }
  if (d.HasMember("bids"))
  {
    assert(d["bids"].IsArray());
    const Value &c = d["bids"];
    assert(c[0].IsArray());
    const Value &b = c[0];
    assert(b[0].IsString());
    ret = b[0].GetString();
  }
  return ret;
}

double API::Get_Balance(std::string currency)
{
  double ret = 0;
  std::string txt = Call("GET", true, "/accounts", "");
  Document d;
  d.Parse(txt.c_str());
  assert(d.IsArray());
  for (SizeType i = 0; i < d.Size(); i++)
  {
    assert(d[i].HasMember("currency"));
    assert(d[i]["currency"].IsString());
    std::string cur = d[i]["currency"].GetString();
    if (cur == currency)
    {
      assert(d[i].HasMember("available"));
      assert(d[i]["available"].IsString());
      ret = std::stod(d[i]["available"].GetString());
      return ret;
    }
  }
  return ret;
}

std::string API::Place_Limit_Order(std::string side, std::string price, std::string size)
{
  std::string order_id = "";
  Document d;
  d.SetObject();
  rapidjson::Document::AllocatorType &allocator = d.GetAllocator();

  /* adds type */
  d.AddMember("type", "limit", allocator);

  /* Adds the Size */
  Value v_side;
  v_side = StringRef(side.c_str());
  d.AddMember("side", v_side, allocator);

  /* Adds the Product id */
  Value v_product_id;
  v_product_id = StringRef(product_id.c_str());
  d.AddMember("product_id", v_product_id, allocator);

  /* Adds the price (USD) */
  Value v_price;
  v_price = StringRef(price.c_str());
  d.AddMember("price", v_price, allocator);

  /* Adds the size (BTC, ETH, LTC) */
  Value v_size;
  v_size = StringRef(size.c_str());
  d.AddMember("size", v_size, allocator);

  /* Gets the Order to be a maker and not a taker */
  d.AddMember("post_only", true, allocator);

  /* creates the string in json */
  StringBuffer strbuf;
  Writer<StringBuffer> writer(strbuf);
  d.Accept(writer);
  std::string returned = Call("POST", true, "/orders", strbuf.GetString());
  // std::cout << "returned = " << returned << std::endl;
  // std::cout << "returned strbuf.GetString = " << strbuf.GetString() << std::endl;
  Document d_1;
  d_1.Parse(returned.c_str());
  // std::cout << "returned.c_str() = " << returned.c_str() << std::endl;
  if (d_1.HasMember("id"))
  {
    assert(d_1["id"].IsString());
    order_id = d_1["id"].GetString();
  }
  if (d_1.HasMember("message"))
  {
    assert(d_1["message"].IsString());
    std::cout << "(Limit_Order) Message: " << d_1["message"].GetString() << std::endl;
  }
  return order_id;
}

//List Accounts
std::string API::Get_List_Accounts()
{
  std::string res = Call("GET", true, "/accounts/", "");
  std::cout << res;
  return "";
}

//Get an Account
std::string API::Get_Account(std::string account_id)
{
  std::string res = Call("GET", true, "/accounts/" + account_id, "");
  std::cout << res;
  return "";
}

//Get Account History
std::string API::Get_Account_History(std::string account_id)
{
  std::string res = Call("GET", true, "/accounts/" + account_id + "/ledger", "");
  std::cout << res;
  return "";
}

//Get Holds
std::string API::Get_Holds(std::string account_id)
{
  std::string res = Call("GET", true, "/accounts/" + account_id + "/ledger", "");
  std::cout << res;
  return "";
}

// Place a New Order
std::string API::Place_New_Order(std::string side, std::string price, std::string size, std::string client_oid, std::string type, std::string stp, std::string stop, std::string stop_price, std::string time_in_force, std::string cancel_after, bool post_only, std::string funds)
{
  std::string order_id = "";
  Document d;
  d.SetObject();
  rapidjson::Document::AllocatorType &allocator = d.GetAllocator();

  /* [optional] adds type */
  Value v_type;
  v_type = StringRef(type.c_str());
  d.AddMember("type", v_type, allocator);

  /* Adds the Size */
  Value v_side;
  v_side = StringRef(side.c_str());
  d.AddMember("side", v_side, allocator);

  /* Adds the Product id */
  Value v_product_id;
  v_product_id = StringRef(product_id.c_str());
  d.AddMember("product_id", v_product_id, allocator);

  /* Adds the price (USD) */
  Value v_price;
  v_price = StringRef(price.c_str());
  d.AddMember("price", v_price, allocator);

  /* Adds the size (BTC, ETH, LTC) */
  Value v_size;
  v_size = StringRef(size.c_str());
  d.AddMember("size", v_size, allocator);

  /* [optional] Gets the Order to be a maker and not a taker */
  d.AddMember("post_only", post_only, allocator);

  /* [optional] Order ID selected by you to identify your order */
  Value v_client_oid;
  v_client_oid = StringRef(client_oid.c_str());
  d.AddMember("client_oid", v_client_oid, allocator);

  /* [optional] Only if stop is defined. Sets trigger price for stop order. */
  Value v_stop_price;
  v_stop_price = StringRef(stop_price.c_str());
  d.AddMember("stop_price", v_stop_price, allocator);

  /* [optional] Self-trade prevention flag   
  dc	Decrease and Cancel (default)
  co	Cancel oldest
  cn	Cancel newest
  cb	Cancel both  */
  Value v_stp;
  v_stp = StringRef(stp.c_str());
  d.AddMember("stp", v_stp, allocator);

  /* [optional] Either loss or entry. Requires stop_price to be defined. */
  Value v_stop;
  v_stop = StringRef(stop.c_str());
  d.AddMember("stop", v_stop, allocator);

  /* [optional] GTC, GTT, IOC, or FOK (default is GTC) */
  Value v_time_in_force;
  v_time_in_force = StringRef(time_in_force.c_str());
  d.AddMember("time_in_force", v_time_in_force, allocator);

  /* [optional]* min, hour, day */
  Value v_cancel_after;
  v_cancel_after = StringRef(cancel_after.c_str());
  d.AddMember("cancel_after", v_cancel_after, allocator);

  /* [optional] Desired amount of quote currency to use */
  Value v_funds;
  v_funds = StringRef(funds.c_str());
  d.AddMember("funds", v_funds, allocator);

  /* creates the string in json */
  StringBuffer strbuf;
  Writer<StringBuffer> writer(strbuf);
  d.Accept(writer);
  std::string res = Call("POST", true, "/orders", strbuf.GetString());
  Document d_1;
  d_1.Parse(res.c_str());
  if (d_1.HasMember("id"))
  {
    assert(d_1["id"].IsString());
    order_id = d_1["id"].GetString();
  }
  if (d_1.HasMember("message"))
  {
    assert(d_1["message"].IsString());
    std::cout << "(Place a new order) Message: " << d_1["message"].GetString() << std::endl;
  }
  std::cout << "(Place a new order) Res: " << res << std::endl;
  return res;
}

// Cancel Order
std::string API::Cancel_Order(std::string oid, bool isOid)
{
  std::string url = "";
  if (!isOid)
  {
    url = "/orders/" + oid;
  }
  else
    url = "/orders/client:" + oid;
  std::string res = Call("DELETE", true, url, "");
  std::cout << res;
  Document d;
  d.Parse(res.c_str());
  if (d.HasMember("message"))
  {
    assert(d["message"].IsString());
    std::cout << "(Cancel_Order) Message: " << d["message"].GetString() << std::endl;
  }
  return "";
}

// Cancel All Orders
std::string API::Cancel_All_Order()
{
  std::string res = Call("DELETE", true, "/orders", "");
  std::cout << res;
  return "";
}

//List Orders
std::string API::List_Orders(std::string product_id, bool isOpen, bool isPending, bool isActive)
{
  std::string url = "/orders?";
  if (product_id != "")
  {
    url += "product_id=" + product_id + "&";
  }
  if (isOpen)
  {
    url += "status=open&";
  }
  if (isPending)
  {
    url += "status=pending&";
  }
  if (isActive)
  {
    url += "status=active";
  }

  std::string res = Call("GET", true, url, "");
  std::cout << res << std::endl;
  return "";
}

//Get an Order
std::string API::Get_Order(std::string oid, bool isOid)
{
  std::string url = "";
  if (!isOid)
  {
    url = "/orders/" + oid;
  }
  else
    url = "/orders/client:" + oid;
  std::string res = Call("GET", true, url, "");
  std::cout << res << std::endl;
  Document d;
  d.Parse(res.c_str());
  if (d.HasMember("message"))
  {
    assert(d["message"].IsString());
    std::cout << "(Get_Order) Message: " << d["message"].GetString() << std::endl;
  }
  return "";
}

//Products
// Get Products
std::string API::Get_Products()
{
  std::string url = "/products";

  std::string res = Call("GET", false, url, "");
  std::cout << res << std::endl;
  return "";
}

//Get Single Product
std::string API::Get_Single_Product()
{
  std::string url = "/products/" + product_id;
  std::string res = Call("GET", false, url, "");
  std::cout << res << std::endl;
  return "";
}

//Get Product Order Book
std::string API::Get_Product_Order_Book()
{
  std::string url = "/products/" + product_id + "/book";
  std::string res = Call("GET", false, url, "");
  std::cout << res << std::endl;
  return "";
}

//Get Product Ticker
std::string API::Get_Product_Ticker()
{
  std::string url = "/products/" + product_id + "/ticker";
  std::string res = Call("GET", false, url, "");
  std::cout << res << std::endl;
  return "";
}

//Get Trades
std::string API::Get_Trades()
{
  std::string url = "/products/" + product_id + "/trades";
  std::string res = Call("GET", false, url, "");
  std::cout << res << std::endl;
  return "";
}

//Get Historic Rates
std::string API::Get_Historic_Rates()
{
  std::string url = "/products/" + product_id + "/candles";
  std::string res = Call("GET", false, url, "");
  std::cout << res << std::endl;
  return "";
}

//Get 24hr Stats
std::string API::Get_24hr_Stats()
{
  std::string url = "/products/" + product_id + "/stats";
  std::string res = Call("GET", false, url, "");
  std::cout << res << std::endl;
  return "";
}

//Get Currencies
std::string API::Get_Currencies()
{
  std::string url = "/currencies/";
  std::string res = Call("GET", false, url, "");
  std::cout << res << std::endl;
  return "";
}

//Get Currency
std::string API::Get_Currency(std::string cid)
{
  std::string url = "/currencies/" + cid;
  std::string res = Call("GET", false, url, "");
  std::cout << res << std::endl;
  return "";
}

//Time
//Get Time
std::string API::Get_Time()
{
  std::string url = "/time";
  std::string res = Call("GET", false, url, "");
  std::cout << res << std::endl;
  return "";
}

//Fills
//List Fills
std:: string API::List_Fills(std::string order_id, std::string product_id)
{
  std::string url = "/fills?";
  if(order_id != ""){
    url += "order_id=" + order_id + "&";
  }
  if(product_id != ""){
    url += "product_id=" + product_id;
  }
  std::string res = Call("GET", true, url, "");
  std::cout << res << std::endl;
  return "";
}

//Payment Methods
//List_Payment_Methods
std::string API::List_Payment_Methods()
{
  std::string url = "/payment-methods";
  std::string res = Call("GET", true, url, "");
  std::cout << res << std::endl;
  return "";
}

//Coinbase Accounts
//List Coinbase Accounts
std::string API::List_Coinbase_Accounts()
{
  std::string url = "/coinbase-accounts";
  std::string res = Call("GET", true, url, "");
  std::cout << res << std::endl;
  return "";
}

//Limits
//Get Current Exchange Limits
std::string API::Get_Current_Exchange_Limits()
{
  std::string url = "/users/self/exchange-limits";
  std::string res = Call("GET", true, url, "");
  std::cout << res << std::endl;
  return "";
}

//Deposit
//List Deposits
std::string API::List_Deposits(std::string profile_id, std::string before, std::string after, std::string limit)
{
  std::string url = "/transfers?type=deposit";
  if(profile_id != ""){
    url += "profile_id=" + profile_id + "&";
  }
  if(before != ""){
    url += "before=" + before + "&";
  }
  if(after != ""){
    url += "after=" + after + "&";
  }
  if(limit != ""){
    url += "limit=" + limit;
  }
  std::string res = Call("GET", true, url, "");
  std::cout << res << std::endl;
  return "";
}

//Single Deposit
std::string API::Single_Deposit(std::string transfer_id)
{
  std::string url = "/transfers/:" + transfer_id;
  std::string res = Call("GET", true, url, "");
  std::cout << res << std::endl;
  return "";
}

//Payment method
std::string API::Payment_Method_Deposit(std::string amount, std::string currency, std::string payment_method_id)
{
  Document d;
  d.SetObject();
  rapidjson::Document::AllocatorType &allocator = d.GetAllocator();

  /* The amount to deposit */
  Value v_amount;
  v_amount = StringRef(amount.c_str());
  d.AddMember("amount", v_amount, allocator);
  
  /* The type of currency */
  Value v_currency;
  v_currency = StringRef(currency.c_str());
  d.AddMember("currency", v_currency, allocator);

  /* ID of the payment method */
  Value v_payment_method_id;
  v_payment_method_id = StringRef(payment_method_id.c_str());
  d.AddMember("payment_method_id", v_payment_method_id, allocator);

  /* creates the string in json */
  StringBuffer strbuf;
  Writer<StringBuffer> writer(strbuf);
  d.Accept(writer);
  std::string res = Call("POST", true, "/deposits/payment-method", strbuf.GetString());
  std::cout << res ;
  return "";
}

//Coinbase Deposit
std::string API::Coinbase_Deposit(std::string amount, std::string currency, std::string coinbase_account_id)
{
  Document d;
  d.SetObject();
  rapidjson::Document::AllocatorType &allocator = d.GetAllocator();

  /* The amount to deposit */
  Value v_amount;
  v_amount = StringRef(amount.c_str());
  d.AddMember("amount", v_amount, allocator);
  
  /* The type of currency */
  Value v_currency;
  v_currency = StringRef(currency.c_str());
  d.AddMember("currency", v_currency, allocator);

  /* ID of the coinbase_account */
  Value v_coinbase_account_id;
  v_coinbase_account_id = StringRef(coinbase_account_id.c_str());
  d.AddMember("coinbase_account_id", v_coinbase_account_id, allocator);

  /* creates the string in json */
  StringBuffer strbuf;
  Writer<StringBuffer> writer(strbuf);
  d.Accept(writer);
  std::string res = Call("POST", true, "/deposits/coinbase-account", strbuf.GetString());
  std::cout << res ;
  return "";
}

//Generate a Crypto Deposit Address
std::string API::Generate_Crypto_Deposit_Address(std::string coinbase_account_id)
{

  std::string res = Call("POST", true, "/coinbase-accounts/" + coinbase_account_id + "/addresses", "");
  std::cout << res ;
  return "";
}

//Withdraw
//List Withdrawals
std::string API::List_Withdrawals(std::string profile_id, std::string before, std::string after, std::string limit)
{
  std::string url = "/transfers?type=withdraw";
  if(profile_id != ""){
    url += "profile_id=" + profile_id + "&";
  }
  if(before != ""){
    url += "before=" + before + "&";
  }
  if(after != ""){
    url += "after=" + after + "&";
  }
  if(limit != ""){
    url += "limit=" + limit;
  }
  std::string res = Call("GET", true, url, "");
  std::cout << res << std::endl;
  return "";
}

//Single Withdrawal
std::string API::Single_Withdrawal(std::string transfer_id)
{
  std::string url = "/transfers/:" + transfer_id;
  std::string res = Call("GET", true, url, "");
  std::cout << res << std::endl;
  return "";
}

//Payment method
std::string API::Payment_Method_Withdraw(std::string amount, std::string currency, std::string payment_method_id)
{
  Document d;
  d.SetObject();
  rapidjson::Document::AllocatorType &allocator = d.GetAllocator();

  /* The amount to deposit */
  Value v_amount;
  v_amount = StringRef(amount.c_str());
  d.AddMember("amount", v_amount, allocator);
  
  /* The type of currency */
  Value v_currency;
  v_currency = StringRef(currency.c_str());
  d.AddMember("currency", v_currency, allocator);

  /* ID of the payment method */
  Value v_payment_method_id;
  v_payment_method_id = StringRef(payment_method_id.c_str());
  d.AddMember("payment_method_id", v_payment_method_id, allocator);

  /* creates the string in json */
  StringBuffer strbuf;
  Writer<StringBuffer> writer(strbuf);
  d.Accept(writer);
  std::string res = Call("POST", true, "/withdrawals/payment-method", strbuf.GetString());
  std::cout << res ;
  return "";
}

//Coinbase Withdraw
std::string API::Coinbase_Withdraw(std::string amount, std::string currency, std::string coinbase_account_id)
{
  Document d;
  d.SetObject();
  rapidjson::Document::AllocatorType &allocator = d.GetAllocator();

  /* The amount to deposit */
  Value v_amount;
  v_amount = StringRef(amount.c_str());
  d.AddMember("amount", v_amount, allocator);
  
  /* The type of currency */
  Value v_currency;
  v_currency = StringRef(currency.c_str());
  d.AddMember("currency", v_currency, allocator);

  /* ID of the coinbase_account */
  Value v_coinbase_account_id;
  v_coinbase_account_id = StringRef(coinbase_account_id.c_str());
  d.AddMember("coinbase_account_id", v_coinbase_account_id, allocator);

  /* creates the string in json */
  StringBuffer strbuf;
  Writer<StringBuffer> writer(strbuf);
  d.Accept(writer);
  std::string res = Call("POST", true, "/withdrawals/coinbase-account", strbuf.GetString());
  std::cout << res ;
  return "";
}

//Crytop Withdraw
std::string API::Crypto_Withdraw(std::string amount, std::string currency, std::string crypto_address, std::string destination_tag, std::string no_destination_tag, std::string add_network_fee_to_total)
{
  Document d;
  d.SetObject();
  rapidjson::Document::AllocatorType &allocator = d.GetAllocator();

  /* The amount to deposit */
  Value v_amount;
  v_amount = StringRef(amount.c_str());
  d.AddMember("amount", v_amount, allocator);
  
  /* The type of currency */
  Value v_currency;
  v_currency = StringRef(currency.c_str());
  d.AddMember("currency", v_currency, allocator);

  /* A crypto address of the recipient */
  Value v_crypto_address;
  v_crypto_address = StringRef(crypto_address.c_str());
  d.AddMember("crypto_address", v_crypto_address, allocator);

  /* A destination tag for currencies that support one */
  Value v_destination_tag;
  v_destination_tag = StringRef(destination_tag.c_str());
  d.AddMember("destination_tag", v_destination_tag, allocator);

  /* A boolean flag to opt out of using a destination tag for currencies that support one. This is required when not providing a destination tag. */
  Value v_no_destination_tag;
  v_no_destination_tag = StringRef(no_destination_tag.c_str());
  d.AddMember("no_destination_tag", v_no_destination_tag, allocator);

  /* A boolean flag to add the network fee on top of the amount. If this is blank, it will default to deducting the network fee from the amount. */
  Value v_add_network_fee_to_total;
  v_add_network_fee_to_total = StringRef(add_network_fee_to_total.c_str());
  d.AddMember("add_network_fee_to_total", v_add_network_fee_to_total, allocator);

  /* creates the string in json */
  StringBuffer strbuf;
  Writer<StringBuffer> writer(strbuf);
  d.Accept(writer);
  std::string res = Call("POST", true, "/withdrawals/crypto", strbuf.GetString());
  std::cout << res ;
  return "";
}

//Fee Estimate
std::string API::Fee_Estimate(std::string currency, std::string crypto_address)
{
  std::string url = "/withdrawals/fee-estimate?currency=" + currency + "&crypto_address" + crypto_address;
  std::string res = Call("GET", true, url, "");
  std::cout << res << std::endl;
  return "";
}

//Stablecoin Conversions
//Create Conversion
std::string API::Create_Conversion(std::string from, std::string to, std::string amount)
{
  Document d;
  d.SetObject();
  rapidjson::Document::AllocatorType &allocator = d.GetAllocator();

  /* A valid currency id */
  Value v_from;
  v_from = StringRef(from.c_str());
  d.AddMember("from", v_from, allocator);
  
  /* A valid currency id */
  Value v_to;
  v_to = StringRef(to.c_str());
  d.AddMember("to", v_to, allocator);

  /* Amount of from to convert to to, current limit is 10,000,000 */
  Value v_amount;
  v_amount = StringRef(amount.c_str());
  d.AddMember("amount", v_amount, allocator);

  /* creates the string in json */
  StringBuffer strbuf;
  Writer<StringBuffer> writer(strbuf);
  d.Accept(writer);
  std::string res = Call("POST", true, "/conversions", strbuf.GetString());
  std::cout << res ;
  return "";
}

//Fees
//Get Current Fees
std::string API::Get_Current_Fees()
{
  std::string url = "/fees";
  std::string res = Call("GET", true, url, "");
  std::cout << res << std::endl;
  return "";
}

//Reports
//Create a new report
std::string API::Create_new_report(std::string type, std::string start_date, std::string end_date, std::string product_id, std::string account_id, std::string format, std::string email)
{
  Document d;
  d.SetObject();
  rapidjson::Document::AllocatorType &allocator = d.GetAllocator();

  /* fills or account */
  Value v_type;
  v_type = StringRef(type.c_str());
  d.AddMember("type", v_type, allocator);
  
  /* Starting date for the report (inclusive) */
  Value v_start_date;
  v_start_date = StringRef(start_date.c_str());
  d.AddMember("start_date", v_start_date, allocator);

  /* Ending date for the report (inclusive) */
  Value v_end_date;
  v_end_date = StringRef(end_date.c_str());
  d.AddMember("end_date", v_end_date, allocator);

  /* ID of the product to generate a fills report for. E.g. BTC-USD. Required if type is fills */
  Value v_product_id;
  v_product_id = StringRef(product_id.c_str());
  d.AddMember("product_id", v_product_id, allocator);

  /* ID of the account to generate an account report for. Required if type is account */
  Value v_account_id;
  v_account_id = StringRef(account_id.c_str());
  d.AddMember("account_id", v_account_id, allocator);

  /* pdf or csv (defualt is pdf) */
  Value v_format;
  v_format = StringRef(format.c_str());
  d.AddMember("format", v_format, allocator);

  /* Email address to send the report to (optional) */
  Value v_email;
  v_email = StringRef(email.c_str());
  d.AddMember("email", v_email, allocator);

  /* creates the string in json */
  StringBuffer strbuf;
  Writer<StringBuffer> writer(strbuf);
  d.Accept(writer);
  std::string res = Call("POST", true, "/reports", strbuf.GetString());
  std::cout << res ;
  return "";
}

//Get report status
std::string API::Get_report_status(std::string report_id)
{
  std::string url = "/reports/:" + report_id;
  std::string res = Call("GET", true, url, "");
  std::cout << res << std::endl;
  return "";
}

//List Profiles
std::string API::List_Profiles(std::string active)
{
  std::string url = "/profiles";
  std::string res = Call("GET", true, url, "");
  std::cout << res << std::endl;
  return "";
}

//Get a Profile
std::string API::Get_Profile(std::string profile_id)
{
  std::string url = "/profiles/" + profile_id;
  std::string res = Call("GET", true, url, "");
  std::cout << res << std::endl;
  return "";
}

//Create profile transfer
std::string API::Create_profile_transfer(std::string from, std::string to, std::string currency, std::string amount)
{
  Document d;
  d.SetObject();
  rapidjson::Document::AllocatorType &allocator = d.GetAllocator();

  /* The profile id the API key belongs to and where the funds are sourced */
  Value v_from;
  v_from = StringRef(from.c_str());
  d.AddMember("from", v_from, allocator);
  
  /* The target profile id of where funds will be transferred to */
  Value v_to;
  v_to = StringRef(to.c_str());
  d.AddMember("to", v_to, allocator);

  /* i.e. BTC or USD */
  Value v_currency;
  v_currency = StringRef(currency.c_str());
  d.AddMember("currency", v_currency, allocator);

  /* Amount of currency to be transferred */
  Value v_amount;
  v_amount = StringRef(amount.c_str());
  d.AddMember("amount", v_amount, allocator);

  /* creates the string in json */
  StringBuffer strbuf;
  Writer<StringBuffer> writer(strbuf);
  d.Accept(writer);
  std::string res = Call("POST", true, "/profiles/transfer", strbuf.GetString());
  std::cout << res ;
  return "";
}

//User Account
//Trailing Volume
std::string API::Trailing_Volume()
{
  std::string url = "/users/self/trailing-volume";
  std::string res = Call("GET", true, url, "");
  std::cout << res << std::endl;
  return "";
}


















API::API()
{
  curl_global_init(CURL_GLOBAL_DEFAULT);
}

API::~API()
{
  curl_global_cleanup();
}
