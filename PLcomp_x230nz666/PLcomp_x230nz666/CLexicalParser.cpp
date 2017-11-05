#include "stdafx.h"
#include "CLexicalParser.h"
#pragma warning(disable:4101)

CLexicalParser::CLexicalParser(std::wistream *wsIn)
{
	m_msSym[L'+'] = plus_op;
	m_msSym[L'-'] = minus_op;
	m_msSym[L'*'] = times_op;
	m_msSym[L'/'] = slash_op;
	m_msSym[L'%'] = mod_op;
	m_msSym[L'='] = eql_op;
	m_msSym[L'<'] = lss_op;
	m_msSym[L'>'] = gtr_op;
	m_msSym[L'('] = lparen_op;
	m_msSym[L')'] = rparen_op;
	m_msSym[L','] = comma_op;
	m_msSym[L';'] = semicolon_op;
	m_msSym[L':'] = colon_op;
	m_msSym[L'.'] = period_op;

	m_curCh = L' ';

	//add all the keywords
	insertKeyWords(L"and", and_op);
	insertKeyWords(L"as", as_sym);
	insertKeyWords(L"boolean", bool_datatype);
	insertKeyWords(L"break", break_sym);
	insertKeyWords(L"call", call_sym);
	insertKeyWords(L"case", case_sym);
	insertKeyWords(L"const", const_sym);
	insertKeyWords(L"continue", continue_sym);
	insertKeyWords(L"decimal", dec_datatype);
	insertKeyWords(L"else", else_sym);
	insertKeyWords(L"false", false_val);
	insertKeyWords(L"for", for_sym);
	insertKeyWords(L"func", func_sym);
	insertKeyWords(L"if", if_sym);
	insertKeyWords(L"in", in_sym);
	insertKeyWords(L"integer", int_datatype);
	insertKeyWords(L"not", not_sym);
	insertKeyWords(L"odd", odd_sym);
	insertKeyWords(L"or", or_op);
	insertKeyWords(L"print", print_sym);
	insertKeyWords(L"read", read_sym);
	insertKeyWords(L"ref", ref_sym);
	insertKeyWords(L"repeat", repeat_sym);
	insertKeyWords(L"return", return_sym);
	insertKeyWords(L"switch", switch_sym);
	insertKeyWords(L"true", true_val);
	insertKeyWords(L"unless", unless_sym);
	insertKeyWords(L"until", until_sym);
	insertKeyWords(L"var", var_sym);
	insertKeyWords(L"while", while_sym);
	insertKeyWords(L"xor", xor_op);

	m_isInput = wsIn;
}

bool CLexicalParser::Next()
{
	//ִ��ǰ�ȳ�ʼ��һ�µ�ǰ������
	resetvalues();
	try
	{
		while (m_curCh == L' ' ||
			m_curCh == L'\n' ||
			m_curCh == L'\r')
			getnextc();
	}
	catch (EofException &e)
	{
		//ʲô��û�У���Ϊ�ǿյ�
		return false;
	}

	//��ͷ����ĸ�ĵ��ʣ���һ�����Ǵ���������ˡ�
	if (towlower(m_curCh) >= L'a' && towlower(m_curCh) <= L'z')
	{
		try
		{
			do
			{
				m_wstrCurSymbol.push_back(m_curCh);
				getnextc();
			} while (towlower(m_curCh) >= L'a' && towlower(m_curCh) <= L'z' && m_isInput);
		}
		catch (EofException &e) {}
		//TODO: Other error?

		if (m_setReserved.count(m_wstrCurSymbol) > 0)
		{
			//m_isInput->putback(m_curCh);
			m_curSymbolType = m_mwSym[m_wstrCurSymbol];
			return true;
		}
		else
		{
			//��Ϊ��ʶ���������»��ߵȣ���˵���һ���ո�֮ǰ��
			//����Ҫ�����������ӡ�

			try
			{
				while (m_curCh != L' ' &&
					m_curCh != L'\n' &&
					m_curCh != L'\r')
				{
					if (towlower(m_curCh) >= L'a' && towlower(m_curCh) <= L'z' ||
						m_curCh == L'_') {
						m_wstrCurSymbol.push_back(m_curCh);
						getnextc();
					}
					else break;
				}
			}
			catch (EofException &e) {}
			//TODO: Other error?

			//m_isInput->putback(m_curCh);
			m_curSymbolType = ident_type;
			return true;
		}
	}
	//�������ֵĲ���
	else if ((m_curCh >= L'0' && m_curCh <= L'9') || m_curCh == L'.')
	{
		bool isInteger = true,
			intOverflow = false;
		__int64 intvalue = 0, newintvalue = 0;
		double decvalue = 0.0;
		if (m_curCh != L'.')
		{
			if (m_curCh == L'0')
			{
				m_wstrCurSymbol.push_back(m_curCh);
				try
				{
					getnextc();
					if (m_curCh >= L'0' && m_curCh <= L'9')
					{
						//TODO: ����������������ͷ�ж����ǰ��0.
						return false;
					}
					else if (m_curCh == L'.' || towlower(m_curCh) == L'e') //����С����, ���� E ��ת�Ƶ�������	
					{
						if (towlower(m_curCh) == L'e')
							m_isInput->putback(m_curCh);
						else
							m_wstrCurSymbol.push_back(m_curCh);
						isInteger = false;
					}
					else if (towlower(m_curCh) >= L'a' && towlower(m_curCh) <= L'z') //ֱ��������ĸ������
					{
						//TODO: Error
						return false;
					}
					//m_isInput->putback(m_curCh);
				}
				catch (EofException &e) {}
				//TODO: Other error handler?

				m_curSymbolType = int_val;
				m_curSymbolInt = intvalue;
				return true;
			}
			else
			{
				do //�������ַ����򲻶�ѭ��
				{
					m_wstrCurSymbol.push_back(m_curCh);
					intvalue = newintvalue;
					newintvalue *= 10;
					newintvalue += __int64(m_curCh - L'0');
					if (newintvalue < intvalue) //��������Խ�磬��ô��ת�Ƶ�����
					{
						decvalue = double(intvalue) * 10.0 + double(m_curCh - L'0');
						isInteger = false;
						intOverflow = true;
						// ��� int64 �治�£���ô�������λ��Ҳ���ü�����
						break;
					}
					getnextc();
				} while (m_curCh >= L'0' && m_curCh <= L'9');
				if (isInteger)
				{
					intvalue = newintvalue;
					if (m_curCh == L'.' || towlower(m_curCh) == L'e') //����С���� -> С��
					{
						if (towlower(m_curCh) == L'e')
							m_isInput->putback(m_curCh);
						else
							m_wstrCurSymbol.push_back(m_curCh);
						decvalue = double(intvalue);
						isInteger = false;
					}
					else if (towlower(m_curCh) >= L'a' && towlower(m_curCh) <= L'z') //ֱ��������ĸ������
					{
						//TODO: Error
						return false;
					}
					else //������ķ���
					{
						//m_isInput->putback(m_curCh);
						m_curSymbolType = int_val;
						m_curSymbolInt = intvalue;
						return true;
					}
				}
				else
				{
					//��Ϊ��������Ե���һ��.֮ǰ�����е����ֶ�Ҫ����

					try
					{
						getnextc();
						while (m_curCh >= L'0' && m_curCh <= L'9')
						{
							m_wstrCurSymbol.push_back(m_curCh);
							decvalue *= 10;
							decvalue += double(m_curCh - L'0');
							getnextc();
						}
					}
					catch (EofException &e) {}
					//TODO: Other error handler?
				}
			}
		}
		else { //����ֱ���� . ��ͷ����˼���� 0.��ͷ��С��
			//����һ�ֿ��ܾ��� ... ���ţ��������Ҫ�ų���

			isInteger = false;
			m_wstrCurSymbol.push_back(m_curCh);
			bool eof = false;
			try {
				getnextc();
			}
			catch (EofException &e) {
				eof = true;
			}
			if (m_curCh == L'.') //����.��
			{
				m_wstrCurSymbol.push_back(m_curCh);
				eof = false;
				try { getnextc(); }
				catch (EofException &e) {
					eof = true;
				}
				if (eof || m_curCh != L'.') //�������� .. 
				{
					// TODO: Error
					return false;
				}
				m_wstrCurSymbol.push_back(m_curCh);
				m_curSymbolType = to_op;
				try { getnextc(); }
				catch (EofException &e) {}
				return true;
			}
			if (eof || m_curCh < L'0' || m_curCh > L'9')
			{
				// ֱ��һ�� . �ǲ�����ģ�����
				// TODO: Error
				return false;
			}
		}

		if (!isInteger)
		{
			double digit = 0.1;
			try
			{
				getnextc();
				while (m_curCh >= L'0' && m_curCh <= L'9')
				{
					m_wstrCurSymbol.push_back(m_curCh);
					if (!intOverflow)
					{
						decvalue += digit * double(m_curCh - L'0');
						digit /= 10;
					}
					getnextc();
				}
			}
			catch (EofException &e) {}

			if (towlower(m_curCh) == L'e')
			{
				int sign = 1;
				int exp_i = 0;
				bool eof = false;
				m_wstrCurSymbol.push_back(m_curCh);
				try {
					getnextc();
				}
				catch (EofException &e) { eof = true; }

				if (eof || (m_curCh != L'+' && m_curCh != L'-' &&
					(m_curCh < L'0' || m_curCh > L'9')))
				{
					//TODO: Error
					return false;
				}
				else if (m_curCh == L'+')
				{
					m_wstrCurSymbol.push_back(m_curCh);
				}
				else if (m_curCh == L'-')
				{
					m_wstrCurSymbol.push_back(m_curCh);
					sign = -1;
				}
				else
					m_isInput->putback(m_curCh);

				getnextc();
				while (m_curCh >= L'0' && m_curCh <= L'9')
				{
					m_wstrCurSymbol.push_back(m_curCh);
					exp_i *= 10;
					exp_i += int(m_curCh - L'0');
					if (exp_i > 1000) //λ��̫�࣬����1000��ֱ�ӱ���
					{
						//TODO: Error
						return false;
					}
					try
					{
						getnextc();
					}
					catch (EofException &e) { break; }
				}
				decvalue *= pow(10.0, double(exp_i * sign));
				if (decvalue == HUGE_VAL || decvalue == NAN)
				{
					// ��������ˣ��޷�����
					return false;
				}

				//������������ֺ��治��ֱ�Ӹ�����ĸ
				if (towlower(m_curCh) >= L'a' && towlower(m_curCh) <= L'z')
				{
					//TODO: Error
					return false;
				}

			}
			else if (towlower(m_curCh) >= L'a' && towlower(m_curCh) <= L'z')
			{
				//������������ֺ��治��ֱ�Ӹ�����ĸ
				//TODO: Error
				return false;
			}

			//m_isInput->putback(m_curCh);
			m_curSymbolType = dec_val;
			m_curSymbolDec = decvalue;
			return true;
		}
		//TODO:
		//�쳣���� 
		return false;
	}
	// �����ǣ��Ǿ��Ƿ�����

	m_wstrCurSymbol.push_back(m_curCh);
	switch (m_curCh)
	{
	case L';': //���������ţ��������ֱ�Ӹ��κ��ַ�
		m_curSymbolType = semicolon_op;
		break;
	case L'=': //��ֵ���ߵ��ڷ��ţ��������ֱ�Ӹ��κ��ַ�
		m_curSymbolType = eql_op;
		break;
	case L',':
		m_curSymbolType = comma_op;
		break;
	case L'(':
		m_curSymbolType = lparen_op;
		break;
	case L')':
		m_curSymbolType = rparen_op;
		break;
	case L'{':
		m_curSymbolType = lbrace_op;
		break;
	case L'}':
		m_curSymbolType = rbrace_op;
		break;
	case L'*':
		m_curSymbolType = times_op;
		break;
	case L'/':
		m_curSymbolType = slash_op;
		break;
	case L'%':
		m_curSymbolType = mod_op;
		break;
	case L':':
		m_curSymbolType = colon_op;
		break;
	case L'+':
	case L'-':
		try {
			wchar_t wcBegin = m_curCh;
			int iCnt = 1;

			getnextc();
			while (m_curCh == wcBegin)
			{
				iCnt++;
				m_wstrCurSymbol.push_back(m_curCh);
				getnextc();
			}

			if (iCnt >= 3)
			{
				// ������ +++..., ---... ��������ѭ����ȥ
				return false;
			}
			else if (iCnt == 2)
			{
				switch (wcBegin)
				{
				case L'+':
					m_curSymbolType = dblplus_op;
					break;
				case L'-':
					m_curSymbolType = dblminus_op;
					break;
				}
				return true;
			}
			else
				m_isInput->putback(m_curCh);
		}
		catch (EofException &e) {}
		m_curSymbolType = plus_op;
		break;
	case '!':
	case '<':
	case '>':
	{
		wchar_t wcBegin = m_curCh;
		try
		{
			getnextc();
			bool isNeq = ((wcBegin == L'!' && m_curCh == L'=') ||
				(wcBegin == L'<' && m_curCh == L'>')),
				isLeq = (wcBegin == L'<' && m_curCh == L'='),
				isGeq = (wcBegin == L'>' && m_curCh == L'=');

			if (isNeq || isLeq || isGeq)
			{
				if (isNeq)
					m_curSymbolType = neq_op;
				else if (isLeq)
					m_curSymbolType = leq_op;
				else if (isGeq)
					m_curSymbolType = geq_op;
				m_wstrCurSymbol.push_back(m_curCh);
				try { getnextc(); }
				catch (EofException &e) {}
				return true;
			}
		}
		catch (EofException &e) {}
		if (wcBegin == '!')
		{
			// TODO: Error;
			return false;
		}
		else if (wcBegin == L'<')
			m_curSymbolType = lss_op;
		else if (wcBegin == L'>')
			m_curSymbolType = gtr_op;
		return true;
	}
	default:
		return false;
	}
	try { getnextc(); }
	catch (EofException &e) {}
	return true;
}

const CLexicalParser::SymbolType CLexicalParser::GetSymbolType() const
{
	return m_curSymbolType;
}

const __int64 CLexicalParser::GetInteger() const
{
	return m_curSymbolInt;
}

const double CLexicalParser::GetDecimal() const
{
	return m_curSymbolDec;
}

const wchar_t * CLexicalParser::GetSymbol() const
{
	return m_wstrCurSymbol.c_str();
}

void CLexicalParser::insertKeyWords(const std::wstring &word, SymbolType symb)
{
	m_setReserved.insert(word);
	m_mwSym[word] = symb;
}

void CLexicalParser::error()
{
	m_curSymbolType = nul;
}

void CLexicalParser::resetvalues()
{
	m_curSymbolType = nul;
	m_curSymbolInt = 0;
	m_curSymbolDec = 0.0;
	m_wstrCurSymbol.clear();
}

void CLexicalParser::getnextc()
{
	m_isInput->get(m_curCh);
	if (m_isInput->eof())
		throw EofException();
	if (m_isInput->bad() || m_isInput->fail())
		throw StreamFailedException();
}

