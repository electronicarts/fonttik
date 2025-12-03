//Copyright (C) 2022-2025 Electronic Arts, Inc.  All rights reserved.

#include "fonttik/Results.h"

tik::ResultType tik::ResultTypeMerge(const ResultType a, const ResultType b)
{
	switch (a)
	{
	case PASS:
		return b;
	case FAIL:
		return FAIL;
	case WARNING:
		return (b == FAIL) ? FAIL : WARNING;
	default:
		break;
	}
}

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