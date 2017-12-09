// SW_x230nz666.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "error_messages.h"
#include "sw_symHeader.h"
#include "CSWStructure.h"

ProgramSymSyntax program;
DeclarationListSymSyntax declarationListSymSyntax;
VarDeclarationListSymSyntax varDeclarationListSymSyntax;
VarDeclarationSymSyntax varDeclarationSymSyntax;
VarIdentDeclarationSymSyntax varIdentDeclarationSymSyntax;
ConstDeclarataionListSymSyntax constDeclarataionListSymSyntax;
ConstDeclarationSymSyntax constDeclarationSymSyntax;
ConstIdentDeclarationSymSyntax constIdentDeclarationSymSyntax;
FunctionDeclarationListSymSyntax functionDeclarationListSymSyntax;
ParamIdentDeclarationSymSyntax paramIdentDeclarationSymSyntax;
FunctionBodySymSyntax functionBodySymSyntax;
StatementListSymSyntax statementListSymSyntax;
StatementSymSyntax statementSymSyntax;
IfStatSymSyntax ifStatSymSyntax;
WhileStatSymSyntax whileStatSymSyntax;
RepeatStatSymSyntax repeatStatSymSyntax;
ReadStatSymSyntax readStatSymSyntax;
PrintStatSymSyntax printStatSymSyntax;
AssignStatSymSyntax assignStatSymSyntax;
ForStatSymSyntax forStatSymSyntax;
CallStatSymSyntax callStatSymSyntax;
ConditionSymSyntax conditionSymSyntax;
ExpressionSymSyntax expressionSymSyntax;
TermSymSyntax termSymSyntax;
FactorSymSyntax factorSymSyntax;

std::vector<CVirtualMachineInstruction> outputCodes;
CSWStructure sw_Struct;

int error(int error_code)
{
	printf("%d\n", error_code);
	exit(EXIT_FAILURE);
    return error_code;
}

int main(int argc, wchar_t *argv[])
{
    // the main lexical parser for the whole program	
	CLexicalParser lParser(std::_Ptr_wcin);
	
    // the definition of the Symbol Table
    CSymbolTable main_Table;

	if (lParser.Next())
	{
		ProgramSym(lParser, main_Table);
	}
	return 0;
}

void ProgramSym(CLexicalParser & lParser, CSymbolTable & sTable)
{
    // sw_Struct when created, will automatically push(0,0)
    // sw_Struct.push(0, 0);
    outputCodes.push_back(CVirtualMachineInstruction(VI_BRANCH, 0, 0));
	DeclarationListSym(lParser, sTable);
    outputCodes[sw_Struct.GetCurCodeBase()].param_b = outputCodes.size();
    
    // int 0, count
    outputCodes.push_back(CVirtualMachineInstruction(VI_INITIALIZE_VAR, 0, sw_Struct.GetCurVariableSpace()));
	StatementListSym(lParser, sTable);
    outputCodes.push_back(CVMInst_Return());
}

void DeclarationListSym(CLexicalParser & lParser, CSymbolTable & sTable)
{
	ConstDeclarationListSym(lParser, sTable);
	VarDeclarationListSym(lParser, sTable);
	FunctionDeclarationListSym(lParser, sTable);
}

void VarDeclarationListSym(CLexicalParser & lParser, CSymbolTable & sTable)
{
	while (varDeclarationListSymSyntax.GetFirst().count(lParser.GetSymbolType()) > 0)
	{
		VarDeclarationSym(lParser, sTable);

		if (lParser.GetSymbolType() == semicolon_op)
		{
			lParser.Next();
		}
		else
			error(IDS_ERROR_SYNTAX_DECLARATION_NOENDSEMICOLON);
	}
}

void VarDeclarationSym(CLexicalParser & lParser, CSymbolTable & sTable)
{
	if (lParser.GetSymbolType() == var_sym)
	{
		lParser.Next();
	}
	else
		error(IDS_ERROR_SYNTAX_UNEXPECTED);

	VarIdentDeclarationSym(lParser, sTable);

	while (lParser.GetSymbolType() == comma_op)
	{
		lParser.Next();
		VarIdentDeclarationSym(lParser, sTable);
	}
}

void VarIdentDeclarationSym(CLexicalParser & lParser, CSymbolTable & sTable)
{
    std::wstring strIdent;
	if (lParser.GetSymbolType() == ident)
	{
        strIdent = lParser.GetSymbol();
		lParser.Next();
        
        CVariableSymbol * newVari = new CVariableSymbol();
        newVari->SetName(strIdent);
        newVari->SetStatus(SymbolStatus(SDT_INTEGER));
        newVari->SetLevel(sw_Struct.GetCurLevel());
        newVari->SetOffset(3 + (int)sw_Struct.GetCurVariableSpace());
        sw_Struct.AppendSpace(1); //add 1 space for use.
        
        if (!sTable.EnterSymbol(newVari))
        {
            error(IDS_ERROR_SYNTAX_DECLARATION_NAMEREPEAT);
        }
	}
	else
		error(IDS_ERROR_SYNTAX_DECLARATION_MISSIDENT);

	if (lParser.GetSymbolType() == eql_op)
	{
        lParser.Next();
        //TODO: Complete the Variable Initialize operation.
		ExpressionSym(lParser, sTable);
	}
}

void ConstDeclarationListSym(CLexicalParser & lParser, CSymbolTable & sTable)
{
	while (constDeclarationSymSyntax.GetFirst().count(lParser.GetSymbolType()) > 0)
	{
		ConstDeclarationSym(lParser, sTable);

		if (lParser.GetSymbolType() == semicolon_op)
		{
			lParser.Next();
		}
		else
			error(IDS_ERROR_SYNTAX_DECLARATION_NOENDSEMICOLON);
	}
}

void ConstDeclarationSym(CLexicalParser & lParser, CSymbolTable & sTable)
{
	if (lParser.GetSymbolType() == const_sym)
	{
		lParser.Next();
	}
	else
		error(IDS_ERROR_SYNTAX_UNEXPECTED);

	ConstIdentDeclarationSym(lParser, sTable);

	while (lParser.GetSymbolType() == comma_op)
	{
		lParser.Next();
		ConstIdentDeclarationSym(lParser, sTable);
	}
}

void ConstIdentDeclarationSym(CLexicalParser & lParser, CSymbolTable & sTable)
{
    std::wstring strIdent;
	if (lParser.GetSymbolType() == ident)
	{
        strIdent = lParser.GetSymbol();
		lParser.Next();
	}
	else
		error(IDS_ERROR_SYNTAX_DECLARATION_MISSIDENT);

	if (lParser.GetSymbolType() == eql_op)
	{
		lParser.Next();
	}
	else
		error(IDS_ERROR_SYNTAX_DECLARATION_CONST_NOEQL);

    // TODO: 还没有处理好，如何判断 CONSTANT 后面是不是 constexpr 的问题
    // 处理好以后，才可以将 CONSTANT 加入符号表
	ExpressionSym(lParser, sTable, true);
}

void FunctionDeclarationListSym(CLexicalParser & lParser, CSymbolTable & sTable)
{
	while (lParser.GetSymbolType() == func_sym)
	{
		lParser.Next();

        std::wstring symbolName;
		if (lParser.GetSymbolType() == ident)
		{
            symbolName = lParser.GetSymbol();
            lParser.Next();
            CFunctionSymbol *newFunction = new CFunctionSymbol();
            newFunction->SetName(symbolName);
            newFunction->SetLevel(sw_Struct.GetCurLevel());

            // save the current code pos, as the function's address
            newFunction->SetOffset((int)outputCodes.size());
            
            if (!sTable.EnterSymbol(newFunction))
            {
                error(IDS_ERROR_SYNTAX_DECLARATION_NAMEREPEAT);
            }
		}
		else
			error(IDS_ERROR_SYNTAX_DECLARATION_MISSIDENT);

        // push, before params are generated
        sw_Struct.push(sTable.GetTableSize(), outputCodes.size());

		if (lParser.GetSymbolType() == lparen_op)
		{
			lParser.Next();
		}
		else
			error(IDS_ERROR_SYNTAX_DECLARATION_FUNC_NOLPAREN);
		
		if (paramIdentDeclarationSymSyntax.GetFirst().count(lParser.GetSymbolType()) > 0)
		{
			ParamIdentDeclarationSym(lParser, sTable);

			while (lParser.GetSymbolType() == comma_op)
			{
				lParser.Next();
				ParamIdentDeclarationSym(lParser, sTable);
			}
		}

		if (lParser.GetSymbolType() == rparen_op)
		{
			lParser.Next();
		}
		else
			error(IDS_ERROR_SYNTAX_DECLARATION_FUNC_NORPAREN);

		if (lParser.GetSymbolType() == lbrace_op)
		{
			lParser.Next();
		}
		else
			error(IDS_ERROR_SYNTAX_DECLARATION_FUNC_NOLBRACE);

		FunctionBodySym(lParser, sTable);
        sw_Struct.pop();

        // TODO: 因为 pop 了，所以要把 sTable 中高于当前级别的符号全部删掉

		if (lParser.GetSymbolType() == rbrace_op)
		{
			lParser.Next();
		}
		else
			error(IDS_ERROR_SYNTAX_DECLARATION_FUNC_NORBRACE);
	}
}

void ParamIdentDeclarationSym(CLexicalParser & lParser, CSymbolTable & sTable)
{
    // first we need to fetch,
    // how many params now already existed.
    CFunctionSymbol *lastFunc = sTable.GetLastFunction();
    if (!lastFunc)
        error(IDS_ERROR_SYNTAX_DECLARATION_REF_NOFUNCTION);
    size_t param_count = lastFunc->GetParams().size();

    CVariableSymbol *newSymbol = new CVariableSymbol();
    
    // only integer type, no others type
    newSymbol->_Status().dateType = SDT_INTEGER;
    newSymbol->SetLevel(sw_Struct.GetCurLevel());
    newSymbol->SetOffset(-1 - (int)param_count);
	if (lParser.GetSymbolType() == ref_sym)
    {
        newSymbol->_Status().isRef = true;
		lParser.Next();
	}

    // we need to "register" the current symbol status
    // into the last function.
    lastFunc->AppendParam(newSymbol->GetStatus());

	if (lParser.GetSymbolType() == ident)
	{
        newSymbol->SetName(lParser.GetSymbol());
		lParser.Next();
	}
	else
		error(IDS_ERROR_SYNTAX_DECLARATION_MISSIDENT);
    
    if (!sTable.EnterSymbol(newSymbol))
    {
        error(IDS_ERROR_SYNTAX_DECLARATION_NAMEREPEAT);
    }
}

void FunctionBodySym(CLexicalParser & lParser, CSymbolTable & sTable)
{
    outputCodes.push_back(CVirtualMachineInstruction(VI_BRANCH, 0, 0));
    VarDeclarationListSym(lParser, sTable);
    outputCodes[sw_Struct.GetCurCodeBase()].param_b = outputCodes.size();
    outputCodes.push_back(CVirtualMachineInstruction(VI_INITIALIZE_VAR, 0, sw_Struct.GetCurVariableSpace()));
	StatementListSym(lParser, sTable);
    outputCodes.push_back(CVMInst_Return());
}

void StatementListSym(CLexicalParser & lParser, CSymbolTable & sTable)
{
	while (statementSymSyntax.GetFirst().count(lParser.GetSymbolType()) > 0)
	{
		StatementSym(lParser, sTable);
		if (lParser.GetSymbolType() == semicolon_op)
		{
			lParser.Next();
		}
		else
			error(IDS_ERROR_SYNTAX_STATEMENT_NOENDSEMICOLON);
	}
}

void StatementSym(CLexicalParser & lParser, CSymbolTable & sTable)
{
	if (lParser.GetSymbolType() == continue_sym)
	{
		lParser.Next();
	}
	else if (lParser.GetSymbolType() == break_sym)
	{
		lParser.Next();
	}
	else if (ifStatSymSyntax.GetFirst().count(lParser.GetSymbolType()) > 0)
	{
		IfStatSym(lParser, sTable);
	}
	else if (whileStatSymSyntax.GetFirst().count(lParser.GetSymbolType()) > 0)
	{
		WhileStatSym(lParser, sTable);
	}
	else if (repeatStatSymSyntax.GetFirst().count(lParser.GetSymbolType()) > 0)
	{
		RepeatStatSym(lParser, sTable);
	}
	else if (readStatSymSyntax.GetFirst().count(lParser.GetSymbolType()) > 0)
	{
		ReadStatSym(lParser, sTable);
	}
	else if (printStatSymSyntax.GetFirst().count(lParser.GetSymbolType()) > 0)
	{
		PrintStatSym(lParser, sTable);
	}
	else if (assignStatSymSyntax.GetFirst().count(lParser.GetSymbolType()) > 0)
	{
		AssignStatSym(lParser, sTable);
	}
	else if (forStatSymSyntax.GetFirst().count(lParser.GetSymbolType()) > 0)
	{
		ForStatSym(lParser, sTable);
	}
	else if (callStatSymSyntax.GetFirst().count(lParser.GetSymbolType()) > 0)
	{
		CallStatSym(lParser, sTable);
	}
}

void IfStatSym(CLexicalParser & lParser, CSymbolTable & sTable)
{
	if (lParser.GetSymbolType() == if_sym)
	{
		lParser.Next();
	}
	else if (lParser.GetSymbolType() == unless_sym)
	{
		lParser.Next();
	}
	else
		error(IDS_ERROR_SYNTAX_UNEXPECTED);

	ConditionSym(lParser, sTable);

	if (lParser.GetSymbolType() == lbrace_op)
	{
		lParser.Next();
	}
	else
		error(IDS_ERROR_SYNTAX_STATEMENT_NOLBRACE);

	StatementListSym(lParser, sTable);

	if (lParser.GetSymbolType() == rbrace_op)
	{
		lParser.Next();
	}
	else
		error(IDS_ERROR_SYNTAX_STATEMENT_NORBRACE);

	if (lParser.GetSymbolType() == else_sym)
	{
		lParser.Next();

		if (lParser.GetSymbolType() == lbrace_op)
		{
			lParser.Next();
		}
		else
			error(IDS_ERROR_SYNTAX_STATEMENT_NOLBRACE);

		StatementListSym(lParser, sTable);

		if (lParser.GetSymbolType() == rbrace_op)
		{
			lParser.Next();
		}
		else
			error(IDS_ERROR_SYNTAX_STATEMENT_NORBRACE);
	}
}

void WhileStatSym(CLexicalParser & lParser, CSymbolTable & sTable)
{
	if (lParser.GetSymbolType() == while_sym)
	{
		lParser.Next();
	}
	else if (lParser.GetSymbolType() == until_sym)
	{
		lParser.Next();
	}
	else
		error(IDS_ERROR_SYNTAX_UNEXPECTED);

	ConditionSym(lParser, sTable);

	if (lParser.GetSymbolType() == lbrace_op)
	{
		lParser.Next();
	}
	else
		error(IDS_ERROR_SYNTAX_STATEMENT_NOLBRACE);

	StatementListSym(lParser, sTable);

	if (lParser.GetSymbolType() == rbrace_op)
	{
		lParser.Next();
	}
	else
		error(IDS_ERROR_SYNTAX_STATEMENT_NORBRACE);
}

void RepeatStatSym(CLexicalParser & lParser, CSymbolTable & sTable)
{
	if (lParser.GetSymbolType() == repeat_sym)
	{
		lParser.Next();
	}
	else
		error(IDS_ERROR_SYNTAX_UNEXPECTED);

	if (lParser.GetSymbolType() == lbrace_op)
	{
		lParser.Next();
	}
	else
		error(IDS_ERROR_SYNTAX_STATEMENT_NOLBRACE);

	StatementListSym(lParser, sTable);

	if (lParser.GetSymbolType() == rbrace_op)
	{
		lParser.Next();
	}
	else
		error(IDS_ERROR_SYNTAX_STATEMENT_NORBRACE);
}

void ReadStatSym(CLexicalParser & lParser, CSymbolTable & sTable)
{
	if (lParser.GetSymbolType() == read_sym)
	{
		lParser.Next();
	}
	else
		error(IDS_ERROR_SYNTAX_UNEXPECTED);

	if (lParser.GetSymbolType() == lparen_op)
	{
		lParser.Next();
	}
	else
		error(IDS_ERROR_SYNTAX_STATEMENT_NOLPAREN);

	if (lParser.GetSymbolType() == ident)
	{
		lParser.Next();
	}
	else
		error(IDS_ERROR_SYNTAX_STATEMENT_MISSIDENT);

	if (lParser.GetSymbolType() == rparen_op)
	{
		lParser.Next();
	}
	else
		error(IDS_ERROR_SYNTAX_STATEMENT_NORPAREN);
}

void PrintStatSym(CLexicalParser & lParser, CSymbolTable & sTable)
{
	if (lParser.GetSymbolType() == print_sym)
	{
		lParser.Next();
	}
	else
		error(IDS_ERROR_SYNTAX_UNEXPECTED);

	if (lParser.GetSymbolType() == lparen_op)
	{
		lParser.Next();
	}
	else
		error(IDS_ERROR_SYNTAX_STATEMENT_NOLPAREN);

	if (lParser.GetSymbolType() == ident)
	{
		lParser.Next();
	}
	else
		error(IDS_ERROR_SYNTAX_STATEMENT_MISSIDENT);

	if (lParser.GetSymbolType() == rparen_op)
	{
		lParser.Next();
	}
	else
		error(IDS_ERROR_SYNTAX_STATEMENT_NORPAREN);
}

void AssignStatSym(CLexicalParser & lParser, CSymbolTable & sTable)
{
	if (lParser.GetSymbolType() == ident)
	{
		lParser.Next();
	}
	else
		error(IDS_ERROR_SYNTAX_UNEXPECTED);

	if (lParser.GetSymbolType() == eql_op)
	{
		lParser.Next();
		ExpressionSym(lParser, sTable);
	}
	else if (lParser.GetSymbolType() == dblplus_op)
	{
		lParser.Next();
	}
	else if (lParser.GetSymbolType() == dblminus_op)
	{
		lParser.Next();
	}
	else
		error(IDS_ERROR_SYNTAX_ASSIGN_UNEXPECTED);
}

void ForStatSym(CLexicalParser & lParser, CSymbolTable & sTable)
{
	// for
	if (lParser.GetSymbolType() == for_sym)
	{
		lParser.Next();
	}
	else
		error(IDS_ERROR_SYNTAX_UNEXPECTED);

	// ident
	if (lParser.GetSymbolType() == ident)
	{
		lParser.Next();
	}
	else
		error(IDS_ERROR_SYNTAX_STATEMENT_MISSIDENT);

	// in
	if (lParser.GetSymbolType() == in_sym)
	{
		lParser.Next();
	}
	else
		error(IDS_ERROR_SYNTAX_FOR_MISSIN);

	if (lParser.GetSymbolType() == number)
	{
		lParser.Next();
	}
	else if (lParser.GetSymbolType() == dec_val)
	{
		error(IDS_ERROR_LEXICAL_NODECIMAL);
	}
	else if (lParser.GetSymbolType() == ident)
	{
		lParser.Next();
	}
	else
		error(IDS_ERROR_SYNTAX_FOR_WRONGTYPE);

	if (lParser.GetSymbolType() == to_op)
	{
		lParser.Next();
	}
	else
		error(IDS_ERROR_SYNTAX_FOR_MISSTO);

	if (lParser.GetSymbolType() == number)
	{
		lParser.Next();
	}
	else if (lParser.GetSymbolType() == dec_val)
	{
		error(IDS_ERROR_LEXICAL_NODECIMAL);
	}
	else if (lParser.GetSymbolType() == ident)
	{
		lParser.Next();
	}
	else
		error(IDS_ERROR_SYNTAX_FOR_WRONGTYPE); 

	if (lParser.GetSymbolType() == lbrace_op)
	{
		lParser.Next();
	}
	else
		error(IDS_ERROR_SYNTAX_STATEMENT_NOLBRACE);

	StatementListSym(lParser, sTable);

	if (lParser.GetSymbolType() == rbrace_op)
	{
		lParser.Next();
	}
	else
		error(IDS_ERROR_SYNTAX_STATEMENT_NORBRACE);
}

void CallStatSym(CLexicalParser & lParser, CSymbolTable & sTable)
{
	if (lParser.GetSymbolType() == call_sym)
	{
		lParser.Next();
	}
	else
		error(IDS_ERROR_SYNTAX_UNEXPECTED);

	if (lParser.GetSymbolType() == ident)
	{
		lParser.Next();
	}
	else
		error(IDS_ERROR_SYNTAX_STATEMENT_MISSIDENT);

	if (lParser.GetSymbolType() == lparen_op)
	{
		lParser.Next();
	}
	else
		error(IDS_ERROR_SYNTAX_STATEMENT_NOLPAREN);

	if (expressionSymSyntax.GetFirst().count(lParser.GetSymbolType()) > 0)
	{
		ExpressionSym(lParser, sTable);

		while (lParser.GetSymbolType() == comma_op)
		{
			lParser.Next();
			ExpressionSym(lParser, sTable);
		}
	}

	if (lParser.GetSymbolType() == rparen_op)
	{
		lParser.Next();
	}
	else
		error(IDS_ERROR_SYNTAX_STATEMENT_NORPAREN);

}

void ConditionSym(CLexicalParser & lParser, CSymbolTable & sTable)
{
	if (lParser.GetSymbolType() == odd_sym)
	{
		lParser.Next();
		if (lParser.GetSymbolType() == lparen_op)
		{
			lParser.Next();
		}
		else
			error(IDS_ERROR_SYNTAX_STATEMENT_NOLPAREN);

		ExpressionSym(lParser, sTable);
		if (lParser.GetSymbolType() == rparen_op)
		{
			lParser.Next();
		}
		else
			error(IDS_ERROR_SYNTAX_STATEMENT_NORPAREN);
	}
	else
	{
		ExpressionSym(lParser, sTable);

		if (lParser.GetSymbolType() == eql_op)
		{
			lParser.Next();
		}
		else if (lParser.GetSymbolType() == neq_op)
		{
			lParser.Next();
		}
		else if (lParser.GetSymbolType() == lss_op)
		{
			lParser.Next();
		}
		else if (lParser.GetSymbolType() == leq_op)
		{
			lParser.Next();
		}
		else if (lParser.GetSymbolType() == gtr_op)
		{
			lParser.Next();
		}
		else if (lParser.GetSymbolType() == geq_op)
		{
			lParser.Next();
		}
		else
			error(IDS_ERROR_SYNTAX_CONDITION_MISSREL);
		ExpressionSym(lParser, sTable);
	}
}

void ExpressionSym(CLexicalParser & lParser, CSymbolTable & sTable, bool isConstant)
{
	if (lParser.GetSymbolType() == plus_op ||
		lParser.GetSymbolType() == minus_op)
	{
		lParser.Next();
	}
	TermSym(lParser, sTable);
	while (lParser.GetSymbolType() == plus_op ||
		lParser.GetSymbolType() == minus_op)
	{
		lParser.Next();
		TermSym(lParser, sTable);
	}
}

void TermSym(CLexicalParser & lParser, CSymbolTable & sTable)
{
	FactorSym(lParser, sTable);
	while (lParser.GetSymbolType() == times_op ||
		lParser.GetSymbolType() == slash_op)
	{
		lParser.Next();
		FactorSym(lParser, sTable);
	}
}

void FactorSym(CLexicalParser &lParser, CSymbolTable & sTable)
{
	if (lParser.GetSymbolType() == ident)
	{
		lParser.Next();
	}
	else if (lParser.GetSymbolType() == number)
	{
		lParser.Next();
	}
	else if (lParser.GetSymbolType() == dec_val)
	{
		error(IDS_ERROR_LEXICAL_NODECIMAL);
	}
	else if (lParser.GetSymbolType() == lparen_op)
	{
		lParser.Next();
		ExpressionSym(lParser, sTable);
		if (lParser.GetSymbolType() == rparen_op)
		{
			lParser.Next();
		}
		else
			error(IDS_ERROR_SYNTAX_STATEMENT_NORPAREN);
	}
	else
		error(IDS_ERROR_SYNTAX_EXPRESSION_UNEXPECTED);
}