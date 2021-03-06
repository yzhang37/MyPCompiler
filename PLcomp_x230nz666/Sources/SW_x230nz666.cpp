// SW_x230nz666.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "error_messages.h"
#include "sw_symHeader.h"
#include "CSWStructure.h"
#include "EncodeConvHelper.h"
#include "CErrorDetails.h"

void CallStatSymParamProc(CLexicalParser & lParser, CSymbolTable & sTable,
    CFunctionSymbol *pFunction, size_t curParamId);

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
CSWLoopStructure sw_LoopStruct;
CFileEncodeIO fil_coder;
CErrorDetails error_detail;

int error(int error_code, const CLexicalParser & parser)
{
    printf("Error on Ln %zd, Col %zd:\n", parser.GetCurrentLineNo(), parser.GetCurrentColumnNo());
    std::wstring str_err = error_detail.GetErrorMessage(error_code);
    parser.MakeErrorMessageInfo(str_err);
    wprintf(str_err.c_str());
    putchar('\n');
    exit(EXIT_FAILURE);
    return error_code;
}

int warning(int error_code, const CLexicalParser & parser)
{
    printf("Warning on Ln %zd, Col %zd:\n", parser.GetCurrentLineNo(), parser.GetCurrentColumnNo());
    std::wstring str_err = error_detail.GetErrorMessage(error_code);
    parser.MakeErrorMessageInfo(str_err);
    wprintf(str_err.c_str());
    putchar('\n');
    return error_code;
}

int wmain(int argc, wchar_t** argv)
{
    wchar_t *source_file = NULL;
    for (int i = 1; i < argc; ++i)
    {
        source_file = argv[i];
        break;
    }
    if (!source_file)
    {
        printf("Error: no source file entered.\n");
        exit(IDS_ERROR_VM_NOFILE);
    }
    auto bRet = fil_coder.LoadFile(source_file, TRUE);
    if (!bRet)
    {
        printf("Open file \'%ws\' failed.", source_file);
        exit(IDS_ERROR_VM_FILEERROR);
    }
    const wchar_t *file_data = fil_coder.GetValue();
    std::wistringstream sourcein(file_data);

    // the main lexical parser for the whole program	
    CLexicalParser lParser(&sourcein);

    // the definition of the Symbol Table
    CSymbolTable main_Table;

    if (lParser.Next())
    {
        ProgramSym(lParser, main_Table);

        CInstructionTranslator translator;

        std::wstringstream sout;
        std::wstring str_inst;
        for (auto asm_code : outputCodes)
        {
            translator.GetString(asm_code.inst, str_inst);
            sout << str_inst << L" " << asm_code.param_a << L" " << asm_code.param_b << std::endl;
        }
        std::wstring output_str = sout.str();
        fil_coder.SetValue(output_str);
        // make the output path
        std::wstring target_file = source_file;
        if (target_file.find_last_of(L".") != -1)
            target_file.resize(target_file.find_last_of(L"."));
        target_file.append(L".asw");
        fil_coder.SaveFile(target_file.c_str());
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
    outputCodes.push_back(CVirtualMachineInstruction(VI_INITIALIZE_VAR, 0, sw_Struct.GetCurVariableSpace()));
    sw_Struct.PopStashed(outputCodes);
    StatementListSym(lParser, sTable);
    outputCodes.push_back(CVMInst_Return());
}

void DeclarationListSym(CLexicalParser & lParser, CSymbolTable & sTable)
{
    ConstDeclarationListSym(lParser, sTable);
    VarDeclarationListSym(lParser, sTable);
    // until now, there shouldn't be any more variable intialize operations
    // exists here. However, indeed, they exist.

    // have to push all these code into sw_Struct, and after all those 
    // functions are finished, they're moved back.
    sw_Struct.PushStashCodes(outputCodes);

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
            error(IDS_ERROR_SYNTAX_DECLARATION_NOENDSEMICOLON, lParser);
    }
}

void VarDeclarationSym(CLexicalParser & lParser, CSymbolTable & sTable)
{
    if (lParser.GetSymbolType() == var_sym)
    {
        lParser.Next();
    }
    else
        error(IDS_ERROR_SYNTAX_UNEXPECTED, lParser);

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
    CVariableSymbol * newVariable = nullptr;
    if (lParser.GetSymbolType() == ident)
    {
        strIdent = lParser.GetSymbol();
        lParser.Next();

        newVariable = new CVariableSymbol();
        newVariable->SetName(strIdent);
        newVariable->SetStatus(SymbolStatus(SDT_INTEGER));
        newVariable->SetLevel(sw_Struct.GetCurLevel());
        newVariable->SetOffset(3 + (int)sw_Struct.GetCurVariableSpace());
        sw_Struct.AppendSpace(1); //add 1 space for use.

        if (!sTable.EnterSymbol(newVariable))
        {
            error(IDS_ERROR_SYNTAX_DECLARATION_NAMEREPEAT, lParser);
        }
    }
    else
        error(IDS_ERROR_SYNTAX_DECLARATION_MISSIDENT, lParser);

    if (lParser.GetSymbolType() == eql_op ||
        lParser.GetSymbolType() == dbleql_op)
    {
        if (lParser.GetSymbolType() == dbleql_op)
            warning(IDS_WARNING_SYNTAX_ASSIGNMENT_DBLEQUAL, lParser);
        lParser.Next();
        size_t curLine = outputCodes.size();
        bool isConst = true;
        __int64 rValue;
        ExpressionSym(lParser, sTable, &isConst, &rValue);
        if (isConst)
        {
            outputCodes.resize(curLine);
            outputCodes.push_back(CVirtualMachineInstruction(VI_LIT_INTEGER, 0, rValue));
        }
        outputCodes.push_back(CVMInst_Store(*newVariable));
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
            error(IDS_ERROR_SYNTAX_DECLARATION_NOENDSEMICOLON, lParser);
    }
}

void ConstDeclarationSym(CLexicalParser & lParser, CSymbolTable & sTable)
{
    if (lParser.GetSymbolType() == const_sym)
    {
        lParser.Next();
    }
    else
        error(IDS_ERROR_SYNTAX_UNEXPECTED, lParser);

    ConstIdentDeclarationSym(lParser, sTable);

    while (lParser.GetSymbolType() == comma_op)
    {
        lParser.Next();
        ConstIdentDeclarationSym(lParser, sTable);
    }
}

void ConstIdentDeclarationSym(CLexicalParser & lParser, CSymbolTable & sTable)
{
    std::wstring symbolName;
    if (lParser.GetSymbolType() == ident)
    {
        symbolName = lParser.GetSymbol();
        lParser.Next();
    }
    else
        error(IDS_ERROR_SYNTAX_DECLARATION_MISSIDENT, lParser);

    if (lParser.GetSymbolType() == eql_op ||
        lParser.GetSymbolType() == dbleql_op)
    {
        if (lParser.GetSymbolType() == dbleql_op)
            warning(IDS_WARNING_SYNTAX_CONSTANT_DBLEQUAL, lParser);
        lParser.Next();
    }
    else
        error(IDS_ERROR_SYNTAX_DECLARATION_CONST_NOEQL, lParser);

    size_t curLine = outputCodes.size();
    bool isConst = true;
    __int64 rValue;
    ExpressionSym(lParser, sTable, &isConst, &rValue);

    if (!isConst)
        error(IDS_ERROR_SYNTAX_DECLARATION_CONST_NONLITERAL, lParser);
    else
    {
        CConstantSymbol *pNewConst = new CConstantSymbol;
        pNewConst->SetName(symbolName);
        pNewConst->SetLevel(sw_Struct.GetCurLevel());
        pNewConst->SetValue(rValue);
        sTable.EnterSymbol(pNewConst);
        outputCodes.resize(curLine);
    }
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
            CFunctionSymbol *newFunction = new CFunctionSymbol();
            newFunction->SetName(symbolName);
            newFunction->SetLevel(sw_Struct.GetCurLevel());

            // save the current code pos, as the function's address
            newFunction->SetOffset((int)outputCodes.size());

            if (!sTable.EnterSymbol(newFunction))
            {
                error(IDS_ERROR_SYNTAX_DECLARATION_NAMEREPEAT, lParser);
            }
        }
        else
            error(IDS_ERROR_SYNTAX_DECLARATION_MISSIDENT, lParser);

        // push, before params are generated
        sw_Struct.push(sTable.GetTableSize(), outputCodes.size());

        lParser.Next();
        if (lParser.GetSymbolType() == lparen_op)
        {
            lParser.Next();
        }
        else
            error(IDS_ERROR_SYNTAX_DECLARATION_FUNC_NOLPAREN, lParser);

        if (paramIdentDeclarationSymSyntax.GetFirst().count(lParser.GetSymbolType()) > 0)
        {
            ParamIdentDeclarationSym(lParser, sTable);

            while (lParser.GetSymbolType() == comma_op)
            {
                lParser.Next();
                ParamIdentDeclarationSym(lParser, sTable);
            }
        }

        sTable.EncapLastFuncParams();

        if (lParser.GetSymbolType() == rparen_op)
        {
            lParser.Next();
        }
        else
            error(IDS_ERROR_SYNTAX_DECLARATION_FUNC_NORPAREN, lParser);

        if (lParser.GetSymbolType() == lbrace_op)
        {
            lParser.Next();
        }
        else
            error(IDS_ERROR_SYNTAX_DECLARATION_FUNC_NOLBRACE, lParser);

        FunctionBodySym(lParser, sTable);
        sw_Struct.pop();
        // prune the sTable: remove all the symbols higher than the current level.
        sTable.PruneTo(sw_Struct.GetCurLevel());

        if (lParser.GetSymbolType() == rbrace_op)
        {
            lParser.Next();
        }
        else
            error(IDS_ERROR_SYNTAX_DECLARATION_FUNC_NORBRACE, lParser);
    }
}

void ParamIdentDeclarationSym(CLexicalParser & lParser, CSymbolTable & sTable)
{
    // first we need to fetch,
    // how many params now already existed.
    CFunctionSymbol *lastFunc = sTable.GetLastFunction();
    if (!lastFunc)
        error(IDS_ERROR_SYNTAX_DECLARATION_REF_NOFUNCTION, lParser);
    size_t param_count = lastFunc->GetParams().size();

    CVariableSymbol *newSymbol = new CVariableSymbol();

    // only integer type, no others type
    newSymbol->_Status().dateType = SDT_INTEGER;
    newSymbol->SetLevel(sw_Struct.GetCurLevel());

    // notice: now the offset of these param are marked as "positive"
    // number. However indeed, it is processed as "negative" when running
    // inside the stack structure. When push these params before bl, 
    // the index - foundFunc->GetParams().size is the real positioin.
    newSymbol->SetOffset((int)param_count);

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
    }
    else
        error(IDS_ERROR_SYNTAX_DECLARATION_MISSIDENT, lParser);

    if (!sTable.EnterSymbol(newSymbol))
    {
        error(IDS_ERROR_SYNTAX_DECLARATION_NAMEREPEAT, lParser);
    }
    lParser.Next();
}

void FunctionBodySym(CLexicalParser & lParser, CSymbolTable & sTable)
{
    outputCodes.push_back(CVirtualMachineInstruction(VI_BRANCH, 0, 0));
    VarDeclarationListSym(lParser, sTable);
    sw_Struct.PushStashCodes(outputCodes);
    // where is the function?
    // haha, currently nested functions is not allowed.

    outputCodes[sw_Struct.GetCurCodeBase()].param_b = outputCodes.size();
    outputCodes.push_back(CVirtualMachineInstruction(VI_INITIALIZE_VAR, 0, sw_Struct.GetCurVariableSpace()));

    sw_Struct.PopStashed(outputCodes);

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
            error(IDS_ERROR_SYNTAX_STATEMENT_NOENDSEMICOLON, lParser);
    }
    if (constDeclarataionListSymSyntax.GetFirst().count(lParser.GetSymbolType()) > 0)
        error(IDS_ERROR_SYNTAX_DECLARATION_CONST_BEFORESTAT, lParser);
    else if (varDeclarationListSymSyntax.GetFirst().count(lParser.GetSymbolType()) > 0)
        error(IDS_ERROR_SYNTAX_DECLARATION_CONST_BEFORESTAT, lParser);
}

void StatementSym(CLexicalParser & lParser, CSymbolTable & sTable)
{
    if (lParser.GetSymbolType() == continue_sym)
    {
        if (!sw_LoopStruct.addContinue(outputCodes.size()))
            error(IDS_ERROR_SYNTAX_STATEMENT_CONTINUE_NOLOOP, lParser);
        else
            outputCodes.push_back(CVirtualMachineInstruction(VI_BRANCH, 0, 0));
        lParser.Next();
    }
    else if (lParser.GetSymbolType() == break_sym)
    {
        if (!sw_LoopStruct.addBreak(outputCodes.size()))
            error(IDS_ERROR_SYNTAX_STATEMENT_BREAK_NOLOOP, lParser);
        else
            outputCodes.push_back(CVirtualMachineInstruction(VI_BRANCH, 0, 0));
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
    bool neg = false;
    if (lParser.GetSymbolType() == if_sym)
        lParser.Next();
    else if (lParser.GetSymbolType() == unless_sym)
    {
        neg = true;
        lParser.Next();
    }
    else
        error(IDS_ERROR_SYNTAX_UNEXPECTED, lParser);

    ConditionSym(lParser, sTable);
    if (neg)
        outputCodes.push_back(CVMInst_Not());

    size_t ifJumpBase1 = outputCodes.size();
    // bc 0 pos
    outputCodes.push_back(CVirtualMachineInstruction(VI_BRANCH_COND, 0, 0));

    if (lParser.GetSymbolType() == lbrace_op)
    {
        lParser.Next();
    }
    else
        error(IDS_ERROR_SYNTAX_STATEMENT_NOLBRACE, lParser);

    StatementListSym(lParser, sTable);

    if (lParser.GetSymbolType() == rbrace_op)
    {
        lParser.Next();
    }
    else
        error(IDS_ERROR_SYNTAX_STATEMENT_NORBRACE, lParser);

    // if there exists else
    if (lParser.GetSymbolType() == else_sym)
    {
        lParser.Next();
        outputCodes.push_back(CVirtualMachineInstruction(VI_BRANCH, 0, 0));
        outputCodes[ifJumpBase1].param_b = outputCodes.size();

        if (lParser.GetSymbolType() == lbrace_op)
        {
            lParser.Next();
        }
        else
            error(IDS_ERROR_SYNTAX_STATEMENT_NOLBRACE, lParser);

        StatementListSym(lParser, sTable);

        if (lParser.GetSymbolType() == rbrace_op)
        {
            lParser.Next();
        }
        else
            error(IDS_ERROR_SYNTAX_STATEMENT_NORBRACE, lParser);

        outputCodes[outputCodes[ifJumpBase1].param_b - 1].param_b = outputCodes.size();
    }
    else // do not exist else
    {
        outputCodes[ifJumpBase1].param_b = outputCodes.size();
    }
}

void WhileStatSym(CLexicalParser & lParser, CSymbolTable & sTable)
{
    bool neg = false;
    if (lParser.GetSymbolType() == while_sym)
        lParser.Next();
    else if (lParser.GetSymbolType() == until_sym)
    {
        neg = true;
        lParser.Next();
    }
    else
        error(IDS_ERROR_SYNTAX_UNEXPECTED, lParser);

    size_t loopBase1 = outputCodes.size();
    ConditionSym(lParser, sTable);
    if (neg)
        outputCodes.push_back(CVMInst_Not());

    size_t loopBase2 = outputCodes.size();
    outputCodes.push_back(CVirtualMachineInstruction(VI_BRANCH_COND, 0, 0));

    if (lParser.GetSymbolType() == lbrace_op)
    {
        lParser.Next();
    }
    else
        error(IDS_ERROR_SYNTAX_STATEMENT_NOLBRACE, lParser);

    sw_LoopStruct.push();
    StatementListSym(lParser, sTable);
    outputCodes.push_back(CVirtualMachineInstruction(VI_BRANCH, 0, loopBase1));
    outputCodes[loopBase2].param_b = outputCodes.size();

    if (lParser.GetSymbolType() == rbrace_op)
    {
        lParser.Next();
    }
    else
        error(IDS_ERROR_SYNTAX_STATEMENT_NORBRACE, lParser);
    sw_LoopStruct.pop(loopBase1, outputCodes.size(), outputCodes);
}

void RepeatStatSym(CLexicalParser & lParser, CSymbolTable & sTable)
{
    if (lParser.GetSymbolType() == repeat_sym)
        lParser.Next();
    else
        error(IDS_ERROR_SYNTAX_UNEXPECTED, lParser);

    if (lParser.GetSymbolType() == lbrace_op)
        lParser.Next();
    else
        error(IDS_ERROR_SYNTAX_STATEMENT_NOLBRACE, lParser);

    size_t repeatBase = outputCodes.size();
    
    sw_LoopStruct.push();
    StatementListSym(lParser, sTable);

    if (lParser.GetSymbolType() == rbrace_op)
        lParser.Next();
    else
        error(IDS_ERROR_SYNTAX_STATEMENT_NORBRACE, lParser);

    bool neg = false;
    if (lParser.GetSymbolType() == while_sym)
    {
        neg = true;
        lParser.Next();
    }
    else if (lParser.GetSymbolType() == until_sym)
        lParser.Next();
    else
        error(IDS_ERROR_SYNTAX_REPEAT_NOWHILEUNTIL, lParser);

    size_t selfOperLine = outputCodes.size();
    ConditionSym(lParser, sTable);
    if (neg)
        outputCodes.push_back(CVMInst_Not());
    outputCodes.push_back(CVirtualMachineInstruction(VI_BRANCH_COND, 0, repeatBase));
    sw_LoopStruct.pop(selfOperLine, outputCodes.size(), outputCodes);
}

void ReadStatSym(CLexicalParser & lParser, CSymbolTable & sTable)
{
    if (lParser.GetSymbolType() == read_sym)
    {
        lParser.Next();
    }
    else
        error(IDS_ERROR_SYNTAX_UNEXPECTED, lParser);

    outputCodes.push_back(CVMInst_Read());

    if (lParser.GetSymbolType() == lparen_op)
    {
        lParser.Next();
    }
    else
        error(IDS_ERROR_SYNTAX_STATEMENT_NOLPAREN, lParser);

    std::wstring symbolName;
    if (lParser.GetSymbolType() == ident)
    {
        symbolName = lParser.GetSymbol();
    }
    else
        error(IDS_ERROR_SYNTAX_STATEMENT_MISSIDENT, lParser);

    auto pSymbol = sTable.FindLatest(symbolName);
    if (!pSymbol)
        error(IDS_ERROR_SYNTAX_STATEMENT_IDENTNOTEXIST, lParser);

    if (pSymbol->GetAttribute() != SA_VARIABLE)
        error(IDS_ERROR_SYNTAX_STATEMENT_NOTVARIABLE, lParser);

    CVariableSymbol *pVar = (CVariableSymbol *)pSymbol;
    if (pVar->GetStatus().isRef) // if it is a reference
        outputCodes.push_back(CVMInst_StoreRef(*pVar));
    else
        outputCodes.push_back(CVMInst_Store(*pVar));

    lParser.Next();
    if (lParser.GetSymbolType() == rparen_op)
    {
        lParser.Next();
    }
    else
        error(IDS_ERROR_SYNTAX_STATEMENT_NORPAREN, lParser);
}

void PrintStatSym(CLexicalParser & lParser, CSymbolTable & sTable)
{
    if (lParser.GetSymbolType() == print_sym)
    {
        lParser.Next();
    }
    else
        error(IDS_ERROR_SYNTAX_UNEXPECTED, lParser);

    if (lParser.GetSymbolType() == lparen_op)
    {
        lParser.Next();
    }
    else
        error(IDS_ERROR_SYNTAX_STATEMENT_NOLPAREN, lParser);

    std::wstring symbolName;
    if (lParser.GetSymbolType() == ident)
    {
        symbolName = lParser.GetSymbol();
    }
    else
        error(IDS_ERROR_SYNTAX_STATEMENT_MISSIDENT, lParser);

    auto pSymbol = sTable.FindLatest(symbolName);
    if (!pSymbol)
        error(IDS_ERROR_SYNTAX_STATEMENT_IDENTNOTEXIST, lParser);

    if (pSymbol->GetAttribute() == SA_FUNCTION)
        error(IDS_ERROR_SYNTAX_STATEMENT_NOTPRINTABLE, lParser);

    if (pSymbol->GetAttribute() == SA_VARIABLE)
    {
        CVariableSymbol *pVar = (CVariableSymbol *)pSymbol;
        if (pVar->GetStatus().isRef) // if it is a reference
            outputCodes.push_back(CVMInst_LoadRef(*pVar));
        else
            outputCodes.push_back(CVMInst_Load(*pVar));
    }
    else if (pSymbol->GetAttribute() == SA_CONSTANT)
    {
        CConstantSymbol *pConst = (CConstantSymbol *)pSymbol;
        outputCodes.push_back(CVirtualMachineInstruction(VI_LIT_INTEGER, 0, pConst->GetValue()));
    }
    else
        error(IDS_ERROR_SYNTAX_UNEXPECTED, lParser);

    outputCodes.push_back(CVMInst_Print());
    lParser.Next();
    if (lParser.GetSymbolType() == rparen_op)
    {
        lParser.Next();
    }
    else
        error(IDS_ERROR_SYNTAX_STATEMENT_NORPAREN, lParser);
}

void AssignStatSym(CLexicalParser & lParser, CSymbolTable & sTable)
{
    std::wstring symbolName;
    if (lParser.GetSymbolType() == ident)
    {
        symbolName = lParser.GetSymbol();
    }
    else
        error(IDS_ERROR_SYNTAX_UNEXPECTED, lParser);

    auto pFoundSymbol = sTable.FindLatest(symbolName);
    if (!pFoundSymbol)
        error(IDS_ERROR_SYNTAX_STATEMENT_IDENTNOTEXIST, lParser);
    if (pFoundSymbol->GetAttribute() == SA_CONSTANT)
        error(IDS_ERROR_SYNTAX_STATEMENT_ASSIGN_NONLVALUE, lParser);
    else if (pFoundSymbol->GetAttribute() == SA_FUNCTION)
        error(IDS_ERROR_SYNTAX_STATEMENT_CALL_NOCALL, lParser);
    
    lParser.Next();
    auto pFoundVariable = (CVariableSymbol *)pFoundSymbol;
    if (lParser.GetSymbolType() == eql_op    ||
        lParser.GetSymbolType() == dbleql_op)
    {
        if (lParser.GetSymbolType() == dbleql_op)
            warning(IDS_WARNING_SYNTAX_ASSIGNMENT_DBLEQUAL, lParser);
        lParser.Next();
        size_t curLine = outputCodes.size();
        bool isConst = true;
        __int64 rValue;
        ExpressionSym(lParser, sTable, &isConst, &rValue);
        if (isConst)
        {
            outputCodes.resize(curLine);
            outputCodes.push_back(CVirtualMachineInstruction(VI_LIT_INTEGER, 0, rValue));
        }

        if (pFoundVariable->GetStatus().isRef)
            outputCodes.push_back(CVMInst_StoreRef(*pFoundVariable));
        else
            outputCodes.push_back(CVMInst_Store(*pFoundVariable));
    }
    else if (lParser.GetSymbolType() == dblplus_op ||
        lParser.GetSymbolType() == dblminus_op)
    {
        auto symbOper = lParser.GetSymbolType();
        // load the original value
        if (pFoundVariable->GetStatus().isRef)
            outputCodes.push_back(CVMInst_LoadRef(*pFoundVariable));
        else
            outputCodes.push_back(CVMInst_Load(*pFoundVariable));
        // push 1
        outputCodes.push_back(CVirtualMachineInstruction(VI_LIT_INTEGER, 0, 1));
        if (symbOper == dblplus_op)
            outputCodes.push_back(CVMInst_Plus());
        else
            outputCodes.push_back(CVMInst_Minus());
        // store the value
        if (pFoundVariable->GetStatus().isRef)
            outputCodes.push_back(CVMInst_StoreRef(*pFoundVariable));
        else
            outputCodes.push_back(CVMInst_Store(*pFoundVariable));
        lParser.Next();
    }
    else
        error(IDS_ERROR_SYNTAX_ASSIGN_UNEXPECTED, lParser);
}

void ForStatSym(CLexicalParser & lParser, CSymbolTable & sTable)
{
    // for
    if (lParser.GetSymbolType() == for_sym)
        lParser.Next();
    else
        error(IDS_ERROR_SYNTAX_UNEXPECTED, lParser);

    // ident
    std::wstring symbolName;
    if (lParser.GetSymbolType() == ident)
    {
        symbolName = lParser.GetSymbol();
    }
    else
        error(IDS_ERROR_SYNTAX_STATEMENT_MISSIDENT, lParser);

    auto pFoundSymbol = sTable.FindLatest(symbolName);
    if (!pFoundSymbol)
        error(IDS_ERROR_SYNTAX_STATEMENT_IDENTNOTEXIST, lParser);
    if (pFoundSymbol->GetAttribute() != SA_VARIABLE)
        error(IDS_ERROR_SYNTAX_STATEMENT_FOR_NONLVALUE, lParser);
    auto pVariable = (CVariableSymbol *)pFoundSymbol;

    // in
    lParser.Next();
    if (lParser.GetSymbolType() == in_sym)
    {
        lParser.Next();
    }
    else
        error(IDS_ERROR_SYNTAX_FOR_MISSIN, lParser);

    // first value
    size_t curLine = outputCodes.size();
    bool isConst = true;
    __int64 rValue1;
    ExpressionSym(lParser, sTable, &isConst, &rValue1);
    if (isConst)
    {
        outputCodes.resize(curLine);
        outputCodes.push_back(CVirtualMachineInstruction(VI_LIT_INTEGER, 0, rValue1));
    }
    else
        error(IDS_ERROR_SYNTAX_FOR_NONLITERAL, lParser);

    if (pVariable->GetStatus().isRef)
        outputCodes.push_back(CVMInst_StoreRef(*pVariable));
    else
        outputCodes.push_back(CVMInst_Store(*pVariable));

    if (lParser.GetSymbolType() == to_op)
    {
        lParser.Next();
    }
    else
        error(IDS_ERROR_SYNTAX_FOR_MISSTO, lParser);

    // second value
    curLine = outputCodes.size();
    size_t forJumpBase1 = curLine;
    __int64 rValue2;
    isConst = true;
    ExpressionSym(lParser, sTable, &isConst, &rValue2);
    if (isConst)
    {
        outputCodes.resize(curLine);
        outputCodes.push_back(CVirtualMachineInstruction(VI_LIT_INTEGER, 0, rValue2));
    }
    else
        error(IDS_ERROR_SYNTAX_FOR_NONLITERAL, lParser);

    if (pVariable->GetStatus().isRef)
        outputCodes.push_back(CVMInst_LoadRef(*pVariable));
    else
        outputCodes.push_back(CVMInst_Load(*pVariable));

    __int64 stepVal = 1;
    if (lParser.GetSymbolType() == step_sym)
    {
        lParser.Next();
        size_t curLine = outputCodes.size();
        bool isConst = true;
        __int64 rValue;
        ExpressionSym(lParser, sTable, &isConst, &rValue);
        if (isConst)
            outputCodes.resize(curLine);
        else
            error(IDS_ERROR_SYNTAX_FOR_NONLITERAL, lParser);
        stepVal = rValue;
    }
    else
    {
        if (rValue1 > rValue2)
            stepVal = -1;
    }

    if (rValue1 <= rValue2)
        outputCodes.push_back(CVMInst_GreaterEqual());
    else
        outputCodes.push_back(CVMInst_LessEqual());

    size_t forJumpBase2 = outputCodes.size();
    outputCodes.push_back(CVirtualMachineInstruction(VI_BRANCH_COND, 0, 0));

    if (lParser.GetSymbolType() == lbrace_op)
    {
        lParser.Next();
    }
    else
        error(IDS_ERROR_SYNTAX_STATEMENT_NOLBRACE, lParser);

    sw_LoopStruct.push();
    StatementListSym(lParser, sTable);

    // self add operation
    size_t forSelfOpLine = outputCodes.size();
    if (pVariable->GetStatus().isRef)
        outputCodes.push_back(CVMInst_LoadRef(*pVariable));
    else
        outputCodes.push_back(CVMInst_Load(*pVariable));
    outputCodes.push_back(CVirtualMachineInstruction(VI_LIT_INTEGER, 0, stepVal));
    outputCodes.push_back(CVMInst_Plus());
    if (pVariable->GetStatus().isRef)
        outputCodes.push_back(CVMInst_StoreRef(*pVariable));
    else
        outputCodes.push_back(CVMInst_Store(*pVariable));

    // jump back
    outputCodes.push_back(CVirtualMachineInstruction(VI_BRANCH, 0, forJumpBase1));
    outputCodes[forJumpBase2].param_b = outputCodes.size();
    sw_LoopStruct.pop(forSelfOpLine, outputCodes.size(), outputCodes);

    if (lParser.GetSymbolType() == rbrace_op)
    {
        lParser.Next();
    }
    else
        error(IDS_ERROR_SYNTAX_STATEMENT_NORBRACE, lParser);
}

void CallStatSymParamProc(CLexicalParser & lParser, CSymbolTable & sTable,
    CFunctionSymbol *pFunction, size_t curParamId)
{
    if (pFunction->GetParams()[curParamId].isRef)
    {
        // if it is an ref, then the next symbol must be an ident, not an expression;
        if (lParser.GetSymbolType() != ident)
            error(IDS_ERROR_SYNTAX_STATEMENT_CALL_NOTREFERENCE, lParser);
        std::wstring symbolName = lParser.GetSymbol();
        auto pFoundSymbol = sTable.FindLatest(symbolName);
        if (!pFoundSymbol)
            error(IDS_ERROR_SYNTAX_STATEMENT_IDENTNOTEXIST, lParser);
        if (pFoundSymbol->GetAttribute() != SA_VARIABLE)
            error(IDS_ERROR_SYNTAX_STATEMENT_CALL_NOTREFERENCE, lParser);

        CVariableSymbol *var = (CVariableSymbol *)pFoundSymbol;
        if (var->GetStatus().isRef)
        {
            // notice: here is a tricky use.
            outputCodes.push_back(CVMInst_Load(*var));
        }
        else
        {
            outputCodes.push_back(CVirtualMachineInstruction(VI_MAKE_REFERENCE, var->GetLevel(),
                var->GetOffset()));
        }
        lParser.Next();
    }
    else
    {
        size_t curLine = outputCodes.size();
        bool isConst = true;
        __int64 rValue;
        ExpressionSym(lParser, sTable, &isConst, &rValue);
        if (isConst)
        {
            outputCodes.resize(curLine);
            outputCodes.push_back(CVirtualMachineInstruction(VI_LIT_INTEGER, 0, rValue));
        }
    }
}

void CallStatSym(CLexicalParser & lParser, CSymbolTable & sTable)
{
    if (lParser.GetSymbolType() == call_sym)
    {
        lParser.Next();
    }
    else
        error(IDS_ERROR_SYNTAX_UNEXPECTED, lParser);

    std::wstring symbolName;
    if (lParser.GetSymbolType() == ident)
    {
        symbolName = lParser.GetSymbol();
    }
    else
        error(IDS_ERROR_SYNTAX_STATEMENT_MISSIDENT, lParser);

    // look up for the symbol in the sTable
    auto pFoundSymbol = sTable.FindLatest(symbolName);
    if (!pFoundSymbol)
    {
        error(IDS_ERROR_SYNTAX_STATEMENT_IDENTNOTEXIST, lParser);
    }
    if (pFoundSymbol->GetAttribute() != SA_FUNCTION)
    {
        error(IDS_ERROR_SYNTAX_STATEMENT_CALL_NONFUNC, lParser);
    }
    CFunctionSymbol *pFoundFunction = (CFunctionSymbol *)pFoundSymbol;

    lParser.Next();
    if (lParser.GetSymbolType() == lparen_op)
    {
        lParser.Next();
    }
    else
        error(IDS_ERROR_SYNTAX_STATEMENT_NOLPAREN, lParser);

    size_t usedParams = 0;
    if (expressionSymSyntax.GetFirst().count(lParser.GetSymbolType()) > 0)
    {
        if (usedParams < pFoundFunction->GetParams().size())
        {
            CallStatSymParamProc(lParser, sTable, pFoundFunction, usedParams);
            ++usedParams;
        }
        else
            error(IDS_ERROR_SYNTAX_STATEMENT_CALL_NUMDISMATCH, lParser);

        while (lParser.GetSymbolType() == comma_op)
        {
            lParser.Next();
            if (usedParams < pFoundFunction->GetParams().size())
            {
                CallStatSymParamProc(lParser, sTable, pFoundFunction, usedParams);
                ++usedParams;
            }
            else
                error(IDS_ERROR_SYNTAX_STATEMENT_CALL_NUMDISMATCH, lParser);
        }
    }
    if (usedParams < pFoundFunction->GetParams().size())
    {
        // too little params
        error(IDS_ERROR_SYNTAX_STATEMENT_CALL_NUMDISMATCH, lParser);
    }

    if (lParser.GetSymbolType() == rparen_op)
    {
        lParser.Next();
    }
    else
        error(IDS_ERROR_SYNTAX_STATEMENT_NORPAREN, lParser);

    outputCodes.push_back(CVirtualMachineInstruction(VI_LIT_INTEGER, 0, pFoundFunction->GetParams().size()));
    outputCodes.push_back(CVirtualMachineInstruction(VI_BRANCH_LINK, 0, pFoundFunction->GetOffset()));
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
            error(IDS_ERROR_SYNTAX_STATEMENT_NOLPAREN, lParser);

        size_t curLine = outputCodes.size();
        bool isConst = true;
        __int64 rValue;
        ExpressionSym(lParser, sTable, &isConst, &rValue);
        if (isConst)
        {
            outputCodes.resize(curLine);
            outputCodes.push_back(CVirtualMachineInstruction(VI_LIT_INTEGER, 0, rValue));
        }
        outputCodes.push_back(CVMInst_Odd());

        if (lParser.GetSymbolType() == rparen_op)
        {
            lParser.Next();
        }
        else
            error(IDS_ERROR_SYNTAX_STATEMENT_NORPAREN, lParser);
    }
    else
    {
        size_t curLine = outputCodes.size();
        bool isConst = true;
        __int64 rValue;
        ExpressionSym(lParser, sTable, &isConst, &rValue);
        if (isConst)
        {
            outputCodes.resize(curLine);
            outputCodes.push_back(CVirtualMachineInstruction(VI_LIT_INTEGER, 0, rValue));
        }

        if (lParser.GetSymbolType() == eql_op    ||
            lParser.GetSymbolType() == dbleql_op ||
            lParser.GetSymbolType() == neq_op    ||
            lParser.GetSymbolType() == lss_op    ||
            lParser.GetSymbolType() == leq_op    ||
            lParser.GetSymbolType() == gtr_op    ||
            lParser.GetSymbolType() == geq_op)
        {
            auto symbol = lParser.GetSymbolType();
            lParser.Next();

            size_t curLine = outputCodes.size();
            bool isConst = true;
            __int64 rValue;
            ExpressionSym(lParser, sTable, &isConst, &rValue);
            if (isConst)
            {
                outputCodes.resize(curLine);
                outputCodes.push_back(CVirtualMachineInstruction(VI_LIT_INTEGER, 0, rValue));
            }

            switch (symbol)
            {
            case eql_op:
            case dbleql_op:
                outputCodes.push_back(CVMInst_Equal());
                break;
            case neq_op:
                outputCodes.push_back(CVMInst_NotEqual());
                break;
            case lss_op:
                outputCodes.push_back(CVMInst_Less());
                break;
            case leq_op:
                outputCodes.push_back(CVMInst_LessEqual());
                break;
            case gtr_op:
                outputCodes.push_back(CVMInst_Greater());
                break;
            case geq_op:
                outputCodes.push_back(CVMInst_GreaterEqual());
                break;
            }
        }
        else
            error(IDS_ERROR_SYNTAX_CONDITION_MISSREL, lParser);
    }
}

void ExpressionSym(CLexicalParser & lParser, CSymbolTable & sTable, bool *pIsConst, __int64 *pRVal)
{
    bool neg = false;
    if (lParser.GetSymbolType() == plus_op ||
        lParser.GetSymbolType() == minus_op)
    {
        if (lParser.GetSymbolType() == minus_op)
            neg = true;
        lParser.Next();
    }

    if (pIsConst && pRVal && *pIsConst) TermSym(lParser, sTable, pIsConst, pRVal);
    else TermSym(lParser, sTable);

    // after handle with it, if neg, we make a neg operation on it.
    if (neg)
    {
        outputCodes.push_back(CVMInst_MakeNegative());
        if (pIsConst && pRVal && *pIsConst)
            *pRVal = -*pRVal;
    }

    while (lParser.GetSymbolType() == plus_op ||
        lParser.GetSymbolType() == minus_op)
    {
        neg = false;
        if (lParser.GetSymbolType() == minus_op)
            neg = true;
        lParser.Next();

        __int64 seRVal;
        if (pIsConst && pRVal && *pIsConst) TermSym(lParser, sTable, pIsConst, &seRVal);
        else TermSym(lParser, sTable);

        if (!neg)
        {
            outputCodes.push_back(CVMInst_Plus());
            if (pIsConst && pRVal && *pIsConst)
                *pRVal += seRVal;
        }
        else
        {
            outputCodes.push_back(CVMInst_Minus());
            if (pIsConst && pRVal && *pIsConst)
                *pRVal -= seRVal;
        }
    }
}

void TermSym(CLexicalParser & lParser, CSymbolTable & sTable, bool *pIsConst, __int64 *pRVal)
{
    if (pIsConst && pRVal && *pIsConst)
        FactorSym(lParser, sTable, pIsConst, pRVal);
    else
        FactorSym(lParser, sTable);

    while (lParser.GetSymbolType() == times_op ||
        lParser.GetSymbolType() == slash_op ||
        lParser.GetSymbolType() == mod_op)
    {
        auto thisSymb = lParser.GetSymbolType();
        lParser.Next();

        __int64 seRVal;
        if (pIsConst && pRVal && *pIsConst)
            FactorSym(lParser, sTable, pIsConst, &seRVal);
        else
            FactorSym(lParser, sTable);

        switch (thisSymb)
        {
        case times_op:
            outputCodes.push_back(CVMInst_Times());
            if (pIsConst && pRVal && *pIsConst)
                (*pRVal) *= seRVal;
            break;
        case slash_op:
            outputCodes.push_back(CVMInst_Divide());
            if (pIsConst && pRVal && *pIsConst)
                (*pRVal) /= seRVal;
            break;
        case mod_op:
            outputCodes.push_back(CVMInst_Module());
            if (pIsConst && pRVal && *pIsConst)
                (*pRVal) %= seRVal;
            break;
        }
    }
}

void FactorSym(CLexicalParser &lParser, CSymbolTable & sTable, bool *pIsConst, __int64 *pRVal)
{
    if (lParser.GetSymbolType() == ident)
    {
        // find the variable in the symbol table
        // if not constant, then make *pIsConst = false
        std::wstring symbolName = lParser.GetSymbol();

        auto pFoundSymbol = sTable.FindLatest(symbolName);

        if (!pFoundSymbol)
            error(IDS_ERROR_SYNTAX_STATEMENT_IDENTNOTEXIST, lParser);

        // currently, it's not allowed directly put the function
        // into the expression statement. Maybe in the future, 
        // it will be open.
        if (pFoundSymbol->GetAttribute() == SA_FUNCTION)
            error(IDS_ERROR_SYNTAX_STATEMENT_EXP_FUNCNOTALLOW, lParser);

        if (pFoundSymbol->GetAttribute() == SA_CONSTANT) // ok, is constant
        {
            CConstantSymbol *pFoundConst = (CConstantSymbol *)pFoundSymbol;
            outputCodes.push_back(CVirtualMachineInstruction(VI_LIT_INTEGER, 0, pFoundConst->GetValue()));

            if (pIsConst && pRVal && *pIsConst)
                *pRVal = pFoundConst->GetValue();
        }
        else if (pFoundSymbol->GetAttribute() == SA_VARIABLE)// is variable, :(
        {
            if (pIsConst && pRVal && *pIsConst)
                *pIsConst = false; // say goodbye to constant literal!
            CVariableSymbol *pFoundVariable = (CVariableSymbol *)pFoundSymbol;
            if (pFoundVariable->GetStatus().isRef) // is reference
            {
                // find the last function
                auto pFunc = sTable.GetLastFunction();
                if (!pFunc)
                    error(IDS_ERROR_SYNTAX_DECLARATION_REF_NOFUNCTION, lParser);
                outputCodes.push_back(CVMInst_LoadRef(*pFoundVariable));
            }
            else
                outputCodes.push_back(CVMInst_Load(*pFoundVariable));
        }
        else
            error(IDS_ERROR_SYNTAX_UNEXPECTED, lParser); // what? something unexpected happened.

        lParser.Next();
    }
    else if (lParser.GetSymbolType() == number)
    {
        outputCodes.push_back(CVirtualMachineInstruction(VI_LIT_INTEGER, 0, lParser.GetInteger()));
        if (pIsConst && pRVal && *pIsConst)
            *pRVal = lParser.GetInteger();
        lParser.Next();
    }
    else if (lParser.GetSymbolType() == dec_val)
    {
        error(IDS_ERROR_LEXICAL_NODECIMAL, lParser);
    }
    else if (lParser.GetSymbolType() == lparen_op)
    {
        lParser.Next();
        if (pIsConst && pRVal && *pIsConst)
            ExpressionSym(lParser, sTable, pIsConst, pRVal);
        else
            ExpressionSym(lParser, sTable);

        if (lParser.GetSymbolType() == rparen_op)
        {
            lParser.Next();
        }
        else
            error(IDS_ERROR_SYNTAX_STATEMENT_NORPAREN, lParser);
    }
    else
        error(IDS_ERROR_SYNTAX_EXPRESSION_UNEXPECTED, lParser);
}