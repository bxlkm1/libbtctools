//
// echo_server.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2016 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "tcpclient/all.hpp"
#include "utils/IpGenerator.hpp"
#include <boost/regex.hpp>
#include <windows.h>

using namespace std;
using namespace btctools::tcpclient;
using namespace btctools::utils;

int main(int argc, char* argv[])
{
	try
	{
		Client client(5);

		ResponseConsumer responseConsumer(
			[&](ResponseProductor & responseProductor)
		{
			RequestProductor requestProductor(
				[&](RequestConsumer &requestConsumer)
			{
				client.run(requestConsumer, responseProductor);
			});

			StringConsumer ipSource(
				[](StringProductor &ipYield)
			{
				IpGenerator::genIpRange("192.168.20.0-192.168.21.255", ipYield);
			});
			
			for (auto ip : ipSource)
			{
				//cout << ip << endl;

				// use CGMiner RPC: https://github.com/ckolivas/cgminer/blob/master/API-README
				// the response of JSON styled calling {"command":"stats"} will responsed
				// a invalid JSON string from Antminer S9, so call with plain text style.
				Request *req = new Request(ip, "4028", "stats|");
				req->usrdata_ = req;

				requestProductor(req);
			}
		});

		for (auto response : responseConsumer)
		{
			Request *request = (Request *)response->usrdata_;

			if (response->error_code_ == boost::asio::error::eof)
			{
				string minerType = "Unknown";

				// Only Antminer has the field.
				boost::regex expression(",Type=([^\\|]+)\\|");
				boost::smatch what;

				if (boost::regex_search(response->content_, what, expression))
				{
					minerType = what[1];
				}
				/*else
				{
					minerType = response->content_;
				}*/

				cout << request->host_ << "��" << minerType << endl;
			}
			else
			{
				cout << request->host_ << ": " << boost::system::system_error(response->error_code_).what() << endl;
			}

			//delete request;
			delete response;
		}

	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	std::cout << "\nDone" << std::endl;

	::system("pause");

	return 0;
}