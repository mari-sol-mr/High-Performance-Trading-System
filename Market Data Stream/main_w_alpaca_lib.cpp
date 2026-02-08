#include <cstdio>
#include <cstdlib>
#include <iostream>
// #include "alpaca/config.h"
// #include "alpaca/streaming.h"
#include "alpaca/foo.h"
#include "rapidjson/document.h"

int main(int argc, const char **argv) {
    printf("Hello\n");
    std::cout << fooFunc();
    // auto env = alpaca::Environment();
    // std::cout << env.();
//   if (auto status = env.parse(); !status.ok()) {
//     std::cerr << "Error parsing environment: " << status.getMessage() << std::endl;
//     return status.getCode();
//   }
  printf("Hello again\n");

//   // Log trade updates
//   std::function<void(alpaca::stream::DataType data)> on_trade_update = [=](alpaca::stream::DataType data) {
//     // rapidjson::Document d;
//     // d.Parse(data.c_str());
//     // rapidjson::StringBuffer s;
//     // rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(s);
//     // d.Accept(writer);
//     // std::cout << "Got trade update: " << s.GetString() << std::endl;
//   };

//   // Log account updates
//   std::function<void(alpaca::stream::DataType data)> on_account_update = [=](alpaca::stream::DataType data) {
//     // rapidjson::Document d;
//     // d.Parse(data.c_str());
//     // rapidjson::StringBuffer s;
//     // rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(s);
//     // d.Accept(writer);
//     // std::cout << "Got account update: " << s.GetString() << std::endl;
//   };
// auto handler = alpaca::stream::Handler(on_trade_update, on_account_update);
//    

//   if (auto status = handler.run(env); !status.ok()) {
//     std::cerr << "Error running stream handler: " << status.getMessage() << std::endl;
//     return status.getCode();
//   }
    return EXIT_SUCCESS;
}