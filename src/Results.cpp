//Copyright (C) 2022-2025 Electronic Arts, Inc.  All rights reserved.
#include "fonttik/Results.h"
std::string tik::ResultTypeAsString(ResultType t)
{
	 switch (t)
	 {
	 case tik::PASS:
		 return "PASS";
	 case tik::FAIL:
		 return "FAIL";
	 case tik::WARNING:
		 return "WARNING";
	 case tik::UNRECOGNIZED:
		 return "UNRECOGNIZED";
	 default:
		 throw std::runtime_error("unreachable");
		 break;
	 }
}