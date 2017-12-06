// SW_x230nz666.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "error_messages.h"
#include "sw_symHeader.h"

ProgramSymSyntax program;
DeclarationListSymSyntax declarationListSymSyntax;
IdentTypeSymSyntax identTypeSymSyntax;
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

void error(int a)
{
	printf("%d\n", a);
	exit(EXIT_FAILURE);
}

int main(int argc, wchar_t *argv[])
{
	CLexicalParser lParser(std::_Ptr_wcin);
	// the main lexical parser for the whole program	
	if (lParser.Next())
	{
		ProgramSym(lParser);
	}
	return 0;
}

void ProgramSym(CLexicalParser & lParser)
{
	DeclarationListSym(lParser);
	StatementListSym(lParser);
}

void DeclarationListSym(CLexicalParser & lParser)
{
	ConstDeclarationListSym(lParser);
	VarDeclarationListSym(lParser);
	FunctionDeclarationListSym(lParser);
}

void IdentTypeSym(CLexicalParser & lParser)
{
	if (lParser.GetSymbolType() == int_datatype)
	{
		lParser.Next();
	}
	else if (lParser.GetSymbolType() == dec_datatype)
	{
		lParser.Next();
	}
	else
		error(IDS_ERROR_SYNTAX_DECLARATION_MISSTYPE);
}

void VarDeclarationListSym(CLexicalParser & lParser)
{
	while (varDeclarationListSymSyntax.GetFirst().count(lParser.GetSymbolType()) > 0)
	{
		VarDeclarationSym(lParser);

		if (lParser.GetSymbolType() == semicolon_op)
		{
			lParser.Next();
		}
		else
			error(IDS_ERROR_SYNTAX_DECLARATION_NOENDSEMICOLON);
	}
}

void VarDeclarationSym(CLexicalParser & lParser)
{
	if (lParser.GetSymbolType() == var_sym)
	{
		lParser.Next();
	}
	else
		error(IDS_ERROR_SYNTAX_UNEXPECTED);

	VarIdentDeclarationSym(lParser);

	while (lParser.GetSymbolType() == comma_op)
	{
		lParser.Next();
		VarIdentDeclarationSym(lParser);
	}
}

void VarIdentDeclarationSym(CLexicalParser & lParser)
{
	if (lParser.GetSymbolType() == ident_type)
	{
		lParser.Next();
	}
	else
		error(IDS_ERROR_SYNTAX_DECLARATION_MISSIDENT);

	if (lParser.GetSymbolType() == eql_op)
	{
		lParser.Next();
		ExpressionSym(lParser);
	}
	else if (lParser.GetSymbolType() == as_sym)
	{
		lParser.Next();
		IdentTypeSym(lParser);
		if (lParser.GetSymbolType() == eql_op)
		{
			lParser.Next();
			ExpressionSym(lParser);
		}
	}
}

void ConstDeclarationListSym(CLexicalParser & lParser)
{
	while (constDeclarationSymSyntax.GetFirst().count(lParser.GetSymbolType()) > 0)
	{
		ConstDeclarationSym(lParser);

		if (lParser.GetSymbolType() == semicolon_op)
		{
			lParser.Next();
		}
		else
			error(IDS_ERROR_SYNTAX_DECLARATION_NOENDSEMICOLON);
	}
}

void ConstDeclarationSym(CLexicalParser & lParser)
{
	if (lParser.GetSymbolType() == const_sym)
	{
		lParser.Next();
	}
	else
		error(IDS_ERROR_SYNTAX_UNEXPECTED);

	ConstIdentDeclarationSym(lParser);

	while (lParser.GetSymbolType() == comma_op)
	{
		lParser.Next();
		ConstIdentDeclarationSym(lParser);
	}
}

void ConstIdentDeclarationSym(CLexicalParser & lParser)
{
	if (lParser.GetSymbolType() == ident_type)
	{
		lParser.Next();
	}
	else
		error(IDS_ERROR_SYNTAX_DECLARATION_MISSIDENT);

	if (lParser.GetSymbolType() == as_sym)
	{
		lParser.Next();
		IdentTypeSym(lParser);
	}

	if (lParser.GetSymbolType() == eql_op)
	{
		lParser.Next();
	}
	else
		error(IDS_ERROR_SYNTAX_DECLARATION_CONST_NOEQL);

	ExpressionSym(lParser);
}

void FunctionDeclarationListSym(CLexicalParser & lParser)
{
	while (lParser.GetSymbolType() == func_sym)
	{
		lParser.Next();

		if (lParser.GetSymbolType() == ident_type)
		{
			lParser.Next();
		}
		else
			error(IDS_ERROR_SYNTAX_DECLARATION_MISSIDENT);

		if (lParser.GetSymbolType() == lparen_op)
		{
			lParser.Next();
		}
		else
			error(IDS_ERROR_SYNTAX_DECLARATION_FUNC_NOLPAREN);
		
		if (paramIdentDeclarationSymSyntax.GetFirst().count(lParser.GetSymbolType()) > 0)
		{
			ParamIdentDeclarationSym(lParser);

			while (lParser.GetSymbolType() == comma_op)
			{
				lParser.Next();
				ParamIdentDeclarationSym(lParser);
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

		FunctionBodySym(lParser);

		if (lParser.GetSymbolType() == rbrace_op)
		{
			lParser.Next();
		}
		else
			error(IDS_ERROR_SYNTAX_DECLARATION_FUNC_NORBRACE);
	}
}

void ParamIdentDeclarationSym(CLexicalParser & lParser)
{
	if (lParser.GetSymbolType() == ref_sym)
	{
		lParser.Next();
	}

	if (lParser.GetSymbolType() == ident_type)
	{
		lParser.Next();
	}
	else
		error(IDS_ERROR_SYNTAX_DECLARATION_MISSIDENT);

	if (lParser.GetSymbolType() == as_sym)
	{
		lParser.Next();
	}
	else
		error(IDS_ERROR_SYNTAX_DECLARATION_FUNC_PARAM_NOAS);

	IdentTypeSym(lParser);
}

void FunctionBodySym(CLexicalParser & lParser)
{
	VarDeclarationListSym(lParser);
	StatementListSym(lParser);
}

void StatementListSym(CLexicalParser & lParser)
{
	while (statementSymSyntax.GetFirst().count(lParser.GetSymbolType()) > 0)
	{
		StatementSym(lParser);
		if (lParser.GetSymbolType() == semicolon_op)
		{
			lParser.Next();
		}
		else
			error(IDS_ERROR_SYNTAX_STATEMENT_NOENDSEMICOLON);
	}
}

void StatementSym(CLexicalParser & lParser)
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
		IfStatSym(lParser);
	}
	else if (whileStatSymSyntax.GetFirst().count(lParser.GetSymbolType()) > 0)
	{
		WhileStatSym(lParser);
	}
	else if (repeatStatSymSyntax.GetFirst().count(lParser.GetSymbolType()) > 0)
	{
		RepeatStatSym(lParser);
	}
	else if (readStatSymSyntax.GetFirst().count(lParser.GetSymbolType()) > 0)
	{
		ReadStatSym(lParser);
	}
	else if (printStatSymSyntax.GetFirst().count(lParser.GetSymbolType()) > 0)
	{
		PrintStatSym(lParser);
	}
	else if (assignStatSymSyntax.GetFirst().count(lParser.GetSymbolType()) > 0)
	{
		AssignStatSym(lParser);
	}
	else if (forStatSymSyntax.GetFirst().count(lParser.GetSymbolType()) > 0)
	{
		ForStatSym(lParser);
	}
	else if (callStatSymSyntax.GetFirst().count(lParser.GetSymbolType()) > 0)
	{
		CallStatSym(lParser);
	}
}

void IfStatSym(CLexicalParser & lParser)
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

	ConditionSym(lParser);

	if (lParser.GetSymbolType() == lbrace_op)
	{
		lParser.Next();
	}
	else
		error(IDS_ERROR_SYNTAX_STATEMENT_NOLBRACE);

	StatementListSym(lParser);

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

		StatementListSym(lParser);

		if (lParser.GetSymbolType() == rbrace_op)
		{
			lParser.Next();
		}
		else
			error(IDS_ERROR_SYNTAX_STATEMENT_NORBRACE);
	}
}

void WhileStatSym(CLexicalParser & lParser)
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

	ConditionSym(lParser);

	if (lParser.GetSymbolType() == lbrace_op)
	{
		lParser.Next();
	}
	else
		error(IDS_ERROR_SYNTAX_STATEMENT_NOLBRACE);

	StatementListSym(lParser);

	if (lParser.GetSymbolType() == rbrace_op)
	{
		lParser.Next();
	}
	else
		error(IDS_ERROR_SYNTAX_STATEMENT_NORBRACE);
}

void RepeatStatSym(CLexicalParser & lParser)
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

	StatementListSym(lParser);

	if (lParser.GetSymbolType() == rbrace_op)
	{
		lParser.Next();
	}
	else
		error(IDS_ERROR_SYNTAX_STATEMENT_NORBRACE);
}

void ReadStatSym(CLexicalParser & lParser)
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

	if (lParser.GetSymbolType() == ident_type)
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

void PrintStatSym(CLexicalParser & lParser)
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

	if (lParser.GetSymbolType() == ident_type)
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

void AssignStatSym(CLexicalParser & lParser)
{
	if (lParser.GetSymbolType() == ident_type)
	{
		lParser.Next();
	}
	else
		error(IDS_ERROR_SYNTAX_UNEXPECTED);

	if (lParser.GetSymbolType() == eql_op)
	{
		lParser.Next();
		ExpressionSym(lParser);
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

void ForStatSym(CLexicalParser & lParser)
{
	// for
	if (lParser.GetSymbolType() == for_sym)
	{
		lParser.Next();
	}
	else
		error(IDS_ERROR_SYNTAX_UNEXPECTED);

	// ident
	if (lParser.GetSymbolType() == ident_type)
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

	if (lParser.GetSymbolType() == int_val)
	{
		lParser.Next();
	}
	else if (lParser.GetSymbolType() == dec_val)
	{
		lParser.Next();
	}
	else if (lParser.GetSymbolType() == ident_type)
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

	if (lParser.GetSymbolType() == int_val)
	{
		lParser.Next();
	}
	else if (lParser.GetSymbolType() == dec_val)
	{
		lParser.Next();
	}
	else if (lParser.GetSymbolType() == ident_type)
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

	StatementListSym(lParser);

	if (lParser.GetSymbolType() == rbrace_op)
	{
		lParser.Next();
	}
	else
		error(IDS_ERROR_SYNTAX_STATEMENT_NORBRACE);
}

void CallStatSym(CLexicalParser & lParser)
{
	if (lParser.GetSymbolType() == call_sym)
	{
		lParser.Next();
	}
	else
		error(IDS_ERROR_SYNTAX_UNEXPECTED);

	if (lParser.GetSymbolType() == ident_type)
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
		ExpressionSym(lParser);

		while (lParser.GetSymbolType() == comma_op)
		{
			lParser.Next();
			ExpressionSym(lParser);
		}
	}

	if (lParser.GetSymbolType() == rparen_op)
	{
		lParser.Next();
	}
	else
		error(IDS_ERROR_SYNTAX_STATEMENT_NORPAREN);

}

void ConditionSym(CLexicalParser & lParser)
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

		ExpressionSym(lParser);
		if (lParser.GetSymbolType() == rparen_op)
		{
			lParser.Next();
		}
		else
			error(IDS_ERROR_SYNTAX_STATEMENT_NORPAREN);
	}
	else
	{
		ExpressionSym(lParser);

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
		ExpressionSym(lParser);
	}
}

void ExpressionSym(CLexicalParser & lParser)
{
	if (lParser.GetSymbolType() == plus_op ||
		lParser.GetSymbolType() == minus_op)
	{
		lParser.Next();
	}
	TermSym(lParser);
	while (lParser.GetSymbolType() == plus_op ||
		lParser.GetSymbolType() == minus_op)
	{
		lParser.Next();
		TermSym(lParser);
	}
}

void TermSym(CLexicalParser & lParser)
{
	FactorSym(lParser);
	while (lParser.GetSymbolType() == times_op ||
		lParser.GetSymbolType() == slash_op)
	{
		lParser.Next();
		FactorSym(lParser);
	}
}

void FactorSym(CLexicalParser &lParser)
{
	if (lParser.GetSymbolType() == ident_type)
	{
		lParser.Next();
	}
	else if (lParser.GetSymbolType() == int_val ||
		lParser.GetSymbolType() == dec_val)
	{
		lParser.Next();
	}
	else if (lParser.GetSymbolType() == lparen_op)
	{
		lParser.Next();
		ExpressionSym(lParser);
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