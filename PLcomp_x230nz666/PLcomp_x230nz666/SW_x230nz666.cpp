// SW_x230nz666.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "CLexicalParser.h"


int main(int argc, wchar_t *argv[])
{
	CLexicalParser lParser(std::_Ptr_wcin);
	CLexicalParser::SymbolType sym;
	while (lParser.Next())
	{
		switch (lParser.GetSymbolType())
		{
		case CLexicalParser::int_datatype:
			wprintf(L"%s -> %I64d\n", lParser.GetSymbol(), lParser.GetInteger());
			
			break;
		case CLexicalParser::dec_datatype:
			wprintf(L"%s -> %f\n", lParser.GetSymbol(), lParser.GetDecimal());
			break;
		default:
			break;
		}
	}
	
    return 0;
}

